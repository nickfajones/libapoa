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

#ifndef LIBAPOA_MEMORY_POOL_TYPES_HPP
#define LIBAPOA_MEMORY_POOL_TYPES_HPP

#include <set>

#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/detail/singleton.hpp>

#include <libapoa/common.hpp>


namespace apoa
{

namespace priv
{

//#############################################################################
template <
  typename Userallocator_type,
  size_t RequestedSize,
  size_t NextSize,
  size_t MaxSize>
class per_thread_pool
  {
  public:
    typedef Userallocator_type user_allocator;

    typedef typename boost::pool<Userallocator_type>::size_type size_type;
    typedef typename boost::pool<Userallocator_type>::difference_type difference_type;

  public:
    static const size_t requested_size =                  RequestedSize;
    static const size_t next_size =                       NextSize;

  private:
    struct pool_container
      {
      boost::pool<Userallocator_type> pool_;

      pool_container() : pool_(requested_size, next_size) {}
      };

  private:
    static apoa::per_thread_index<pool_container>& get_index()
      {
      static apoa::per_thread_index<pool_container> index;

      return index;
      }

  public:
    per_thread_pool()
      {
      }

    ~per_thread_pool()
      {
      }

  public:
    static void* malloc()
      {
      return get_index().get().pool_.malloc();
      }

    static void* ordered_malloc()
      {
      return get_index().get().pool_.ordered_malloc();
      }

    static void* ordered_malloc(const size_type size)
      {
      return get_index().get().pool_.ordered_malloc(size);
      }

  public:
    static bool is_from(void* const ptr)
      {
      return get_index().get().pool_.is_from(ptr);
      }

  public:
    static void free(void* const ptr)
      {
      get_index().get().pool_.free(ptr);
      }

    static void ordered_free(void* const ptr)
      {
      get_index().get().pool_.ordered_free(ptr);
      }

    static void free(void* const ptr, const size_type size)
      {
      get_index().get().pool_.free(ptr, size);
      }

    static void ordered_free(void* const ptr, const size_type size)
      {
      get_index().get().pool_.ordered_free(ptr, size);
      }

  public:
    static bool release_memory()
      {
      return get_index().get().pool_.release_memory();
      }

    static bool purge_memory()
      {
      return get_index().get().pool_.purge_memory();
      }
  };

}; // namespace priv



//#############################################################################
template <
  typename T,
  typename Userallocator_type = boost::default_user_allocator_new_delete,
  unsigned NextSize = 32,
  unsigned MaxSize = 0>
class per_thread_allocator
  {
  public:
    typedef T value_type;
    typedef Userallocator_type user_allocator;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef typename boost::pool<Userallocator_type>::size_type size_type;
    typedef typename boost::pool<Userallocator_type>::difference_type difference_type;

  public:
    typedef
      priv::per_thread_pool<Userallocator_type, sizeof(T), NextSize, MaxSize>
        pool_type;

  public:
    static const size_t next_size =                       NextSize;

  public:
    template <typename U>
    struct rebind
      {
      typedef per_thread_allocator<U> other;
      };

  public:
    per_thread_allocator()
      {
      }

    template <typename U>
    per_thread_allocator(
        const per_thread_allocator<U, Userallocator_type, NextSize, MaxSize>& rvalue)
      {
      }

  public:
    template <typename U>
    per_thread_allocator& operator=(
        const per_thread_allocator<U, Userallocator_type, NextSize, MaxSize>& rvalue)
      {
      return *this;
      }

  public:
    bool operator==(const per_thread_allocator& rvalue) const
      {
      return true;
      }

    bool operator!=(const per_thread_allocator& rvalue) const
      {
      return false;
      }

  public:
    static pointer address(reference x)
      {
      return &x;
      }

    static const_pointer address(const_reference x)
      {
      return &x;
      }

  public:
    static size_type max_size()
      {
      return (std::numeric_limits<size_type>::max)();
      }

  public:
    static void construct(pointer ptr, const T& val)
      {
      new (ptr) T(val);
      }

    static void destroy(pointer ptr)
      {
      ptr->~T();
      (void)ptr;
      }

  public:
    static pointer allocate(size_type size)
      {
      void* ptr = pool_type::ordered_malloc(size);

      APOA_PRINT("allocated: '%p' ('%ld x %ld'))", ptr, size, sizeof(T));

      return static_cast<pointer>(ptr);
      }

