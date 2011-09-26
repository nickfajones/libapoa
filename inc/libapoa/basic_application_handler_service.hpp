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

#ifndef LIBAPOA_BASIC_APPLICATION_HANDLER_SERVICE_HPP
#define LIBAPOA_BASIC_APPLICATION_HANDLER_SERVICE_HPP

#include <libapoa/application_handler_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_application_handler_service :
    public boost::asio::detail::service_base<
      basic_application_handler_service>
  {
  public:
    typedef boost::shared_ptr<apoa::application_handler_impl>
      implementation_type;
    
  public:
    explicit basic_application_handler_service(
      boost::asio::io_service& io_service);
    virtual ~basic_application_handler_service();
    
  public:
    virtual void shutdown_service();
    void construct(implementation_type& impl);
    void destroy(implementation_type& impl);
    
  public:
    template <typename ProcessStartHandler>
    void process_start(
        implementation_type& impl,
        boost::asio::io_service& process_io_service,
        ProcessStartHandler handler)
      {
      impl->process_start(process_io_service, handler);
      }

  public:
    void process_init(
      implementation_type& impl,
      boost::program_options::options_description& options_desc,
      int argc, char** argv);

    int get_process_retval(implementation_type& impl);
    boost::asio::io_service& get_process_io_service(
      implementation_type& impl);
    void set_watchdog(implementation_type& impl, uint32_t seconds);

    void shutdown_process(implementation_type& impl, int retval);

  private:
    boost::asio::io_service& io_service_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_APPLICATION_HANDLER_SERVICE_HPP
