/*############################################################################
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


#include <libapoa/basic_thread_pool_impl.hpp>


namespace apoa
{

//#############################################################################
thread_pool_ref::thread_pool_ref(boost::asio::io_service& io_service) :
  m_io_service(io_service)
  {
  }

//#############################################################################
thread_pool_ref::~thread_pool_ref()
  {
  apoa::shutdown_thread();
  }

//#############################################################################
void thread_pool_ref::on_thread_start(
  const boost::system::error_code& ec,
  boost::shared_ptr<basic_thread_pool_impl> impl)
  {
  if (ec)
    {
    BOOST_ASSERT(0 && ec.message().data());
    }

  impl->register_thread(shared_from_this());
  }

//#############################################################################
boost::asio::io_service& thread_pool_ref::get_io_service()
  {
  return m_io_service;
  }



//#############################################################################
basic_thread_pool_impl::basic_thread_pool_impl(
    boost::asio::io_service& io_service) :
  thread_handler_(io_service),
  pool_(),
  iter_(pool_.end())
  {
  }

basic_thread_pool_impl::~basic_thread_pool_impl()
  {
  }

//#############################################################################
void basic_thread_pool_impl::create_pool(uint32_t size)
  {
  if (size == 0)
    {
    return;
    }

  boost::unique_lock<boost::mutex> pool_lock(pool_mutex_);
  if (pool_.size() > 0)
    {
    BOOST_ASSERT(0 && "basic_thread_pool_impl: pool already created");
    }
  pool_lock.unlock();

  for (uint32_t i = 0; i < size; i++)
    {
    thread_handler_.create_thread(
      &thread_pool_ref::on_thread_start,
      static_cast<thread_pool_ref*>(NULL),
      shared_from_this());
    }
  }

//#############################################################################
void basic_thread_pool_impl::destroy_pool()
  {
  boost::lock_guard<boost::mutex> pool_lock(pool_mutex_);

  if (pool_.size() == 0)
    {
    return;
    }

  pool_.clear();
  }

//#############################################################################
boost::asio::io_service& basic_thread_pool_impl::get_thread_service()
  {
  boost::lock_guard<boost::mutex> pool_lock(pool_mutex_);

  if (pool_.size() == 0)
    {
    BOOST_ASSERT(0 && "basic_thread_pool_impl: pool not created");
    }

  if (iter_ == pool_.end())
    {
    iter_ = pool_.begin();
    }
  else
    {
    iter_++;

    if (iter_ == pool_.end())
      {
      iter_ = pool_.begin();
      }
    }

  return iter_->second->get_io_service();
  }

//#############################################################################
void basic_thread_pool_impl::register_thread(
    boost::shared_ptr<thread_pool_ref> ref)
  {
  boost::lock_guard<boost::mutex> pool_lock(pool_mutex_);
  pid_t tid = apoa::get_tid();

  BOOST_ASSERT(pool_.find(tid) == pool_.end() &&
    "basic_thread_pool_impl: create existing thread id");

  pool_[tid] = ref;
  }
}; // namespace apoa
