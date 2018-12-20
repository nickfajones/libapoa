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

#ifndef LIBAPOA_COMMON_H
#define LIBAPOA_COMMON_H

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <list>
#include <set>
#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <asio.hpp>


namespace apoa
{

//#############################################################################
class tenum_t
  {
  public:
    tenum_t(int tenum) : tenum_(tenum) {}
    tenum_t(const tenum_t& rvalue) : tenum_(rvalue.tenum_) {}
    ~tenum_t() {tenum_ = -1;}

  public:
    tenum_t& operator=(const tenum_t& rvalue)
      {
      tenum_ = rvalue.tenum_;
      return *this;
      }

  public:
    bool operator<(const tenum_t& rvalue) const {return tenum_ < rvalue.tenum_;}

  public:
    bool operator==(const tenum_t& rvalue) const {return tenum_ == rvalue.tenum_;}
    bool operator!=(const tenum_t& rvalue) const {return tenum_ != rvalue.tenum_;}

  public:
    int tenum_;
  };



//#############################################################################
pid_t get_pid();
pid_t get_tid();
tenum_t get_tenum();
bool is_process_thread();



//#############################################################################
enum log_level
  {
  log_level_none =                                        0,
  log_level_info =                                        10,
  log_level_debug =                                       100,
  log_level_warning =                                     200,
  log_level_error =                                       500,
  log_level_critical =                                    1000,
  };



//#############################################################################
int get_log_level();
void set_log_level(int level);



//#############################################################################
namespace priv
{
template <typename ARG>
static void push_log_message_arg(
    boost::format& message, const ARG& arg)
  {
  message % arg;
  }

template <typename ARG, typename... ARGS>
static void push_log_message_arg(
    boost::format& message, const ARG& arg, const ARGS&... args)
  {
  message % arg;

  push_log_message_arg(message, args...);
  }
};

//#############################################################################
template <typename... ARGS>
void apoavsprintf(
    std::string& str, const std::string& fmt, const ARGS&... args)
  {
  try
    {
    boost::format message(fmt);
    priv::push_log_message_arg(message, args...);
    str = boost::str(message);
    }
  catch(std::exception const& e)
    {
    str = boost::str(
      boost::format("format error '%1s'") % fmt.data());
    }
  }

inline void apoavsprintf(
    std::string& str, const std::string& fmt, va_list& args)
  {
  str.resize(4096, '\0');
  vsnprintf(const_cast<char*>(str.data()), 4096, fmt.c_str(), args);
  }

//#############################################################################
inline void apoalogstdout(int level, const std::string& message)
  {
  if ((apoa::get_log_level() == 0) || (apoa::get_log_level() <= level))
    {
    return;
    }

  struct timeval tv;
  gettimeofday(&tv, NULL);

  char tm_buf[10];
  if (strftime(tm_buf, 9, "%T", localtime(&tv.tv_sec)) == 0) \
    {
    printf("[%05d] unable to generate apoa::log timestamp\n", get_tid());
    fflush(stdout);

    return;
    }

  std::string prefix = boost::str(boost::format("[%1$05ld] %2$s:%3$06ld") %
    get_tid() % tm_buf % tv.tv_usec);

  std::list<std::string> lines;
  boost::split(lines, message, boost::is_any_of("\n"));
  for (std::list<std::string>::iterator itr = lines.begin();
       itr != lines.end();
       ++itr)
    {
    printf("%s %s\n", prefix.c_str(), itr->c_str());
    fflush(stdout);
    }
  }

inline void apoalogstdout(
    int level, const std::string& fmt, va_list& args)
  {
  if ((apoa::get_log_level() == 0) || (apoa::get_log_level() <= level))
    {
    return;
    }

  std::string str;
  apoavsprintf(str, fmt, args);

  apoalogstdout(level, str);
  }

template <typename... ARGS>
void apoalogstdout(
    int level, const std::string& fmt, const ARGS&... args)
  {
  if ((apoa::get_log_level() == 0) || (apoa::get_log_level() <= level))
    {
    return;
    }

  std::string str;
  apoavsprintf(str, fmt, args...);

  apoalogstdout(level, str);
  }

//#############################################################################
#define APOA_PRINT(fmt, ...) \
  apoa::apoalogstdout(apoa::log_level_info, fmt, ##__VA_ARGS__);



//#############################################################################
asio::io_service& get_process_io_service();
void shutdown_process(int retval = 0);

asio::io_service& get_io_service(apoa::tenum_t tenum = apoa::get_tenum());
void shutdown_thread(apoa::tenum_t tenum = apoa::get_tenum(), int retval = 0);

asio::io_service& get_io_service(pid_t tid);
void shutdown_thread(pid_t tid, int retval);

asio::io_service& get_thread_io_service();



//#############################################################################
class thread_handler;



namespace priv
{

//#############################################################################
class per_thread_registry
  {
  public:
    friend class apoa::thread_handler;

  private:
    typedef std::function<void (void)> per_thread_index_callback;
    typedef std::map<int32_t, per_thread_index_callback>
      per_thread_index_callback_map;

  private:
    static void init()
      {
      std::lock_guard<std::mutex> callback_lock(callback_mutex_);

      thread_set_.insert(0);
      }

  private:
    static void thread_start()
      {
      std::lock_guard<std::mutex> callback_lock(callback_mutex_);

      thread_set_.insert(get_tenum());

      for (per_thread_index_callback_map::iterator itr =
             thread_start_callbacks_.begin();
           itr != thread_start_callbacks_.end();
           ++itr)
        {
        (itr->second)();
        }
      }

    static void thread_finish()
      {
      std::lock_guard<std::mutex> callback_lock(callback_mutex_);

      for (per_thread_index_callback_map::iterator itr =
             thread_finish_callbacks_.begin();
           itr != thread_finish_callbacks_.end();
           ++itr)
        {
        (itr->second)();
        }

      thread_set_.erase(get_tenum());
      }

  protected:
    static int32_t index_id_;

  protected:
    static std::mutex callback_mutex_;

    static per_thread_index_callback_map thread_start_callbacks_;
    static per_thread_index_callback_map thread_finish_callbacks_;

  protected:
    static std::set<tenum_t> thread_set_;
  };

}; // namespace priv



//#############################################################################
template <typename T>
class per_thread_index : public priv::per_thread_registry
  {
  public:
    friend class thread_handler;

  public:
    const static size_t default_index_size =              10;

  public:
    per_thread_index() :
        id_(__sync_add_and_fetch(&index_id_, 1)),
        index_size_(default_index_size)
      {
      index_ = new T*[index_size_];
      memset(index_, 0, sizeof(T*) * index_size_);

      std::lock_guard<std::mutex> callback_lock(callback_mutex_);

      thread_start_callbacks_.insert(
        std::make_pair(
          id_, std::bind(&per_thread_index<T>::on_thread_start, this)));
      thread_finish_callbacks_.insert(
        std::make_pair(
          id_, std::bind(&per_thread_index<T>::on_thread_finish, this)));

      for (std::set<tenum_t>::iterator itr =
             thread_set_.begin();
           itr != thread_set_.end();
           ++itr)
        {
        do_on_thread_start(*itr, NULL);
        }
      }

    per_thread_index(int32_t id) :
        id_(id),
        index_size_(default_index_size)
      {
      index_ = new T*[index_size_];
      memset(index_, 0, sizeof(T*) * index_size_);
      }

    ~per_thread_index()
      {
      if (id_ >= 0)
        {
        std::lock_guard<std::mutex> callback_lock(callback_mutex_);

        thread_start_callbacks_.erase(id_);
        thread_finish_callbacks_.erase(id_);
        }

      std::lock_guard<std::mutex> index_lock(index_mutex_);

      for (ssize_t counter = index_size_ - 1;
           counter >= 0;
           --counter)
        {
        if (index_[counter] != NULL)
          {
          delete index_[counter];
          index_[counter] = NULL;
          }
        }

      delete [] index_;
      index_ = NULL;
      }

  public:
    T& get(apoa::tenum_t tenum = apoa::get_tenum())
      {
      return *index_[tenum.tenum_];
      }

  private:
    void on_thread_start()
      {
      do_on_thread_start(get_tenum(), NULL);
      }

    void do_on_thread_start(apoa::tenum_t tenum, T* t)
      {
      std::lock_guard<std::mutex> index_lock(index_mutex_);

      if (tenum.tenum_ >= index_size_)
        {
        T** new_index = new T*[(index_size_ * 2)];
        memset(new_index, 0, sizeof(T*) * (index_size_ * 2));
        memcpy(new_index, index_, sizeof(T*) * index_size_);

        index_size_ *= 2;

        T** old_index = index_;
        __sync_bool_compare_and_swap(&index_, index_, new_index);

        delete [] old_index;
        }

      if (t == NULL)
        {
        index_[tenum.tenum_] = new T();
        }
      else
        {
        index_[tenum.tenum_] = t;
        }
      }

    void on_thread_finish()
      {
      do_on_thread_finish(get_tenum());
      }

    void do_on_thread_finish(apoa::tenum_t tenum)
      {
      std::lock_guard<std::mutex> index_lock(index_mutex_);

      assert(index_[tenum.tenum_] != NULL);
      delete index_[tenum.tenum_];
      index_[tenum.tenum_] = NULL;
      }

  private:
    int32_t id_;

  private:
    std::mutex index_mutex_;

    size_t index_size_;

    T** index_;
  };

}; // namespace apoa

#endif // LIBAPOA_COMMON_H
