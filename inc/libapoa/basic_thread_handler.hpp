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

#ifndef LIBAPOA_BASIC_THREAD_HANDLER_HPP
#define LIBAPOA_BASIC_THREAD_HANDLER_HPP

#include <boost/call_traits.hpp>
#include <boost/program_options.hpp>

#include <libapoa/common.hpp>
#include <libapoa/basic_thread_handler_service.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_handler :
    public boost::asio::basic_io_object<basic_thread_handler_service>
  {
  public:
    friend boost::asio::io_service& get_io_service(pid_t);
    friend void shutdown_thread(pid_t, int);

    friend class application_handler_impl;

  public:
    explicit basic_thread_handler(boost::asio::io_service& io_service);
    virtual ~basic_thread_handler();
    
  public:
    template <typename CreateThreadHandler>
    void create_thread(CreateThreadHandler handler)
      {
      this->service.create_thread(
        this->implementation,
        get_io_service(),
        handler);
      }
    
    template <typename CreateThreadTargetType>
    void create_thread(
        void (CreateThreadTargetType::*method)
          (const boost::system::error_code&),
        CreateThreadTargetType* target)
      {
      if (target == NULL)
        {
        this->service.create_thread(
          this->implementation,
          get_io_service(),
          boost::bind(
            &basic_thread_handler::new_instance_callback_helper<
              CreateThreadTargetType,
                void (CreateThreadTargetType::*)
                  (const boost::system::error_code&)>,
              method,
              _1));
        }
      else
        {
        this->service.create_thread(
          this->implementation,
          get_io_service(),
          boost::bind(method, target, _1));
        }
      }
    
    template <typename StartTargetType,
              typename A2>
    void create_thread(
        void (StartTargetType::*method)
          (const boost::system::error_code&, A2),
        StartTargetType* target,
        typename boost::call_traits<A2>::param_type a2)
      {
      if (target == NULL)
        {
        this->service.create_thread(
          this->implementation,
          get_io_service(),
          boost::bind(
            &basic_thread_handler::new_instance_callback_helper<
                StartTargetType,
                void (StartTargetType::*)
                  (const boost::system::error_code&, A2),
                typename boost::call_traits<A2>::param_type>,
              method,
              _1, a2));
        }
      else
        {
        this->service.create_thread(
          this->implementation,
          get_io_service(),
          boost::bind(method, target, _1, a2));
        }
      }
    
    template <typename StartTargetType,
              typename A2, typename A3>
    void create_thread(
        void (StartTargetType::*method)
          (const boost::system::error_code&, A2, A3),
        StartTargetType* target,
        typename boost::call_traits<A2>::param_type a2,
        typename boost::call_traits<A3>::param_type a3)
      {
      if (target == NULL)
        {
        this->service.create_thread(
          this->implementation,
          get_io_service(),
          boost::bind(
            &basic_thread_handler::new_instance_callback_helper<
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
        this->service.create_thread(
          this->implementation,
          get_io_service(),
          boost::bind(method, target, _1, a2, a3));
        }
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
    boost::asio::io_service& get_io_service(pid_t tid = 0);
    void shutdown_thread(pid_t tid, int retval);

  private:
    void register_main_thread(
      boost::asio::io_service& process_io_service,
      apoa::application_callback handler);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_HANDLER_HPP
