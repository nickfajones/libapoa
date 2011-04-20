/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#ifndef LIBAPOA_COMMON_H
#define LIBAPOA_COMMON_H

#include <sys/types.h>

#include <boost/type_traits.hpp>


namespace apoa
{

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
void set_log_level(log_level level);

void log(log_level level, const char* fmt, ...);



//#############################################################################
#define APOA_PRINT(fmt, ...) \
  apoa::log(apoa::log_level_info, fmt, ##__VA_ARGS__);



//#############################################################################
// get current thread id
pid_t get_tid();



/*#############################################################################
 * storage classes for holding function arguments
 */
template <typename A, bool B = (boost::is_integral<A>::value ||
                                boost::is_floating_point<A>::value)>
class value_carrier
  {
  public:
    typedef const A param_type;
    typedef A ref_type;
    
  public:
    value_carrier(typename value_carrier<A, B>::param_type value) :
      m_value(value)
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(y)
      {
      }
    
    ~value_carrier()
      {
      }
    
  public:
    typename value_carrier<A, B>::ref_type get() const
      {
      return m_value;
      }
    
  private:
    A m_value;
  };



/*#############################################################################
 */
template <typename A>
class value_carrier<A, false>
  {
  public:
    typedef const A& param_type;
    typedef A& ref_type;
    
  public:
    value_carrier(typename value_carrier<A, false>::param_type value) :
      m_value(NULL), m_value_ref(const_cast<A*>(&value))
      {
      }
    
    value_carrier(typename value_carrier<A, false>::ref_type value) :
      m_value(NULL), m_value_ref(&value)
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(new A(y)), m_value_ref(m_value)
      {
      }
    
    ~value_carrier()
      {
      if (m_value)
        {
        delete m_value;
        m_value = NULL;
        }
      
      m_value_ref = NULL;
      }
    
  public:
    typename value_carrier<A, false>::ref_type get() const
      {
      return *m_value_ref;
      }
    
  private:
    A* m_value;
    A* m_value_ref;
  };



/*#############################################################################
 */
template <typename A>
class value_carrier<const A, false>
  {
  public:
    typedef const A& param_type;
    typedef const A& ref_type;
    
  public:
    value_carrier(
        typename value_carrier<const A, false>::param_type value) :
      m_value(NULL), m_value_ref(const_cast<A*>(&value))
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(new A(y)), m_value_ref(m_value)
      {
      }
    
    ~value_carrier()
      {
      if (m_value)
        {
        delete m_value;
        m_value = NULL;
        }
      
      m_value_ref = NULL;
      }
    
  public:
    typename value_carrier<const A, false>::ref_type get() const
      {
      return *m_value_ref;
      }
    
  private:
    A* m_value;
    const A* m_value_ref;
  };



/*#############################################################################
 */
template <typename A>
class value_carrier<A&, false>
  {
  public:
    typedef const A& param_type;
    typedef A& ref_type;
    
  public:
    value_carrier(
        typename value_carrier<A&, false>::param_type value) :
      m_value(NULL), m_value_ref(const_cast<A*>(&value))
      {
      }
    
    value_carrier(typename value_carrier<A, false>::ref_type value) :
      m_value(NULL), m_value_ref(&value)
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(new A(y)), m_value_ref(m_value)
      {
      }
    
    ~value_carrier()
      {
      if (m_value)
        {
        delete m_value;
        m_value = NULL;
        }
      
      m_value_ref = NULL;
      }
    
  public:
    typename value_carrier<A&, false>::ref_type get() const
      {
      return *m_value_ref;
      }
    
  private:
    A* m_value;
    A* m_value_ref;
  };



/*#############################################################################
 */
template <typename A>
class value_carrier<const A&, false>
  {
  public:
    typedef const A& param_type;
    typedef const A& ref_type;
    
  public:
    value_carrier(
        typename value_carrier<const A&, false>::param_type value) :
      m_value(NULL), m_value_ref(const_cast<A*>(&value))
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(new A(y)), m_value_ref(m_value)
      {
      }
    
    ~value_carrier()
      {
      if (m_value)
        {
        delete m_value;
        m_value = NULL;
        }
      
      m_value_ref = NULL;
      }
    
  public:
    typename value_carrier<const A&, false>::ref_type get() const
      {
      return *m_value_ref;
      }
    
  private:
    A* m_value;
    const A* m_value_ref;
  };



/*#############################################################################
 */
template <typename A>
class value_carrier<A*, false>
  {
  public:
    typedef A* param_type;
    typedef A* ref_type;
    
  public:
    value_carrier(
        typename value_carrier<A*, false>::param_type value) :
      m_value(NULL), m_value_ref(const_cast<A*>(value))
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(new A(y)), m_value_ref(m_value)
      {
      }
    
    ~value_carrier()
      {
      if (m_value)
        {
        delete m_value;
        m_value = NULL;
        }
      
      m_value_ref = NULL;
      }
    
  public:
    typename value_carrier<A*, false>::ref_type get() const
      {
      return m_value_ref;
      }
    
  private:
    A* m_value;
    A* m_value_ref;
  };



/*#############################################################################
 */
template <typename A>
class value_carrier<const A*, false>
  {
  public:
    typedef const A* param_type;
    typedef const A* ref_type;
    
  public:
    value_carrier(
        typename value_carrier<const A*, false>::param_type value) :
      m_value(NULL), m_value_ref(value)
      {
      }
    
    template <typename Y>
    value_carrier(Y y) :
      m_value(new A(y)), m_value_ref(m_value)
      {
      }
    
    ~value_carrier()
      {
      if (m_value)
        {
        delete m_value;
        m_value = NULL;
        }
      
      m_value_ref = NULL;
      }
    
  public:
    typename value_carrier<const A*, false>::ref_type get() const
      {
      return m_value_ref;
      }
    
  private:
    A* m_value;
    const A* m_value_ref;
  };



/*#############################################################################
 */
template <>
class value_carrier<void, false>
  {
  public:
    typedef const void* param_type;
    typedef const void* ref_type;
    
  public:
    template <typename Y>
    value_carrier(Y y)
      {
      }
    
    ~value_carrier()
      {
      }
    
  public:
    value_carrier<void, false>::ref_type get() const
      {
      return NULL;
      }
  };

}; // namespace apoa

#endif // LIBAPOA_COMMON_H
