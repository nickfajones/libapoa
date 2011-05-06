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

#include <sys/types.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/basic_thread_handler_service.hpp>


namespace apoa
{

//#############################################################################
boost::asio::io_service& get_process_io_service();
boost::asio::io_service& get_io_service(pid_t tid = 0);

void shutdown_thread(pid_t tid = apoa::get_tid(), int retval = 0);
void shutdown_process(int retval = 0);



//#############################################################################
class basic_thread_handler :
    public boost::asio::basic_io_object<basic_thread_handler_service>
  {
  public:
    friend boost::asio::io_service& get_process_io_service();
    friend boost::asio::io_service& get_io_service(pid_t);
    
    friend void shutdown_thread(pid_t, int);
    friend void shutdown_process(int);
    
  public:
    explicit basic_thread_handler(boost::asio::io_service& io_service);
    virtual ~basic_thread_handler();
    
  public:
    void process_init(int argc, char** argv);
    
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
            &basic_thread_handler::new_instance_callback_helper<
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
        typename value_carrier<A2>::param_type a2)
      {
      pre_start();
      
      if (target == NULL)
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(
            &basic_thread_handler::new_instance_callback_helper<
                StartTargetType,
                void (StartTargetType::*)
                  (const boost::system::error_code&, A2),
                typename value_carrier<A2>::param_type>,
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
        typename value_carrier<A2>::param_type a2,
        typename value_carrier<A3>::param_type a3)
      {
      pre_start();
      
      if (target == NULL)
        {
        this->service.process_start(
          this->implementation,
          get_process_io_service(),
          boost::bind(
            &basic_thread_handler::new_instance_callback_helper<
                StartTargetType,
                void (StartTargetType::*)
                  (const boost::system::error_code&, A2, A3),
                typename value_carrier<A2>::param_type,
                typename value_carrier<A3>::param_type>,
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
    
  public:
    boost::asio::io_service& get_process_io_service();
    boost::asio::io_service& get_io_service(pid_t tid = 0);
    
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
        typename value_carrier<A2>::param_type a2)
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
                typename value_carrier<A2>::param_type>,
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
        typename value_carrier<A2>::param_type a2,
        typename value_carrier<A3>::param_type a3)
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
                typename value_carrier<A2>::param_type,
                typename value_carrier<A3>::param_type>,
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
    
  public:
    void shutdown_thread(int retval = 0);
    void shutdown_process(int retval = 0);
    
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
        typename value_carrier<A2>::param_type a2)
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
        typename value_carrier<A2>::param_type a2,
        typename value_carrier<A3>::param_type a3)
      {
      boost::shared_ptr<TargetType> target(
        new TargetType(apoa::get_io_service()));
      
      (*target.*method)(ec, a2, a3);
      }
    
  private:
    void pre_start();
    void shutdown_thread(pid_t tid, int retval);
    
  private:
    class details;
    
  private:
    boost::shared_ptr<details> process_details_;
    
    pid_t tid_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_HANDLER_HPP
