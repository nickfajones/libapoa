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


namespace apoa
{

//#############################################################################
class application_handler_impl :
    public boost::enable_shared_from_this<application_handler_impl>
  {
  public:
    friend boost::asio::io_service& get_process_io_service();
    friend void shutdown_process(int);

    friend bool process_has_option(const std::string& option_name);
    friend boost::program_options::variable_value
      get_process_option(const std::string& option_name);

    friend boost::asio::io_service& get_io_service(pid_t tid);
    friend void shutdown_thread(pid_t tid, int retval);

  public:
    application_handler_impl(boost::asio::io_service& io_service);
    ~application_handler_impl();

  public:
    void process_start(
      boost::asio::io_service& process_io_service,
      apoa::application_callback handler);
    void process_init(
      boost::program_options::options_description& options_desc,
      int argc, char** argv);

    int get_process_retval();
    boost::asio::io_service& get_process_io_service();
    void set_watchdog(uint32_t seconds);

    void shutdown_process(int retval);

  private:
    void on_watchdog_timeout(const boost::system::error_code& ec);

  private:
    struct details
      {
      public:
        boost::asio::io_service* process_io_service_;
        bool is_initialised_;
        boost::program_options::variables_map options_map_;
      };

  private:
    boost::shared_ptr<details> process_details_;
    const pid_t pid_;
    boost::asio::deadline_timer watchdog_timer_;
    uint32_t watchdog_length_;
  };

}; // namespace apoa

#endif // LIBAPOA_APPLICATION_HANDLER_IMPL_HPP
