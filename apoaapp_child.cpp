/*#############################################################################
#
# Copyright (C) 2018 Nick Jones <nick.fa.jones@gmail.com>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <string>
#include <memory>
#include <system_error>
#include <functional>

#include <asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/application_handler.hpp>
#include <libapoa/process_handler.hpp>
#include <libapoa/signal_handler.hpp>


//#############################################################################
class myapp : public std::enable_shared_from_this<myapp>
  {
  public:
    explicit myapp(asio::io_service& io_service) :
      io_service_(io_service),
      sigint_handler_(io_service),
      process_handler_(io_service),
      timer_(io_service)
      {
      APOA_PRINT("myapp::myapp");
      }

    ~myapp()
      {
      APOA_PRINT("myapp::~myapp");

      apoa::shutdown_process();
      }

  public:
    static void process_start(
        const std::error_code& ec,
        asio::io_service& io_service)
      {
      assert(!ec);

      std::shared_ptr<myapp> app(new myapp(io_service));
      io_service.post(std::bind(&myapp::start, app));
      }

    void start()
      {
      APOA_PRINT("myapp::on_application_start");

      sigint_handler_.handle(SIGINT);
      sigint_handler_.async_wait(
        std::bind(
          &myapp::on_sigint, shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2));

      timer_.expires_from_now(
        boost::posix_time::seconds(5));
      timer_.async_wait(
        std::bind(
          &myapp::on_timeout, shared_from_this(),
          std::placeholders::_1));
      }

  public:
    void on_timeout(const std::error_code& ec)
      {
      if (ec)
        {
        if (ec != asio::error::operation_aborted)
          {
          APOA_PRINT("myapp::on_timeout error %s", ec.message().data());
          }

        return;
        }

      APOA_PRINT("myapp::on_timeout");

      apoa::process_context context;
      context.exectuable_file_path("/usr/bin/env");

      std::error_code ec2;
      process_handler_.launch(context, ec2);
      if (ec)
        {
        APOA_PRINT("  launch error %s", ec2.message().data());

        return;
        }

      process_handler_.async_wait_exit(
        std::bind(
          &myapp::on_child_exit, shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2));

      process_handler_.async_pipe_read(
        asio::buffer(buffer_.data(), buffer_.size() - 1),
        std::bind(
          &myapp::on_child_read, shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2));
      }

  public:
    void on_child_read(
        const std::error_code& ec, std::size_t readed)
      {
      if (ec)
        {
        if (ec != asio::error::operation_aborted)
          {
          APOA_PRINT("myapp::on_child_read error %s", ec.message().data());
          }

        return;
        }

      buffer_[readed] = '\0';

      APOA_PRINT("%s", buffer_.data());

      process_handler_.async_pipe_read(
        asio::buffer(buffer_.data(), buffer_.size() - 1),
        std::bind(
          &myapp::on_child_read, shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2));
      }

  public:
    void on_child_exit(
        const std::error_code& ec,
        apoa::basic_process_context::status_type)
      {
      if (ec)
        {
        if (ec != asio::error::operation_aborted)
          {
          APOA_PRINT("myapp::on_child_exit error %s", ec.message().data());
          }

        return;
        }

      APOA_PRINT("myapp::on_child_exit");

      process_handler_.close_pipe_read();
      }

  public:
    void on_sigint(
        const std::error_code& ec, apoa::siginfo sigint_info)
      {
      APOA_PRINT("myapp::on_sigint");

      if (ec)
        {
        if (ec != asio::error::operation_aborted)
          {
          APOA_PRINT("myapp::on_sigint error %s", ec.message().data());
          }

        return;
        }

      sigint_handler_.cancel();
      timer_.cancel();
      }

  private:
    asio::io_service& io_service_;

    apoa::signal_handler sigint_handler_;

    apoa::process_handler process_handler_;

    asio::deadline_timer timer_;

    std::array<char, 1025> buffer_;
  };

//#############################################################################
int main(int argc, char** argv)
  {
  boost::program_options::options_description desc("test options");
  desc.add_options()
    ("help", "");

  asio::io_service io_service;
  apoa::application_handler app_handler(io_service);

  app_handler.process_init(desc, argc, argv);

  int retval = app_handler.process_start(
    std::bind(
      &myapp::process_start,
      std::placeholders::_1,
      std::placeholders::_2));

  alarm(0);

  return retval;
  }
