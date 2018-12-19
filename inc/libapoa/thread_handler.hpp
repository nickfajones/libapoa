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

#ifndef LIBAPOA_THREAD_HANDLER_HPP
#define LIBAPOA_THREAD_HANDLER_HPP

#include <asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/thread_handler_types.hpp>



//#############################################################################
#define APOA_CHILD_SHUTDOWN_COUNTDOWN_DEFAULT             10
#define APOA_PROCESS_SHUTDOWN_COUNTDOWN_DEFAULT           30


namespace apoa
{

//#############################################################################
class thread_handler
  {
  private:
    class thread_registration
      {
      public:
        thread_registration();
        ~thread_registration();

      public:
        pid_t tid_;

      public:
        unsigned int shutdown_countdown_;
        int retval_;

      public:
        thread_callback start_handler_;

      public:
        boost::thread* system_thread_;

      public:
        asio::io_service* thread_io_service_;
        asio::io_service::work* thread_work_;

      public:
        bool is_process_;
        bool is_shutdown_started_;
        bool is_shutting_down_;
      };

  public:
    explicit thread_handler(asio::io_service& io_service);
    ~thread_handler();

  public:
    int start_main_thread(thread_callback handler);
    void create_thread(thread_callback handler);

  private:
    static void on_thread_created(thread_registration* registration);
    static void run_thread(thread_registration* registration);

  public:
    static void shutdown_thread_tid(pid_t tid, int retval);
    static void shutdown_thread(tenum_t tenum, int retval);

  private:
    static void shutdown_thread_initial(apoa::tenum_t tenum);
    static void join_thread(apoa::tenum_t tenum);

  public:
    static asio::io_service& get_io_service_tid(
      pid_t tid = get_tid());
    static asio::io_service& get_io_service(
      tenum_t tenum = get_tenum());

  private:
    static per_thread_index<thread_registration> thread_registry_;

  private:
    static boost::mutex tid_tenum_map_mutex_;
    static std::map<pid_t, tenum_t> tid_tenum_map_;

  private:
    asio::io_service& io_service_;
  };

}; // namespace apoa

#endif // LIBAPOA_THREAD_HANDLER_HPP
