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

#ifndef LIBAPOA_APPLICATION_HANDLER_IMPL_HPP
#define LIBAPOA_APPLICATION_HANDLER_IMPL_HPP

#include <sys/types.h>

#include <map>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <libapoa/common.hpp>
#include <libapoa/thread_handler.hpp>


namespace apoa
{

//#############################################################################
class application_handler_impl :
    public boost::enable_shared_from_this<application_handler_impl>
  {
  public:
    explicit application_handler_impl(boost::asio::io_service& io_service) :
      thread_handler_(io_service),
      watchdog_timer_(io_service),
      watchdog_length_(0),
      process_retval_(0),
      is_started_(false)
      {
      if (!is_process_thread())
        {
        BOOST_ASSERT(0 &&
          "application_handler: must be initialised in the process thread");
        }
      }
    ~application_handler_impl()
      {
      }

  public:
    void process_init(
        boost::program_options::options_description& options_desc,
        int argc, char** argv)
      {
      boost::program_options::store(
        boost::program_options::parse_command_line(
          argc, argv, options_desc), options_map_);
      boost::program_options::notify(options_map_);
      }

    bool process_has_option(const std::string& option_name)
      {
      return options_map_.count(option_name) > 0;
      }

    boost::program_options::variable_value get_process_option(
        const std::string& option_name)
      {
      return options_map_[option_name];
      }

    int process_start(thread_callback handler)
      {
      if (is_started_)
        {
        BOOST_ASSERT(0 &&
          "basic_application_handler: process already initialised");

        return -1;
        }

      is_started_ = true;

      process_retval_ = thread_handler_.start_main_thread(handler);

      return process_retval_;
      }

    void set_watchdog(uint32_t seconds)
      {
      if (seconds == 0)
        {
        watchdog_timer_.cancel();
        alarm(0);
        return;
        }

      if (seconds == 1)
        {
        seconds = 2;
        }

      boost::system::error_code ec;
      on_watchdog_timeout(ec);
      }

  private:
    void on_watchdog_timeout(const boost::system::error_code& ec)
      {
      if (ec)
        {
        return;
        }

      watchdog_timer_.expires_from_now(
          boost::posix_time::seconds(watchdog_length_/2));
      watchdog_timer_.async_wait(
        boost::bind(
          &application_handler_impl::on_watchdog_timeout, shared_from_this(),
        boost::asio::placeholders::error));

      alarm(watchdog_length_);
      }

  private:
    thread_handler thread_handler_;

  private:
    boost::asio::deadline_timer watchdog_timer_;
    uint32_t watchdog_length_;

  private:
    boost::program_options::variables_map options_map_;

  private:
    int process_retval_;

  private:
    bool is_started_;
  };

}; // namespace apoa

#endif // LIBAPOA_APPLICATION_HANDLER_IMPL_HPP
