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

#include <string>
#include <list>
#include <map>
#include <signal.h>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
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
std::string& basic_process_context::working_directory()
  {
  return details_->working_directory;
  }

void basic_process_context::working_directory(const std::string& path)
  {
  details_->working_directory = path;
  }

//#############################################################################
std::string& basic_process_context::executable_file_path()
  {
  return details_->executable_file_path;
  }

void basic_process_context::exectuable_file_path(const std::string& file_path)
  {
  details_->executable_file_path = file_path;
  }

//#############################################################################
basic_process_context::environment_type& basic_process_context::environment()
  {
  return details_->environment;
  }

boost::shared_array<char*> basic_process_context::get_envp()
  {
  boost::shared_array<char*> envp(new char*[details_->environment.size()+1]);
  size_t counter = 0;

  details_->env_name_value.reset(
    new std::string[details_->environment.size()]);

  for (environment_iterator_type iter = details_->environment.begin();
    iter != details_->environment.end(); iter++, counter++)
    {
    details_->env_name_value[counter] = iter->first + "=" + iter->second;
    envp[counter] =
      const_cast<char*>(details_->env_name_value[counter].data());
    }
  if (counter ==  details_->environment.size())
    {
    envp[counter] = NULL;
    }

  return envp;
  }

void basic_process_context::environment_set(
  const std::string& name, const std::string& value)
  {
  details_->environment.insert(std::pair<std::string, std::string>(
    name, value));
  }

void basic_process_context::environment_unset(const std::string& name)
  {
  details_->environment.erase(name);
  }

basic_process_context::environment_iterator_type
  basic_process_context::environment_begin()
  {
  return details_->environment.begin();
  }

basic_process_context::environment_iterator_type
  basic_process_context::environment_end()
  {
  return details_->environment.end();
  }

//#############################################################################
basic_process_context::args_type& basic_process_context::args()
  {
  return details_->args;
  }

boost::shared_array<char*> basic_process_context::get_args()
  {
  boost::shared_array<char*> argv(new char*[details_->args.size()+1]);
  size_t counter = 0;

  for (args_iterator_type iter = details_->args.begin();
    iter != details_->args.end(); iter++, counter++)
    {
    argv[counter] = const_cast<char*>(iter->data());
    }
  if (counter == details_->args.size())
    {
    argv[counter] = NULL;
    }

  return argv;
  }

void basic_process_context::args_add(const std::string& arg)
  {
  details_->args.push_back(arg);
  }

basic_process_context::args_iterator_type basic_process_context::args_begin()
  {
  return details_->args.begin();
  }

basic_process_context::args_iterator_type basic_process_context::args_end()
  {
  return details_->args.end();
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
