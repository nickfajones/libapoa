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

#ifndef LIBAPOA_PROCESS_HANDLER_IMPL_HPP
#define LIBAPOA_PROCESS_HANDLER_IMPL_HPP

#include <libapoa/signal_handler.hpp>


namespace apoa
{

//#############################################################################
class process_handler_impl :
    public boost::enable_shared_from_this<process_handler_impl>
  {
  public:
    typedef boost::function<
        void (const boost::system::error_code&,
              apoa::basic_process_context::status_type)>
      basic_process_exit_callback;
    typedef boost::function<
        void (const boost::system::error_code&, std::size_t)>
      async_pipe_read_callback;
    typedef boost::function<
        void (const boost::system::error_code&, std::size_t)>
      async_pipe_write_callback;
    
  public:
    explicit process_handler_impl(boost::asio::io_service& io_service);
    ~process_handler_impl();
    
  public:
    void deactivate();

    void launch(
      basic_process_context& context, boost::system::error_code& ec);
    void exec(
      basic_process_context& context, boost::system::error_code& ec);

    void cancel(boost::system::error_code& ec);
    void signal(
      basic_process_context::signal_type signal_code,
      boost::system::error_code& ec);

    void async_wait_exit(basic_process_exit_callback callback);

    template <typename MutableBuffer>
    void async_pipe_read(
      MutableBuffer buffer, async_pipe_read_callback handler)
      {
      if (async_reading_)
        {
        io_service_.post(boost::bind(
          &process_handler_impl::async_pipe_read<MutableBuffer>,
          this, buffer, handler));
        return;
        }

      async_reading_ = true;
      boost::asio::async_read(read_descriptor_, buffer,
        boost::bind(&process_handler_impl::async_pipe_read_handle,
        this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred,
        handler, shared_from_this()));
      }

    template <typename ConstBuffer>
    void async_pipe_write(
      ConstBuffer buffer, async_pipe_write_callback handler)
      {
      if (async_writing_)
        {
        io_service_.post(boost::bind(
          &process_handler_impl::async_pipe_write<ConstBuffer>,
          this, buffer, handler));
        return;
        }

      async_writing_ = true;
      boost::asio::async_write(write_descriptor_, buffer,
        boost::bind(&process_handler_impl::async_pipe_write_handle,
        this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred,
        handler, shared_from_this()));
      }

  public:
    void cancel_pipe_read();
    void cancel_pipe_write();
    void close_pipe_read();
    void close_pipe_write();

  private:
    bool is_file_executable(const std::string& file,
      boost::system::error_code& ec);

    void create_pipe(int pipe_one[], int pipe_two[],
      boost::system::error_code& ec);
    void exec_in_child(int pipe_one[], int pipe_two[],
      boost::system::error_code& ec);

    void async_pipe_read_handle(
      const boost::system::error_code& ec, std::size_t bytes_transferred,
      async_pipe_read_callback handler,
      boost::shared_ptr<process_handler_impl> self);
    void async_pipe_write_handle(
      const boost::system::error_code& ec, std::size_t bytes_transferred,
      async_pipe_write_callback handler,
      boost::shared_ptr<process_handler_impl> self);

    void sigchld_handle(const boost::system::error_code& ec,
      apoa::siginfo sigint_info,
      boost::shared_ptr<process_handler_impl> self);

  private:
    boost::asio::io_service& io_service_;
    
    boost::asio::posix::stream_descriptor read_descriptor_;
    boost::asio::posix::stream_descriptor write_descriptor_;
    
    signal_handler sigchld_handler_;
    
    basic_process_context context_;
    basic_process_exit_callback wait_exit_callback_;
    
    bool exit_callback_;
    bool child_active_;
    bool async_reading_;
    bool async_writing_;
  };

}; // namespace apoa

#endif // LIBAPOA_PROCESS_HANDLER_IMPL_HPP
