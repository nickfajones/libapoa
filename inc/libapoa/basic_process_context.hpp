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

#ifndef LIBAPOA_BASIC_PROCESS_CONTEXT_HPP
#define LIBAPOA_BASIC_PROCESS_CONTEXT_HPP

#include <string>
#include <list>
#include <map>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>


namespace apoa
{

//#############################################################################
class basic_process_context
  {
  public:
    typedef std::map<std::string, std::string> env_type;
    typedef env_type::const_iterator env_iterator;
    typedef std::list<std::string> args_type;
    typedef args_type::const_iterator args_iterator;

    typedef pid_t handle_type;
    typedef int signal_type;
    typedef int status_type;

  public:
    static const signal_type signal_interrupt;
    static const signal_type signal_terminate;
    static const signal_type signal_kill;
    static const signal_type signal_quit;
    static const signal_type signal_stop;
    static const signal_type signal_continue;
    static const signal_type signal_hangup;
    static const signal_type signal_userdefined1;
    static const signal_type signal_userdefined2;

  public:
    basic_process_context();
    basic_process_context(const basic_process_context& rvalue);
    ~basic_process_context();

  public:
    const std::string& working_directory();
    void working_directory(const std::string& path);

    const std::string& executable_file_path();
    void exectuable_file_path(const std::string& file_path);

  public:
    void env_set(const std::string& name, const std::string& value);


    const env_type& env();
    char* const* get_env_array();

  public:
    void args_add(const std::string& arg);

    const args_type& args();
    char* const* get_args_array();

  public:
    handle_type handle();
    void handle(handle_type child_handle);

  private:
    struct details
      {
      std::string working_directory;
      std::string executable_file_path;

      env_type env;
      std::vector<const char*> env_array;

      args_type args;
      std::vector<const char*> args_array;

      handle_type handle;
      status_type status;
      };

    boost::shared_ptr<details> details_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_PROCESS_CONTEXT_HPP
