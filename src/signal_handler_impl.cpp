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

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#ifdef __linux__
#include <sys/signalfd.h>
#endif
#include <errno.h>

#include <set>
#include <map>
#include <memory>
#include <system_error>
#include <functional>

#include <boost/noncopyable.hpp>

#include <asio.hpp>

#include "application_handler.hpp"
#include "basic_signal_handler_siginfo.hpp"
#include "signal_handler_impl.hpp"


namespace apoa
{

//#############################################################################
std::mutex _signal_mutex;

typedef std::set<std::shared_ptr<signal_handler_base_impl> > requests_set;
typedef std::map<signal_handler_base_impl::sigaction_key, requests_set>
  signal_handler_map;

signal_handler_map _signal_handlers;

std::shared_ptr<signal_handler_base_impl::sigaction_handler>
  _sigaction_handler;



//#############################################################################
signal_handler_base_impl::sigaction_key::sigaction_key(int signum) :
  signum_(signum)
  {
  memset(&old_sa_, 0, sizeof(old_sa_));
  }

signal_handler_base_impl::sigaction_key::sigaction_key(
    const sigaction_key& rvalue) :
  signum_(rvalue.signum_)
  {
  memcpy(&old_sa_, &rvalue.old_sa_, sizeof(rvalue.old_sa_));
  }

signal_handler_base_impl::sigaction_key::~sigaction_key()
  {
  signum_ = 0;

  memset(&old_sa_, 0, sizeof(old_sa_));
  }

//#############################################################################
bool signal_handler_base_impl::sigaction_key::operator<(
    const sigaction_key& rvalue) const
  {
  return (signum_ < rvalue.signum_);
  }



//#############################################################################
signal_handler_base_impl::sigaction_handler::sigaction_handler(
    asio::io_context& io_context) :
  signal_descriptor_(io_context),
  buffer_end_(0)
  {
  }

signal_handler_base_impl::sigaction_handler::~sigaction_handler()
  {
  if (signal_descriptor_.is_open())
    {
    signal_descriptor_.cancel();
    signal_descriptor_.release();
    }
  }

//#############################################################################
void signal_handler_base_impl::sigaction_handler::init()
  {
  basic_signal_descriptor& descriptor = open_descriptor();
  if (descriptor == -1)
    {
    return;
    }

  signal_descriptor_.assign(descriptor);
  signal_descriptor_.async_read_some(
    asio::buffer(signal_descriptor_buffer_),
    std::bind(
      &signal_handler_base_impl::sigaction_handler::on_descriptor_read,
      shared_from_this(),
      std::placeholders::_1,
      std::placeholders::_2));
  }

//#############################################################################
void signal_handler_base_impl::sigaction_handler::on_descriptor_read(
    const std::error_code& ec, std::size_t bytes_transferred)
  {
  if (ec)
    {
    if (signal_descriptor_.is_open())
      {
      signal_descriptor_.cancel();
      signal_descriptor_.release();
      }
    close_descriptor();

    return;
    }

  std::lock_guard<std::mutex> signal_lock(_signal_mutex);

  block_signals();

  buffer_end_ += bytes_transferred;
  std::size_t offset = 0;

  while (buffer_end_ >= sizeof(struct basic_siginfo))
    {
    struct basic_siginfo* bsi = signal_descriptor_buffer_ + offset;
    offset += sizeof(struct basic_siginfo);
    buffer_end_ -= sizeof(struct basic_siginfo);

    signal_handler_map::iterator signals_itr =
      _signal_handlers.find(bsi->bsi_signo);

    if (signals_itr == _signal_handlers.end())
      {
      continue;
      }

    requests_set& requests = signals_itr->second;

    for (requests_set::iterator itr =
           requests.begin();
         itr != requests.end();
         itr++)
      {
      std::shared_ptr<signal_handler_base_impl> request = *itr;

      {
      std::lock_guard<std::mutex> active_lock(request->active_mutex_);
      if (!request->active_)
        {
        continue;
        }
      }

      std::error_code ec2;

      request->io_context_.post(
        std::bind(request->callback_, ec2, *bsi));

      request->callback_ = NULL;
      request->active_ = false;
      }
    }

  if (buffer_end_ > 0)
    {
    memcpy(
      signal_descriptor_buffer_,
      signal_descriptor_buffer_ + offset,
      buffer_end_);
    }

  signal_descriptor_.async_read_some(
    asio::buffer(
      reinterpret_cast<char*>(signal_descriptor_buffer_) + buffer_end_,
      (sizeof(struct basic_siginfo) * 64) - buffer_end_),
    std::bind(
      &signal_handler_base_impl::sigaction_handler::on_descriptor_read,
      shared_from_this(),
      std::placeholders::_1,
      std::placeholders::_2));

  unblock_signals();
  }



//#############################################################################
signal_handler_base_impl::signal_handler_base_impl(
    asio::io_context& io_context) :
  signum_(0),
  io_context_(io_context),
  active_(false)
  {
  }

signal_handler_base_impl::~signal_handler_base_impl()
  {
  signum_ = 0;
  }

//#############################################################################
void signal_handler_base_impl::deactivate()
  {
  std::lock_guard<std::mutex> active_lock(active_mutex_);

  callback_ = NULL;
  active_ = false;
  }

//#############################################################################
std::size_t signal_handler_base_impl::cancel(std::error_code& ec)
  {
  if (is_process_thread())
    {
    unhandle(ec);
    }
  else
    {
    apoa::get_process_io_context().post(
      std::bind(
        &signal_handler_base_impl::async_unhandle, shared_from_this()));
    }

  std::lock_guard<std::mutex> active_lock(active_mutex_);
  if (active_ == false)
    {
    return 0;
    }

  std::error_code ec2 = asio::error::operation_aborted;
  struct basic_siginfo bsi;

  io_context_.post(std::bind(callback_, ec2, bsi));

  callback_ = NULL;
  active_ = false;

  return 1;
  }

//#############################################################################
void signal_handler_base_impl::handle(int signum)
  {
  std::error_code ec;
  handle(signum, ec);
  }

void signal_handler_base_impl::handle(
    int signum, std::error_code& ec)
  {
  signum_ = signum;

  std::lock_guard<std::mutex> signal_lock(_signal_mutex);

  signal_handler_map::iterator signals_itr = _signal_handlers.find(
    sigaction_key(signum_));

  if (signals_itr == _signal_handlers.end())
    {
    _signal_handlers.insert(
      signal_handler_map::value_type(
        sigaction_key(signum_), requests_set()));

    if (_signal_handlers.size() == 1)
      {
      create_sigaction_handler(
        apoa::get_process_io_context(),
        _sigaction_handler);
      _sigaction_handler->init();
      }

    signals_itr = _signal_handlers.find(sigaction_key(signum_));
    const sigaction_key& request_key = signals_itr->first;

    add_sigaction(request_key, ec);
    if (ec)
      {
      _signal_handlers.erase(request_key);

      if (_signal_handlers.size() == 0)
        {
        _sigaction_handler->close_descriptor();
        _sigaction_handler.reset();
        }

      return;
      }
    }

  requests_set& requests = signals_itr->second;

  requests.insert(shared_from_this());
  }

//#############################################################################
void signal_handler_base_impl::async_wait(basic_signal_callback callback)
  {
  std::lock_guard<std::mutex> active_lock(active_mutex_);
  callback_ = callback;
  active_ = true;
  }

//#############################################################################
void signal_handler_base_impl::async_unhandle()
  {
  std::error_code ec;
  unhandle(ec);
  }

void signal_handler_base_impl::unhandle(
    std::error_code& ec)
  {
  std::lock_guard<std::mutex> signal_lock(_signal_mutex);

  signal_handler_map::iterator signals_itr = _signal_handlers.find(
    sigaction_key(signum_));

  if (signals_itr == _signal_handlers.end())
    {
    return;
    }

  const sigaction_key& request_key = signals_itr->first;
  requests_set& requests = signals_itr->second;

  if (requests.erase(shared_from_this()) == 0)
    {
    return;
    }

  if (requests.size() == 0)
    {
    remove_sigaction(request_key, ec);

    _signal_handlers.erase(request_key);
    }

  if (_signal_handlers.size() == 0)
    {
    _sigaction_handler->close_descriptor();
    _sigaction_handler.reset();
    }
  }


//#############################################################################
int _posix_signal_pipe[2] = { -1, -1 };

sigset_t _posix_process_blockset;



//#############################################################################
posix_signal_handler_impl::posix_sigaction_handler::posix_sigaction_handler(
    asio::io_context& io_context) :
  signal_handler_base_impl::sigaction_handler(io_context)
  {
  sigprocmask(0, NULL, &_posix_process_blockset);
  sigaddset(&_posix_process_blockset, SIGPIPE);
  sigprocmask(SIG_SETMASK, &_posix_process_blockset, NULL);
  }

posix_signal_handler_impl::posix_sigaction_handler::~posix_sigaction_handler()
  {
  sigdelset(&_posix_process_blockset, SIGPIPE);
  sigprocmask(SIG_SETMASK, &_posix_process_blockset, NULL);
  }

//#############################################################################
signal_handler_base_impl::basic_signal_descriptor&
    posix_signal_handler_impl::posix_sigaction_handler::open_descriptor()
  {
  if (pipe(_posix_signal_pipe))
    {
    _posix_signal_pipe[0] = -1;
    _posix_signal_pipe[1] = -1;
    }

  int fcntl_flags = fcntl(_posix_signal_pipe[0], F_GETFD);
  fcntl(_posix_signal_pipe[0], F_SETFD, fcntl_flags | FD_CLOEXEC);
  fcntl_flags = fcntl(_posix_signal_pipe[0], F_GETFL);
  fcntl(_posix_signal_pipe[0], F_SETFL, fcntl_flags | O_NONBLOCK);

  fcntl_flags = fcntl(_posix_signal_pipe[1], F_GETFD);
  fcntl(_posix_signal_pipe[1], F_SETFD, fcntl_flags | FD_CLOEXEC);
  fcntl_flags = fcntl(_posix_signal_pipe[1], F_GETFL);
  fcntl(_posix_signal_pipe[1], F_SETFL, fcntl_flags | O_NONBLOCK);

  return _posix_signal_pipe[0];
  }

void posix_signal_handler_impl::posix_sigaction_handler::close_descriptor()
  {
  if (_posix_signal_pipe[0] != -1)
    {
    close(_posix_signal_pipe[0]);
    close(_posix_signal_pipe[1]);

    _posix_signal_pipe[0] = -1;
    _posix_signal_pipe[1] = -1;
    }
  }

//#############################################################################
void posix_signal_handler_impl::posix_sigaction_handler::block_signals()
  {
  sigset_t block_sigset;
  sigfillset(&block_sigset);

  pthread_sigmask(SIG_SETMASK, &block_sigset, NULL);
  }

void posix_signal_handler_impl::posix_sigaction_handler::unblock_signals()
  {
  pthread_sigmask(SIG_SETMASK, &_posix_process_blockset, NULL);
  }



//#############################################################################
posix_signal_handler_impl::posix_signal_handler_impl(
    asio::io_context& io_context) :
  signal_handler_base_impl(io_context)
  {
  }

posix_signal_handler_impl::~posix_signal_handler_impl()
  {
  }

//#############################################################################
void posix_signal_handler_impl::create_sigaction_handler(
    asio::io_context& io_context,
    std::shared_ptr<sigaction_handler>& handler)
  {
  handler.reset(new posix_sigaction_handler(io_context));
  }

//#############################################################################
void posix_signal_handler_impl::add_sigaction(
    const sigaction_key& key, std::error_code& ec)
  {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));

  sa.sa_sigaction = &handle_sigaction;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO | SA_RESTART;

  if (sigaction(key.signum_, &sa, &key.old_sa_))
    {
    ec.assign(errno, std::system_category());
    }
  }

