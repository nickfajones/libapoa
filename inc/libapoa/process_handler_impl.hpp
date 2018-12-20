/*#############################################################################
#
# Copyright (C) 2012 Network Box Corporation Limited
#   Nick Jones <nick.jones@network-box.com>
#   Jeff He <jeff.he@network-box.com>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#ifndef LIBAPOA_PROCESS_HANDLER_IMPL_HPP
#define LIBAPOA_PROCESS_HANDLER_IMPL_HPP

#include <system_error>
#include <functional>

#include <libapoa/signal_handler.hpp>

#include <asio.hpp>


namespace apoa
{

//#############################################################################
class process_handler_impl :
    public std::enable_shared_from_this<process_handler_impl>
  {
  public:
    typedef std::function<
        void (const std::error_code&,
              apoa::basic_process_context::status_type)>
      basic_process_exit_callback;
    typedef std::function<
        void (const std::error_code&, std::size_t)>
      async_pipe_read_callback;
    typedef std::function<
        void (const std::error_code&, std::size_t)>
      async_pipe_write_callback;
    
  public:
    explicit process_handler_impl(asio::io_service& io_service);
    ~process_handler_impl();
    
  public:
    void launch(
      basic_process_context& context, std::error_code& ec);
    void exec(
      basic_process_context& context, std::error_code& ec);
    
    void cancel(std::error_code& ec);
    void signal(
      basic_process_context::signal_type signal_code,
      std::error_code& ec);
    
    void async_wait_exit(basic_process_exit_callback callback);
    
    template <typename MutableBuffer>
    void async_pipe_read(
      MutableBuffer buffer, async_pipe_read_callback handler)
      {
      if (async_reading_)
        {
        return;
        }
      
      async_reading_ = true;
      read_descriptor_.async_read_some(
        buffer,
        std::bind(
          &process_handler_impl::async_pipe_read_handle,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            handler));
      }
    
    template <typename ConstBuffer>
    void async_pipe_write(
      ConstBuffer buffer, async_pipe_write_callback handler)
      {
      if (async_writing_)
        {
        return;
        }
      
      async_writing_ = true;
      write_descriptor_.async_write_some(
        buffer,
        std::bind(
          &process_handler_impl::async_pipe_write_handle,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            handler));
      }
    
  public:
    void cancel_pipe_read();
    void cancel_pipe_write();
    void close_pipe_read();
    void close_pipe_write();
    
  private:
    bool is_file_executable(const std::string& file,
      std::error_code& ec);
    
    void create_pipe(int pipe_one[], int pipe_two[],
      std::error_code& ec);
    void exec_in_child(int pipe_one[], int pipe_two[]);
    
    void async_pipe_read_handle(
      const std::error_code& ec, std::size_t bytes_transferred,
      async_pipe_read_callback handler);
    void async_pipe_write_handle(
      const std::error_code& ec, std::size_t bytes_transferred,
      async_pipe_write_callback handler);
    
    void sigchld_handle(const std::error_code& ec,
      apoa::siginfo sigint_info);
    
  private:
    asio::io_service& io_service_;
    
    asio::posix::stream_descriptor read_descriptor_;
    asio::posix::stream_descriptor write_descriptor_;
    
    signal_handler sigchld_handler_;
    
    basic_process_context context_;
    basic_process_exit_callback wait_exit_callback_;
    
    bool child_active_;
    bool async_reading_;
    bool async_writing_;
  };

}; // namespace apoa

#endif // LIBAPOA_PROCESS_HANDLER_IMPL_HPP
