/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#   nick.jones@network-box.com
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <string>
#include <memory>
#include <system_error>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/application_handler.hpp>
#include <libapoa/thread_handler.hpp>
#include <libapoa/signal_handler.hpp>


//#############################################################################
class mythread : public std::enable_shared_from_this<mythread>
  {
  public:
    explicit mythread(asio::io_service& io_service) :
      m_tenum(apoa::get_tenum()),
      timer_(io_service)
      //sigusr1_handler_(io_service)
      {
      APOA_PRINT("mythread::mythread");
      }

    ~mythread()
      {
      APOA_PRINT("mythread::~mythread");

      //sigusr1_handler_.cancel();
      timer_.cancel();

      apoa::shutdown_thread(m_tenum);
      }

  public:
    void cancel()
      {
      timer_.cancel();
      }

  public:
    void start_timer()
      {
      APOA_PRINT("mythread::start_timer");

      timer_.expires_from_now(
        boost::posix_time::seconds(5));
      timer_.async_wait(
        boost::bind(
          &mythread::on_timeout, shared_from_this(),
          asio::placeholders::error));
/*
      sigusr1_handler_.handle(SIGUSR1);
      sigusr1_handler_.async_wait(
        boost::bind(
          &mythread::on_sigusr1, shared_from_this(),
          asio::placeholders::error,
          _2));
*/
      }

  public:
    void on_timeout(
        const std::error_code& ec)
      {
      if (ec)
        {
        return;
        }

      APOA_PRINT("mythread::on_timeout");

      timer_.expires_from_now(
        boost::posix_time::seconds(5));
      timer_.async_wait(
        boost::bind(
          &mythread::on_timeout, shared_from_this(),
          asio::placeholders::error));
      }

  public:
/*
    void on_sigusr1(
        const std::error_code& ec, apoa::siginfo sigusr1_info)
      {
      APOA_PRINT("mythread::on_sigusr1");

      if (ec)
        {
        return;
        }

      sigusr1_handler_.cancel();
      timer_.cancel();
      }
*/
  private:
    apoa::tenum_t m_tenum;
    asio::deadline_timer timer_;

    //apoa::signal_handler sigusr1_handler_;
  };



//#############################################################################
class myapp : public std::enable_shared_from_this<myapp>
  {
  public:
    explicit myapp(asio::io_service& io_service) :
      io_service_(io_service),
      timer_(io_service),
      thread_handler_(io_service),
      sigint_handler_(io_service),
      sigusr1_handler_(io_service),
      sigusr2_handler_(io_service),
      threads__starting_(0),
      threads__max_(3)
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
      io_service.post(boost::bind(&myapp::start, app));
      }

    void start()
      {
      APOA_PRINT("myapp::on_application_start");

      sigint_handler_.handle(SIGINT);
      sigint_handler_.async_wait(
        boost::bind(
          &myapp::on_sigint, shared_from_this(),
          asio::placeholders::error,
          _2));

      sigusr1_handler_.handle(SIGUSR1);
      sigusr1_handler_.async_wait(
        boost::bind(
          &myapp::on_sigusr1, shared_from_this(),
          asio::placeholders::error,
          _2));

      sigusr2_handler_.handle(SIGUSR2);
      sigusr2_handler_.async_wait(
        boost::bind(
          &myapp::on_sigusr2, shared_from_this(),
          asio::placeholders::error,
          _2));

      timer_.expires_from_now(
        boost::posix_time::seconds(5));
      timer_.async_wait(
        boost::bind(
          &myapp::on_timeout, shared_from_this(),
          asio::placeholders::error));
      }

  public:
    void on_thread_started(std::shared_ptr<mythread> thread)
      {
      threads_.push_back(thread);
      }

    void on_thread_start(
        const std::error_code& ec,
        asio::io_service& io_service)
      {
      assert(!ec);

      std::shared_ptr<mythread> thread(new mythread(io_service));
      thread->start_timer();

      io_service_.post(
        boost::bind(&myapp::on_thread_started, shared_from_this(), thread));
      }

  public:
    void on_timeout(const std::error_code& ec)
      {
      if (ec)
        {
        return;
        }

      APOA_PRINT("myapp::on_timeout");

      if (threads__starting_ < threads__max_)
        {
        ++threads__starting_;

        APOA_PRINT(
          "  starting thread: '%u' out of '%u'",
            threads__starting_, threads__max_);

        thread_handler_.create_thread(
          boost::bind(
            &myapp::on_thread_start, shared_from_this(),
            asio::placeholders::error, _2));
        }

      timer_.expires_from_now(
        boost::posix_time::seconds(5));
      timer_.async_wait(
        boost::bind(
          &myapp::on_timeout, shared_from_this(),
          asio::placeholders::error));
      }

  public:
    void on_test(int p)
      {
      APOA_PRINT("myapp::on_test %d", p);
      }

    void on_sigusr1(
        const std::error_code& ec, apoa::siginfo sigusr1_info)
      {
      APOA_PRINT("myapp::on_sigusr1");

      if (ec)
        {
        sigusr2_handler_.cancel();
        sigusr1_handler_.cancel();
        sigint_handler_.cancel();
        timer_.cancel();

        return;
        }
      }

    void on_sigusr2(
        const std::error_code& ec, apoa::siginfo sigusr1_info)
      {
      APOA_PRINT("myapp::on_sigusr2");

      if (ec)
        {
        sigusr2_handler_.cancel();
        sigusr1_handler_.cancel();
        sigint_handler_.cancel();
        timer_.cancel();

        return;
        }
      }

    void on_sigint(
        const std::error_code& ec, apoa::siginfo sigint_info)
      {
      APOA_PRINT("myapp::on_sigint");

      if (ec)
        {
        APOA_PRINT("  error %s", ec.message().data());
        return;
        }

      sigusr2_handler_.cancel();
      sigusr1_handler_.cancel();
      sigint_handler_.cancel();
      timer_.cancel();

      while (!threads_.empty())
        {
        threads_.back()->cancel();
        threads_.pop_back();
        }
      }

  private:
    asio::io_service& io_service_;

    asio::deadline_timer timer_;

    apoa::thread_handler thread_handler_;

    apoa::signal_handler sigint_handler_;
    apoa::signal_handler sigusr1_handler_;
    apoa::signal_handler sigusr2_handler_;

    std::vector<std::shared_ptr<mythread> > threads_;

    unsigned int threads__starting_;
    unsigned int threads__max_;
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
    boost::bind(&myapp::process_start, _1, _2));

  alarm(0);

  return retval;
  }