void posix_signal_handler_impl::remove_sigaction(
    const sigaction_key& key, std::error_code& ec)
  {
  sigaction(key.signum_, &key.old_sa_, NULL);
  }

//#############################################################################
void posix_signal_handler_impl::handle_sigaction(
    int signum, siginfo_t* info, void* data)
  {
  struct basic_siginfo temp_bsi;
  memset(&temp_bsi, 0, sizeof(temp_bsi));

#ifdef __linux__
  temp_bsi.bsi_signo = info->si_signo;
  temp_bsi.bsi_errno = info->si_errno;
  temp_bsi.bsi_code = info->si_code;
  temp_bsi.bsi_pid = info->si_pid;
  temp_bsi.bsi_uid = info->si_uid;
  temp_bsi.bsi_fd = info->si_fd;
  temp_bsi.bsi_tid = info->si_timerid;
  temp_bsi.bsi_band = info->si_band;
  temp_bsi.bsi_overrun = info->si_overrun;
  temp_bsi.bsi_status = info->si_status;
  temp_bsi.bsi_int = info->si_int;
  temp_bsi.bsi_ptr = reinterpret_cast<uint64_t>(info->si_ptr);
  temp_bsi.bsi_utime = info->si_utime;
  temp_bsi.bsi_stime = info->si_stime;
  temp_bsi.bsi_addr = reinterpret_cast<uint64_t>(info->si_addr);
  temp_bsi.bsi_ttid = get_tid();

#elif defined __APPLE__
  temp_bsi.bsi_signo = info->si_signo;
  temp_bsi.bsi_errno = info->si_errno;
  temp_bsi.bsi_code = info->si_code;
  temp_bsi.bsi_pid = info->si_pid;
  temp_bsi.bsi_uid = info->si_uid;
  temp_bsi.bsi_status = info->si_status;
  temp_bsi.bsi_addr = info->si_addr;
  memcpy(&temp_bsi.bsi_value, &info->si_value, sizeof(info->si_value));
  temp_bsi.bsi_band = info->si_band;
#endif

  if (_posix_signal_pipe[1] != -1)
    {
    write(_posix_signal_pipe[1], &temp_bsi, sizeof(struct basic_siginfo));
    }
  }



