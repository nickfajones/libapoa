/*#############################################################################
#
# Copyright (C) 2012 Network Box Corporation Limited
#   Nick Jones <nick.jones@network-box.com>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#ifndef LIBAPOA_BASIC_APPLICATION_HANDLER_SERVICE_HPP
#define LIBAPOA_BASIC_APPLICATION_HANDLER_SERVICE_HPP

#include <libapoa/application_handler_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_application_handler_service :
    public asio::detail::service_base<basic_application_handler_service>
  {
  public:
    typedef std::shared_ptr<apoa::application_handler_impl>
      implementation_type;

  public:
    explicit basic_application_handler_service(
        asio::io_context& io_context) :
      asio::detail::service_base<
        basic_application_handler_service>(io_context),
      io_context_(io_context)
      {
      }
    virtual ~basic_application_handler_service()
      {
      }

  public:
    virtual void shutdown_service()
      {
      }
    void construct(implementation_type& impl)
      {
      impl.reset(new application_handler_impl(io_context_));
      }
    void destroy(implementation_type& impl)
      {
      impl.reset();
      }

  public:
    void process_init(
        implementation_type& impl,
        boost::program_options::options_description& options_desc,
        int argc, char** argv)
      {
      impl->process_init(options_desc, argc, argv);
      }

    bool process_has_option(
        implementation_type& impl, const std::string& option_name)
      {
      return impl->process_has_option(option_name);
      }

    boost::program_options::variable_value get_process_option(
        implementation_type& impl, const std::string& option_name)
      {
      return impl->get_process_option(option_name);
      }

    int process_start(implementation_type& impl, thread_callback handler)
      {
      return impl->process_start(handler);
      }

    void set_watchdog(implementation_type& impl, uint32_t seconds)
      {
      impl->set_watchdog(seconds);
      }

  private:
    asio::io_context& io_context_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_APPLICATION_HANDLER_SERVICE_HPP
