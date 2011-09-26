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

#ifndef LIBAPOA_BASIC_APPLICATION_HANDLER_HPP
#define LIBAPOA_BASIC_APPLICATION_HANDLER_HPP

#include <sys/types.h>

#include <boost/bind.hpp>
#include <boost/call_traits.hpp>

#include <libapoa/common.hpp>
#include <libapoa/basic_application_handler_service.hpp>


namespace apoa
{

//#############################################################################
class basic_application_handler :
    public boost::asio::basic_io_object<
      basic_application_handler_service>
  {
  public:
    explicit basic_application_handler(
      boost::asio::io_service& io_service);
    virtual ~basic_application_handler();

  public:
    template <typename StartHandler>
    int process_start(StartHandler handler)
      {
      pre_start();
      
      this->service.process_start(
        this->implementation,
        get_process_io_service(),
        handler);
      
      return this->service.get_process_retval(this->implementation);
      }
    
    template <typename StartTargetType>
    int process_start(
        void (StartTargetType::*method)
          (const boost::system::error_code&),
        StartTargetType* target)
      {
      pre_start();
      
      if (target == NULL)
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(
            &basic_application_handler::new_instance_callback_helper<
                StartTargetType,
                void (StartTargetType::*)
                  (const boost::system::error_code&)>,
              method,
              _1));
        }
      else
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(method, target, _1));
        }
      
      return this->service.get_process_retval(this->implementation);
      }
    
    template <typename StartTargetType,
              typename A2>
    int process_start(
        void (StartTargetType::*method)
          (const boost::system::error_code&, A2),
        StartTargetType* target,
        typename boost::call_traits<A2>::param_type a2)
      {
      pre_start();
      
      if (target == NULL)
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(
            &basic_application_handler::new_instance_callback_helper<
                StartTargetType,
                void (StartTargetType::*)
                  (const boost::system::error_code&, A2),
                typename boost::call_traits<A2>::param_type>,
              method,
              _1, a2));
        }
      else
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(method, target, _1, a2));
        }
      
      return this->service.get_process_retval(this->implementation);
      }
    
    template <typename StartTargetType,
              typename A2, typename A3>
    int process_start(
        void (StartTargetType::*method)
          (const boost::system::error_code&, A2, A3),
        StartTargetType* target,
        typename boost::call_traits<A2>::param_type a2,
        typename boost::call_traits<A3>::param_type a3)
      {
      pre_start();
      
      if (target == NULL)
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(
            &basic_application_handler::new_instance_callback_helper<
                StartTargetType,
                void (StartTargetType::*)
                  (const boost::system::error_code&, A2, A3),
                typename boost::call_traits<A2>::param_type,
                typename boost::call_traits<A3>::param_type>,
              method,
              _1, a2, a3));
        }
      else
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(method, target, _1, a2, a3));
        }
      
      return this->service.get_process_retval(this->implementation);
      }
    
  private:
    template <typename TargetType, typename TargetMethod>
    static void new_instance_callback_helper(
        TargetMethod method,
        const boost::system::error_code& ec)
      {
      boost::shared_ptr<TargetType> target(
        new TargetType(apoa::get_io_service()));
      
      (*target.*method)(ec);
      }
    
    template <typename TargetType, typename TargetMethod,
              typename A2>
    static void new_instance_callback_helper(
        TargetMethod method,
        const boost::system::error_code& ec,
        typename boost::call_traits<A2>::param_type a2)
      {
      boost::shared_ptr<TargetType> target(
        new TargetType(apoa::get_io_service()));
      
      (*target.*method)(ec, a2);
      }
    
    template <typename TargetType, typename TargetMethod,
              typename A2, typename A3>
    static void new_instance_callback_helper(
        TargetMethod method,
        const boost::system::error_code& ec,
        typename boost::call_traits<A2>::param_type a2,
        typename boost::call_traits<A3>::param_type a3)
      {
      boost::shared_ptr<TargetType> target(
        new TargetType(apoa::get_io_service()));
      
      (*target.*method)(ec, a2, a3);
      }

  public:
    void process_init(
      boost::program_options::options_description& options_desc,
      int argc, char** argv);

    boost::asio::io_service& get_process_io_service();
    void set_watchdog(uint32_t seconds);

    void shutdown_process(int retval = 0);

  private:
    void pre_start();

  private:
    bool is_started_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_APPLICATION_HANDLER_HPP
