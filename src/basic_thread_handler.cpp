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

#include <sys/types.h>

#include <boost/assert.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/program_options/parsers.hpp>

#include <libapoa/common.hpp>

#include <libapoa/basic_thread_handler_service.hpp>
#include <libapoa/basic_thread_handler.hpp>



namespace apoa
{

//#############################################################################
class basic_thread_handler::details
  {
  public:
    details() :
      process_io_service_(NULL),
      is_initialised_(false),
      is_started_(false)
      {
      }
    
    ~details()
      {
      process_io_service_ = NULL;
      }
    
  public:
    boost::asio::io_service* process_io_service_;
    
    bool is_initialised_;
    bool is_started_;

    boost::program_options::variables_map options_map_;
    
  public:
    static boost::weak_ptr<basic_thread_handler::details>
      process_details_;
  };

boost::weak_ptr<basic_thread_handler::details>
  basic_thread_handler::details::process_details_;



//#############################################################################
basic_thread_handler::basic_thread_handler(boost::asio::io_service& io_service) :
  boost::asio::basic_io_object<basic_thread_handler_service>(io_service),
  tid_(get_tid())
  {
  if (details::process_details_.use_count() == 0)
    {
    if (!is_process_thread())
      {
printf("pid: '%d', tid: '%d'\n", get_pid(), get_tid());
      BOOST_ASSERT(0 && "basic_thread_handler: must be initialised in the process thread");
      
      return;
      }
    
    process_details_.reset(new details);
    details::process_details_ = process_details_;
    
    process_details_->process_io_service_ = &io_service;
    }
  else
    {
    process_details_ = details::process_details_.lock();
    }
  }

basic_thread_handler::~basic_thread_handler()
  {
  }

//#############################################################################
void basic_thread_handler::process_init(
    boost::program_options::options_description& options_desc,
    int argc, char** argv)
  {
  if (process_details_->is_initialised_)
    {
    BOOST_ASSERT(0 && "basic_thread_handler: process already initialised");
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
    BOOST_ASSERT(0 && "basic_thread_handler: program option error");
    }

  }

//#############################################################################
boost::asio::io_service& basic_thread_handler::get_process_io_service()
  {
  return *process_details_->process_io_service_;
  }

boost::asio::io_service& basic_thread_handler::get_io_service(pid_t tid)
  {
  if (tid == 0)
    {
    tid = get_tid();
    }
  
  return service.get_thread_io_service(implementation, tid);
  }

//#############################################################################
void basic_thread_handler::shutdown_thread(int retval)
  {
  shutdown_thread(tid_, retval);
  }

void basic_thread_handler::shutdown_process(int retval)
  {
  service.shutdown_thread(implementation, get_pid(), retval);
  }

//#############################################################################
void basic_thread_handler::pre_start()
  {
  if (process_details_->is_started_)
    {
    BOOST_ASSERT(0 && "basic_thread_handler: process already started");
    }
  
  process_details_->is_started_ = true;
  }

void basic_thread_handler::shutdown_thread(pid_t tid, int retval)
  {
  service.shutdown_thread(implementation, tid, retval);
  }

//#############################################################################
boost::asio::io_service& get_process_io_service()
  {
  if (basic_thread_handler::details::process_details_.use_count() == 0)
    {
    BOOST_ASSERT(0 && "get_process_io_service: basic_thread_handler not yet initialised");
    }
  
  return
    *basic_thread_handler::details::process_details_.lock()->process_io_service_;
  }

boost::asio::io_service& get_io_service(pid_t tid)
  {
  if (basic_thread_handler::details::process_details_.use_count() == 0)
    {
    BOOST_ASSERT(0 && "get_io_service: basic_thread_handler not yet initialised");
    }
  
  return
    basic_thread_handler(
      *basic_thread_handler::details::process_details_.lock()->
        process_io_service_).get_io_service(tid);
  }

//#############################################################################
void shutdown_thread(pid_t tid, int retval)
  {
  if (basic_thread_handler::details::process_details_.use_count() == 0)
    {
    BOOST_ASSERT(0 && "shutdown_thread: basic_thread_handler not yet initialised");
    }
  
  basic_thread_handler(
    *basic_thread_handler::details::process_details_.lock()->
      process_io_service_).shutdown_thread(tid, retval);
  }

void shutdown_process(int retval)
  {
  if (basic_thread_handler::details::process_details_.use_count() == 0)
    {
    BOOST_ASSERT(0 && "shutdown_process: basic_thread_handler not yet initialised");
    }
  
  basic_thread_handler(
    *basic_thread_handler::details::process_details_.lock()->
      process_io_service_).shutdown_process(retval);
  }

bool process_has_option(const std::string& option_name)
  {
  return (basic_thread_handler::details::process_details_.lock()->
    options_map_.count(option_name) > 0);
  }

//#############################################################################
boost::program_options::variable_value get_process_option(
    const std::string& option_name)
  {
  if (basic_thread_handler::details::process_details_.use_count() == 0)
    {
    BOOST_ASSERT(0 && "get_process_option: basic_thread_handler not yet initialised");
    }

  return basic_thread_handler::details::process_details_.lock()->
    options_map_[option_name];
  }

}; // namespace apoa
