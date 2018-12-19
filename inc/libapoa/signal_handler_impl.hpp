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

#ifndef LIBAPOA_SIGNAL_HANDLER_IMPL_HPP
#define LIBAPOA_SIGNAL_HANDLER_IMPL_HPP

#include <signal.h>

#include <memory>
#include <system_error>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <asio.hpp>

#include <libapoa/basic_signal_handler_siginfo.hpp>


namespace apoa
{

//#############################################################################
class signal_handler_base_impl :
    public std::enable_shared_from_this<signal_handler_base_impl>
  {
  public:
    class sigaction_handler;
    
  public:
    friend class sigaction_handler;
    
  protected:
    typedef boost::function<
      void (const std::error_code&, struct basic_siginfo)>
        basic_signal_callback;
    
    typedef asio::posix::stream_descriptor_service::native_handle_type
      basic_signal_descriptor;
    
  public:
    explicit signal_handler_base_impl(asio::io_service& io_service);
    ~signal_handler_base_impl();
    
  public:
    void deactivate();

  public:
    std::size_t cancel(std::error_code& ec);

    void handle(int signum);
    void handle(int signum, std::error_code& ec);
    
    void async_wait(basic_signal_callback callback);
    
  public:
    void async_unhandle();
    void unhandle(std::error_code& ec);

  public:
    class sigaction_key : public boost::noncopyable
      {
      public:
        sigaction_key(int signum);
        sigaction_key(const sigaction_key& rvalue);
        ~sigaction_key();
        
      public:
        bool operator<(const sigaction_key& rvalue) const;
        
      public:
        int signum_;
        
        mutable struct sigaction old_sa_;
      };
    
  public:
    class sigaction_handler :
        public boost::noncopyable,
        public std::enable_shared_from_this<sigaction_handler>
      {
      protected:
        sigaction_handler(asio::io_service& io_service);
        
      public:
        virtual ~sigaction_handler();
        
      public:
        void init();
        
      public:
        virtual basic_signal_descriptor& open_descriptor() = 0;
        virtual void close_descriptor() = 0;
        
      public:
        virtual void block_signals() = 0;
        virtual void unblock_signals() = 0;
        
      private:
        void on_descriptor_read(
          const std::error_code& ec,
          std::size_t bytes_transferred);
        
      private:
        asio::posix::stream_descriptor signal_descriptor_;
        
        struct basic_siginfo signal_descriptor_buffer_[64];
        std::size_t buffer_end_;
      };
    
  protected:
    virtual void create_sigaction_handler(
      asio::io_service& io_service,
      std::shared_ptr<sigaction_handler>& handler) = 0;
      
  protected:
    virtual void add_sigaction(
      const sigaction_key& key, std::error_code& ec) = 0;
    virtual void remove_sigaction(
      const sigaction_key& key, std::error_code& ec) = 0;
    
  private:
    int signum_;
    
    asio::io_service& io_service_;

    boost::mutex active_mutex_;
    basic_signal_callback callback_;
    bool active_;
  };



//#############################################################################
class posix_signal_handler_impl : public signal_handler_base_impl
  {
  public:
    explicit posix_signal_handler_impl(
      asio::io_service& io_service);
    virtual ~posix_signal_handler_impl();
    
  protected:
    class posix_sigaction_handler :
        public signal_handler_base_impl::sigaction_handler
      {
      public:
        posix_sigaction_handler(asio::io_service& io_service);
        virtual ~posix_sigaction_handler();
        
      public:
        virtual basic_signal_descriptor& open_descriptor();
        virtual void close_descriptor();
              
      public:
        virtual void block_signals();
        virtual void unblock_signals();
      };
    
  protected:
    virtual void create_sigaction_handler(
      asio::io_service& io_service,
      std::shared_ptr<sigaction_handler>& handler);
    
  protected:
    virtual void add_sigaction(
      const sigaction_key& key, std::error_code& ec);
    virtual void remove_sigaction(
      const sigaction_key& key, std::error_code& ec);
    
  private:
    static void handle_sigaction(int signum, siginfo_t* info, void* data);
  };



//#############################################################################
class signalfd_signal_handler_impl : public signal_handler_base_impl
  {
  public:
    explicit signalfd_signal_handler_impl(
      asio::io_service& io_service);
    virtual ~signalfd_signal_handler_impl();
    
  protected:
    class signalfd_sigaction_handler :
        public signal_handler_base_impl::sigaction_handler
      {
      public:
        signalfd_sigaction_handler(asio::io_service& io_service);
        virtual ~signalfd_sigaction_handler();
        
      public:
        virtual basic_signal_descriptor& open_descriptor();
        virtual void close_descriptor();
              
      public:
        virtual void block_signals();
        virtual void unblock_signals();
      };
    
  protected:
    virtual void create_sigaction_handler(
      asio::io_service& io_service,
      std::shared_ptr<sigaction_handler>& handler);
    
  protected:
    virtual void add_sigaction(
      const sigaction_key& key, std::error_code& ec);
    virtual void remove_sigaction(
      const sigaction_key& key, std::error_code& ec);
  };

}; // namespace apoa

#endif // LIBAPOA_SIGNAL_HANDLER_IMPL_HPP