#ifdef __linux__

//#############################################################################
int _signalfd_fd = 0;

sigset_t _signalfd_process_sigset;



//#############################################################################
signalfd_signal_handler_impl::signalfd_sigaction_handler::signalfd_sigaction_handler(
    asio::io_context& io_context) :
  signal_handler_base_impl::sigaction_handler(io_context)
  {
  pthread_sigmask(SIG_SETMASK, NULL, &_signalfd_process_sigset);
  }

signalfd_signal_handler_impl::signalfd_sigaction_handler::~signalfd_sigaction_handler()
  {
  }

//#############################################################################
signal_handler_base_impl::basic_signal_descriptor&
    signalfd_signal_handler_impl::signalfd_sigaction_handler::open_descriptor()
  {
  _signalfd_fd = signalfd(
    -1, &_signalfd_process_sigset, SFD_NONBLOCK | SFD_CLOEXEC);

  return _signalfd_fd;
  }

void signalfd_signal_handler_impl::signalfd_sigaction_handler::close_descriptor()
  {
  if (_signalfd_fd != -1)
    {
    close(_signalfd_fd);

    _signalfd_fd = -1;
    }
  }

//#############################################################################
void signalfd_signal_handler_impl::signalfd_sigaction_handler::block_signals()
  {
  }

