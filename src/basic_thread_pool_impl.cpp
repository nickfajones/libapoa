/*############################################################################
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

#include <system_error>
#include <functional>

#include <libapoa/basic_thread_pool_impl.hpp>


namespace apoa
{

//#############################################################################
basic_thread_pool_impl::basic_thread_pool_impl(
    asio::io_context& io_context) :
  thread_handler_(io_context),
  pool_(),
  iter_(pool_.end()),
  pool_size_(0)
  {
  }

basic_thread_pool_impl::~basic_thread_pool_impl()
  {
  }

//#############################################################################
void basic_thread_pool_impl::create_pool(
    uint32_t size, std::error_code& ec)
  {
  if (size == 0)
    {
    return;
    }

  std::lock_guard<std::mutex> pool_lock(pool_mutex_);
  if (pool_.size() > 0 || pool_size_ != 0)
    {
    ec.assign(EALREADY, std::system_category());
    return;
    }

  for (uint32_t i = 0; i < size; i++)
    {
    thread_handler_.create_thread(
      std::bind(
        &basic_thread_pool_impl::on_thread_created, shared_from_this(),
          std::placeholders::_1, std::placeholders::_2));
    }
  }

//#############################################################################
void basic_thread_pool_impl::on_thread_created(
    const std::error_code& ec,
    asio::io_context& io_context)
  {
  if (ec)
    {
    BOOST_ASSERT(0 && ec.message().data());
    return;
    }

  pool_size_++;

  pid_t tid = apoa::get_tid();

  std::lock_guard<std::mutex> pool_lock(pool_mutex_);

  BOOST_ASSERT(pool_.find(tid) == pool_.end() &&
    "basic_thread_pool_impl: create existing thread id");

  pool_.insert(std::make_pair(tid, boost::ref(io_context)));
  }

//#############################################################################
void basic_thread_pool_impl::destroy_pool()
  {
  std::lock_guard<std::mutex> pool_lock(pool_mutex_);

  pool_size_ = 0;
  if (pool_.size() == 0)
    {
    return;
    }

  pool_.clear();
  }

//#############################################################################
asio::io_context& basic_thread_pool_impl::get_thread_service(
    std::error_code& ec)
  {
  std::lock_guard<std::mutex> pool_lock(pool_mutex_);

  if (pool_.size() == 0)
    {
    if (pool_size_ != 0)
      {
      ec.assign(EAGAIN, std::system_category());
      }
    else
      {
      ec.assign(ENODATA, std::system_category());
      }

    return thread_handler_.get_io_context();
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

  return iter_->second;
  }
}; // namespace apoa