    static pointer allocate(size_type size, const void* const)
      {
      return allocate(size);
      }

    void deallocate(pointer ptr, size_type size)
      {
      APOA_PRINT("deallocating: '%p' ('%ld x %ld'))", ptr, size, sizeof(T));

      pool_type::ordered_free(ptr, size);
      }
  };



//#############################################################################
namespace priv
{
apoa::per_thread_allocator<std::string::traits_type::value_type> string_allocator;

template <typename Allocator>
struct string_allocator_rebinder
  {
  typedef std::basic_string<
    typename std::string::traits_type::value_type,
    typename std::string::traits_type,
    Allocator>
      other;
  }
};

class string :
    public
      typename priv::string_allocator_rebinder<
       >::other
  {
  public:
    explicit string(const allocator_type& alloc = allocator_type()) :
      std::string(alloc)
      {
      }

    string(
        size_type count,
        typename std::string::traits_type::value_type ch,
        const allocator_type& alloc = allocator_type()) :
      std::string(count, ch, alloc)
      {
      }

    string(
        const asio::string& rvalue,
        size_type pos,
        size_type count = std::basic_string::npos,
        const allocator_type& alloc = allocator_type()) :
      std::string(alloc, pos, count, alloc)
      {
      }

    template <typename Allocator>
    string(
        const typename priv::string_allocator_rebinder<Allocator>& rvalue,
        size_type pos,
        size_type count = std::basic_string::npos,
        const allocator_type& alloc = allocator_type()) :
      std::string(alloc)
      {
      if (pos > rvalue.size())
        {
        throw std::out_of_range;
        }

      if ((pos + count) > rvalue.size())
        {
        count = rvalue.size() - pos;
        }

      this->assign(rvalue.c_str() + pos, count);
      }

    string(
        const typename std::string::traits_type::value_type* s,
        size_type count,
        const allocator_type& alloc = allocator_type()) :
      std::string(s, count, alloc)
      {
      }

    string(
        const typename std::string::traits_type::value_type* s,
        const allocator_type& alloc = allocator_type()) :
      std::string(s, alloc)
      {
      }

    template<typename InputIterator>
    string(
        InputIterator first, InputIterator last,
        const allocator_type& alloc = allocator_type()) :
      std::string(first, last, alloc)
      {
      }

    string(const apoa::string& rvalue) :
      std::string(rvalue.c_str(), allocator_type())
      {
      }

    template <typename Allocator>
    string(
        const std::string<
          typename std::string::traits_type::value_type,
          typename std::string::traits_type,
          Allocator>& rvalue) :
      std::string(rvalue.c_str(), allocator_type())
      {
      }

  public:
    template <typename Allocator>
    string& operator=(
        const std::string<
          typename std::string::traits_type::value_type,
          typename std::string::traits_type,
          Allocator>& rvalue)
      {
      this->assign(rvalue.c_str());

      return *this;
      }

  public:
    template <typename Allocator>
    bool operator==(
        const std::string<
          typename std::string::traits_type::value_type,
          typename std::string::traits_type,
          Allocator>& rvalue) const
      {
      iterator litr = this->begin();
      typename std::string<
        typename std::string::traits_type::value_type,
        typename std::string::traits_type,
        Allocator>::iterator ritr = rvalue.begin();

      for (;litr != this->end() && ritr != rvalue.end(); ++litr, ++ritr)
        {
        if (*litr != *ritr)
          {
          return false;
          }
        }

      return (litr == this->end();
      }
  };

template <typename Allocator1, typename Allocator2>
std::string<
  typename std::string::traits_type::value_type,
  typename std::string::traits_type,
  Allocator1>& operator+()

template <typename T>
struct list
  {
  typedef
    std::list<T, apoa::per_thread_allocator<T> >
      type;
  };

template <typename T>
struct vector
  {
  typedef
    std::vector<T, apoa::per_thread_allocator<T> >
      type;
  };

template <typename Key, typename Compare = std::less<Key> >
struct set
  {
  typedef
    std::set<Key, Compare, apoa::per_thread_allocator<Key> >
      type;
  };

template <typename Key, typename T, typename Compare = std::less<Key> >
struct map
  {
  typedef
    std::map<Key, T, Compare, apoa::per_thread_allocator<std::pair<Key, T> > >
      type;
  };

}; // namesapce apoa

#endif // LIBAPOA_MEMORY_POOL_TYPES_HPP
