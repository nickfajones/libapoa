/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#   nick.jones@network-box.com
#   jeff.he@network-box.com
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <boost/filesystem/v3/operations.hpp>

#include "basic_process_context.hpp"
#include "process_handler_impl.hpp"


namespace apoa
{

//#############################################################################
process_handler_impl::process_handler_impl(
    boost::asio::io_service& io_service) :
  io_service_(io_service),
  read_descriptor_(io_service),
  write_descriptor_(io_service),
  sigchld_handler_(io_service),
  wait_exit_callback_(NULL),
  child_active_(false),
  async_reading_(false),
  async_writing_(false)
  {
  }

process_handler_impl::~process_handler_impl()
  {
  boost::system::error_code ec;
  cancel(ec);
  if (ec)
    {
    boost::asio::detail::throw_error(ec, "~process_handler_impl");
    }
  }

//#############################################################################
void process_handler_impl::launch(
    basic_process_context& context, boost::system::error_code& ec)
  {
  int pipe_one[2] = {-1, -1};
  int pipe_two[2] = {-1, -1};
  pid_t child_pid = 0;

  do
    {
    // Assign local context
    context_ = context;
    
    // Verify if file executable
    if (!is_file_executable(context_.executable_file_path(), ec))
      {
      if (!ec)
        {
        ec.assign(ENOTSUP, boost::system::get_system_category());
        }
      
      break;
      }
    
    // Create SIGCHLD signal handler
    sigchld_handler_.handle(SIGCHLD);
    sigchld_handler_.async_wait(boost::bind(
      &process_handler_impl::sigchld_handle, shared_from_this(),
      boost::asio::placeholders::error, _2));
    
    // Create pipe
    create_pipe(pipe_one, pipe_two, ec);
    if (ec)
      {
      break;
      }
    
    // Do fork
    io_service_.notify_fork(boost::asio::io_service::fork_prepare);
    child_pid = fork();
    
    // Child process of fork
    if (child_pid == 0)
      {
      exec_in_child(pipe_one, pipe_two);
      break;
      }
    
    // fork fails
    if (child_pid == -1)
      {
      ec.assign(errno, boost::system::get_system_category());
      break;
      }
    
    // Parent process
    io_service_.notify_fork(boost::asio::io_service::fork_parent);

    // Assign context with child process id
    context_.handle(child_pid);

    child_active_ = true;

    // Set fds
    int parent_read_fd = pipe_one[0];
    int child_write_fd = pipe_one[1];
    int child_read_fd = pipe_two[0];
    int parent_write_fd = pipe_two[1];

    // Assign to asio descriptor
    read_descriptor_.assign(parent_read_fd, ec);
    if (ec)
      {
      break;
      }

    write_descriptor_.assign(parent_write_fd, ec);
    if (ec)
      {
      break;
      }

    // Close child fds
    if (close(child_read_fd) != 0)
      {
      ec.assign(errno, boost::system::get_system_category());
      break;
      }

    if (close(child_write_fd) != 0)
      {
      ec.assign(errno, boost::system::get_system_category());
      break;
      }

    } while (false);
  }

void process_handler_impl::exec(
    basic_process_context& context, boost::system::error_code& ec)
  {
  // Assign local context
  context_ = context;

  // Verify if file executable
  if (!is_file_executable(context_.executable_file_path(), ec))
    {
    if (!ec)
      {
      ec.assign(ENOTSUP, boost::system::get_system_category());
      }

    return;
    }

  // Execute when no environment set
  if (context_.environment().size() == 0)
    {
    if (execv(context_.executable_file_path().c_str(),
          context_.get_args().get()) == -1)
      {
      ec.assign(errno, boost::system::get_system_category());
      }
    
    return;
    }

  // Execute when use define some environment
  if (execve(context_.executable_file_path().c_str(),
        context_.get_args().get(), context_.get_envp().get()) == -1)
    {
    ec.assign(errno, boost::system::get_system_category());
    }
  }

//#############################################################################
void process_handler_impl::cancel(boost::system::error_code& ec)
  {
  if (read_descriptor_.is_open())
    {
    // Cancel read asynchronous operations
    read_descriptor_.cancel(ec);
    read_descriptor_.close(ec);
    async_reading_ = false;
    }
  
  if (write_descriptor_.is_open())
    {
    write_descriptor_.cancel(ec);
    write_descriptor_.close(ec);
    async_writing_ = false;
    }
  
  if (!child_active_)
    {
    return;
    }

  // Kill child process
  kill(context_.handle(), SIGKILL);
  child_active_ = false;
  }

//#############################################################################
void process_handler_impl::signal(
  basic_process_context::signal_type signal_code,
  boost::system::error_code& ec)
  {
  // Send signal to child process
  if (kill(context_.handle(), signal_code) != 0)
    {
    ec.assign(errno, boost::system::get_system_category());
    }
  }

//#############################################################################
void process_handler_impl::async_wait_exit(
  basic_process_exit_callback callback)
  {
  wait_exit_callback_ = callback;
  }

//#############################################################################
void process_handler_impl::create_pipe(
    int pipe_one[], int pipe_two[], boost::system::error_code& ec)
  {
  int fcntl_flags = -1;
  
  do
    {
    // Create pipe1
    if (pipe(pipe_one) != 0)
      {
      ec.assign(errno, boost::system::get_system_category());
      break;
      }
    
    // Create pipe2
    if (pipe(pipe_two) != 0)
      {
      ec.assign(errno, boost::system::get_system_category());
      break;
      }
    
    // Set parent fds non-block and close on exec
    fcntl_flags = fcntl(pipe_one[0], F_GETFL, 0);
    fcntl(pipe_one[0], F_SETFL, fcntl_flags | O_NONBLOCK);
    fcntl_flags = fcntl(pipe_one[0], F_GETFD, 0);
    fcntl(pipe_one[0], F_SETFD, fcntl_flags | FD_CLOEXEC);
    
    fcntl_flags = fcntl(pipe_two[1], F_GETFL, 0);
    fcntl(pipe_two[1], F_SETFL, fcntl_flags | O_NONBLOCK);
    fcntl_flags = fcntl(pipe_two[1], F_GETFD, 0);
    fcntl(pipe_two[1], F_SETFD, fcntl_flags | FD_CLOEXEC);
    
    }
  while(false);
  }

//#############################################################################
void process_handler_impl::exec_in_child(int pipe_one[], int pipe_two[])
  {
  int parent_read_fd = pipe_one[0];
  int child_write_fd = pipe_one[1];
  int child_read_fd = pipe_two[0];
  int parent_write_fd = pipe_two[1];
  
  do
    {
    // Close parent fds
    close(parent_read_fd);
    close(parent_write_fd);
    
    // Close stdin fd
    if (close(STDIN_FILENO) != 0)
      {
      break;
      }
    
    // Alias read fd as stdin
    if (dup2(child_read_fd, STDIN_FILENO) == -1)
      {
      break;
      }
    
    // Close stdout fd
    if (close(STDOUT_FILENO) != 0)
      {
      break;
      }
    
    // Alias write fd as stdout
    if (dup2(child_write_fd, STDOUT_FILENO) == -1)
      {
      break;
      }
    
    // Close child fds
    close(child_read_fd);
    close(child_write_fd);
    
    // Execute when no environment set
    if (context_.environment().size() == 0)
      {
      execv(
        context_.executable_file_path().c_str(),
        context_.get_args().get());
      }
    else
      {
      // Execute when use define some environment
      execve(
        context_.executable_file_path().c_str(),
        context_.get_args().get(), context_.get_envp().get());
      }
    }
  while (false);
  
  // if we are here, exec failed, we can only exit
  _exit(-1);
  }

//#############################################################################
void process_handler_impl::async_pipe_read_handle(
    const boost::system::error_code& ec, std::size_t bytes_transferred,
    async_pipe_read_callback handler)
  {
  async_reading_ = false;
  handler(ec, bytes_transferred);
  }

void process_handler_impl::async_pipe_write_handle(
    const boost::system::error_code& ec, std::size_t bytes_transferred,
    async_pipe_write_callback handler)
  {
  async_writing_ = false;
  handler(ec, bytes_transferred);
  }

//#############################################################################
void process_handler_impl::sigchld_handle(
    const boost::system::error_code& ec, apoa::siginfo sigint_info)
  {
  // Verify error
  if (ec)
    {
    if (ec == boost::asio::error::operation_aborted)
      {
      return;
      }

    if (wait_exit_callback_ != NULL)
      {
      io_service_.post(
        boost::bind(wait_exit_callback_, ec, sigint_info.bsi_status));

      wait_exit_callback_ = NULL;
      }

    return;
    }

  int status = 0;
  boost::system::error_code new_ec;

  // Wait
  pid_t who_died = waitpid(context_.handle(), &status, WNOHANG);

  // waitpid has error
  if (who_died == -1)
    {
    new_ec.assign(errno, boost::system::get_system_category());
    }

  // Dead child process does not belong to current thread
  if (who_died == 0)
    {
    sigchld_handler_.handle(SIGCHLD);
    sigchld_handler_.async_wait(boost::bind(
      &process_handler_impl::sigchld_handle, shared_from_this(),
      boost::asio::placeholders::error, _2));

    return;
    }

  // No matter error or not, no matter killed by user or exception,
  // now child process is to dead
  if (child_active_)
    {
    child_active_ = false;
    }

  // Call child process exit callback
  if (wait_exit_callback_ != NULL)
    {
    io_service_.post(
      boost::bind(wait_exit_callback_, new_ec, sigint_info.bsi_status));

    io_service_.post(
      boost::bind(
        wait_exit_callback_,
          boost::asio::error::operation_aborted,
          0));

    wait_exit_callback_ = NULL;
    }

  boost::system::error_code ec2;
  sigchld_handler_.cancel(ec2);
  }

//#############################################################################
void process_handler_impl::cancel_pipe_read()
  {
  if (read_descriptor_.is_open())
    {
    read_descriptor_.cancel();
    }
  }

void process_handler_impl::cancel_pipe_write()
  {
  if (write_descriptor_.is_open())
    {
    write_descriptor_.cancel();
    }
  }

void process_handler_impl::close_pipe_read()
  {
  if (read_descriptor_.is_open())
    {
    read_descriptor_.close();
    }
  }

void process_handler_impl::close_pipe_write()
  {
  if (write_descriptor_.is_open())
    {
    write_descriptor_.close();
    }
  }

//#############################################################################
bool process_handler_impl::is_file_executable(const std::string& file,
  boost::system::error_code& ec)
  {
  bool result = false;
  bool ret = false;
  boost::filesystem::path path(file);

  do
    {
    // Check if exist
    ret = boost::filesystem::exists(path, ec);
    if (!ret || ec)
      {
      if (!ec)
        {
        ec.assign(ENOENT, boost::system::get_system_category());
        }

      break;
      }

    // Check if directory
    ret = boost::filesystem::is_directory(path, ec);
    if (ret || ec)
      {
      if (!ec)
        {
        ec.assign(EISDIR, boost::system::get_system_category());
        }

      break;
      }

    // Check if regular
    ret = boost::filesystem::is_regular_file(path, ec);
    if (!ret || ec)
      {
      if (!ec)
        {
        ec.assign(ENOTSUP, boost::system::get_system_category());
        }

      break;
      }

    // Check if executable
    if (access(file.data(), F_OK | X_OK) != 0)
      {
      ec.assign(errno, boost::system::get_system_category());
      break;
      }

    result = true;
    } while (false);

  return result;
  }

}; // namespace apoa
