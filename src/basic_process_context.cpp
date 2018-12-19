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

#include <signal.h>

#include <string>
#include <list>
#include <map>
#include <memory>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include "basic_process_context.hpp"


namespace apoa
{

//#############################################################################
const basic_process_context::signal_type
  basic_process_context::signal_interrupt = SIGINT;
const basic_process_context::signal_type
  basic_process_context::signal_terminate = SIGTERM;
const basic_process_context::signal_type
  basic_process_context::signal_kill =    SIGKILL;
const basic_process_context::signal_type
  basic_process_context::signal_quit =    SIGQUIT;
const basic_process_context::signal_type
  basic_process_context::signal_stop =    SIGSTOP;
const basic_process_context::signal_type
  basic_process_context::signal_continue = SIGCONT;
const basic_process_context::signal_type
  basic_process_context::signal_hangup =  SIGHUP;
const basic_process_context::signal_type
  basic_process_context::signal_userdefined1 = SIGUSR1;
const basic_process_context::signal_type
  basic_process_context::signal_userdefined2 = SIGUSR2;



//#############################################################################
basic_process_context::basic_process_context() :
    details_(new details())
  {
  }

basic_process_context::basic_process_context(
    const basic_process_context& rvalue)
  {
  details_ = rvalue.details_;
  }

basic_process_context::~basic_process_context()
  {
  }

//#############################################################################
const std::string& basic_process_context::working_directory()
  {
  return details_->working_directory;
  }

void basic_process_context::working_directory(const std::string& path)
  {
  details_->working_directory = path;
  }

//#############################################################################
const std::string& basic_process_context::executable_file_path()
  {
  return details_->executable_file_path;
  }

void basic_process_context::exectuable_file_path(const std::string& file_path)
  {
  details_->executable_file_path = file_path;
  }

namespace
{
static const char *_n = 0;
}

//#############################################################################
const basic_process_context::env_type& basic_process_context::env()
  {
  return details_->env;
  }

char* const* basic_process_context::get_env_array()
  {
  details_->env_array.clear();
  details_->env_array.reserve(details_->env.size() + 1);

  for (env_iterator env_iter = details_->env.begin();
       env_iter != details_->env.end();
       env_iter++)
    {
    details_->env_array.push_back(env_iter->second.c_str());
    }


  details_->env_array.push_back(_n);

  details_->env_array.shrink_to_fit();

  return
    static_cast<char* const*>(const_cast<char**>(details_->env_array.data()));
  }

void basic_process_context::env_set(
  const std::string& name, const std::string& value)
  {
  details_->env.insert(std::make_pair(name, name + "=" + value));
  }

//#############################################################################
const basic_process_context::args_type& basic_process_context::args()
  {
  return details_->args;
  }

char* const* basic_process_context::get_args_array()
  {
  details_->args_array.clear();
  details_->args_array.reserve(details_->args.size() + 2);

  details_->args_array.push_back(details_->executable_file_path.c_str());

  for (args_iterator args_iter = details_->args.begin();
       args_iter != details_->args.end();
       args_iter++)
    {
    details_->args_array.push_back(args_iter->c_str());
    }

  details_->args_array.push_back(_n);

  details_->args_array.shrink_to_fit();

  return
    static_cast<char* const*>(const_cast<char**>(
      details_->args_array.data()));
  }

void basic_process_context::args_add(const std::string& arg)
  {
  details_->args.push_back(arg);
  }

//#############################################################################
basic_process_context::handle_type basic_process_context::handle()
  {
  return details_->handle;
  }

void basic_process_context::handle(handle_type child_handle)
  {
  details_->handle = child_handle;
  }

}; // namespace apoa