void signalfd_signal_handler_impl::signalfd_sigaction_handler::unblock_signals()
  {
  }



//#############################################################################
signalfd_signal_handler_impl::signalfd_signal_handler_impl(
    asio::io_context& io_context) :
  signal_handler_base_impl(io_context)
  {
  }

signalfd_signal_handler_impl::~signalfd_signal_handler_impl()
  {
  }

//#############################################################################
void signalfd_signal_handler_impl::create_sigaction_handler(
    asio::io_context& io_context,
    std::shared_ptr<sigaction_handler>& handler)
  {
  handler.reset(new signalfd_sigaction_handler(io_context));
  }

//#############################################################################
void signalfd_signal_handler_impl::add_sigaction(
    const sigaction_key& key, std::error_code& ec)
  {
  sigaddset(&_signalfd_process_sigset, key.signum_);

  if (pthread_sigmask(SIG_SETMASK, &_signalfd_process_sigset, NULL))
    {
    ec.assign(errno, std::system_category());

    return;
    }

  if (signalfd(_signalfd_fd, &_signalfd_process_sigset, 0) == -1)
    {
    ec.assign(errno, std::system_category());
    }
  }

void signalfd_signal_handler_impl::remove_sigaction(
    const sigaction_key& key, std::error_code& ec)
  {
  sigdelset(&_signalfd_process_sigset, key.signum_);

  signalfd(_signalfd_fd, &_signalfd_process_sigset, 0);

  pthread_sigmask(SIG_SETMASK, &_signalfd_process_sigset, NULL);
  }

#endif // __linux__

}; // namespace apoa
