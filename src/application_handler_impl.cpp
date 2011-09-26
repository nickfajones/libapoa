/*############################################################################
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

#include <signal.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include <libapoa/common.hpp>
#include <libapoa/application_handler_impl.hpp>
#include <libapoa/basic_thread_handler.hpp>


namespace apoa
{

//#############################################################################
extern boost::shared_ptr<application_handler_impl> _application_handler_impl;

int thread_process_retval_ = 0;

//#############################################################################
boost::asio::io_service& get_process_io_service()
  {
  if (_application_handler_impl.use_count() == 0)
    {
    BOOST_ASSERT(0 && "get_process_io_service: "
      "application_handler not yet initialised");
    }

  return *_application_handler_impl->process_details_->process_io_service_;
  }

void shutdown_process(int retval)
  {
  if (_application_handler_impl.use_count() == 0)
    {
    BOOST_ASSERT(0 && "shutdown_process: "
      "application_handler not yet initialised");
    }

  _application_handler_impl->shutdown_process(retval);
  }

//#############################################################################
bool process_has_option(const std::string& option_name)
  {
  if (_application_handler_impl.use_count() == 0)
    {
    BOOST_ASSERT(0 && "process_has_option: "
      "application_handler not yet initialised");
    }

  return _application_handler_impl->process_details_->
    options_map_.count(option_name) > 0;
  }

boost::program_options::variable_value get_process_option(
    const std::string& option_name)
  {
  if (_application_handler_impl.use_count() == 0)
    {
    BOOST_ASSERT(0 && "get_process_option: "
      "application_handler not yet initialised");
    }

  return _application_handler_impl->process_details_->
    options_map_[option_name];
  }



//#############################################################################
application_handler_impl::application_handler_impl(
    boost::asio::io_service& io_service) :
  process_details_(new details),
  pid_(get_pid()),
  watchdog_timer_(io_service),
  watchdog_length_(0)
  {
  if (!is_process_thread())
    {
printf("pid: '%d', tid: '%d'\n", pid_, get_tid());
    BOOST_ASSERT(0 && "application_handler: must be "
      "initialised in the process thread");
    }

  process_details_->process_io_service_ = &io_service;
  process_details_->is_initialised_ = false;
  }

application_handler_impl::~application_handler_impl()
  {
  process_details_->process_io_service_ = NULL;
  }

//#############################################################################
void application_handler_impl::process_start(
    boost::asio::io_service& process_io_service,
    apoa::application_callback handler)
  {
  basic_thread_handler(process_io_service).
    register_main_thread(process_io_service, handler);
  }

//#############################################################################
void application_handler_impl::process_init(
    boost::program_options::options_description& options_desc,
    int argc, char** argv)
  {
  if (process_details_->is_initialised_)
    {
    BOOST_ASSERT(0 && "basic_application_handler: "
      "process already initialised");
    }

  try
    {
    boost::program_options::store(boost::program_options::parse_command_line(
      argc, argv, options_desc), process_details_->options_map_);
    boost::program_options::notify(process_details_->options_map_);

    process_details_->is_initialised_ = true;
    }
  catch(std::exception &e)
    {
    printf("%s\n", e.what());
    BOOST_ASSERT(0 && "basic_application_handler: "
      "program option error");
    }
  }

//#############################################################################
int application_handler_impl::get_process_retval()
  {
  return thread_process_retval_;
  }

//#############################################################################
boost::asio::io_service& application_handler_impl::get_process_io_service()
  {
  return *process_details_->process_io_service_;
  }

//#############################################################################
void application_handler_impl::set_watchdog(uint32_t seconds)
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

  watchdog_length_ = seconds/2*2;
  watchdog_timer_.expires_from_now(
    boost::posix_time::seconds(watchdog_length_/2));
  watchdog_timer_.async_wait(
    boost::bind(&application_handler_impl::on_watchdog_timeout,
      shared_from_this(), boost::asio::placeholders::error));

  alarm(watchdog_length_);
  }

void application_handler_impl::on_watchdog_timeout(
    const boost::system::error_code& ec)
  {
  watchdog_timer_.expires_from_now(
      boost::posix_time::seconds(watchdog_length_/2));
  watchdog_timer_.async_wait(
    boost::bind(&application_handler_impl::on_watchdog_timeout,
      shared_from_this(), boost::asio::placeholders::error));

  alarm(watchdog_length_);
  }

//#############################################################################
void application_handler_impl::shutdown_process(int retval)
  {
  basic_thread_handler(*process_details_->process_io_service_).
    shutdown_thread(pid_, retval);
  }

}; // namespace apoa
