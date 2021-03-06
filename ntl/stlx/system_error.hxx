/**\file*********************************************************************
 *                                                                     \brief
 *  19.5 System error support [syserr]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SYSTEM_ERROR
#define NTL__STLX_SYSTEM_ERROR
#pragma once

#ifndef NTL__STLX_TYPE_TRAITS
#include "type_traits.hxx"
#endif
#ifndef NTL__STLX_STDEXCEPT
#include "stdexcept.hxx"
#endif
#ifndef NTL__STLX_IOSFWD
#include "iosfwd.hxx"
#endif
#ifndef NTL__STLX_CSTDIO
#include "cstdio.hxx"
#endif
#ifndef NTL__STLX_CASSERT
#include "cassert.hxx"
#endif
#include "stdstring.hxx"

#include "../nt/strerror.hxx"
//namespace ntl { namespace nt {
//  inline std::string strerror(ntstatus Status);
//  inline std::wstring wstrerror(ntstatus Status);
//}}

namespace std 
{
/**\addtogroup lib_diagnostics 19 Diagnostics library [diagnostics]
 *@{*/

  /**
   *	@addtogroup syserr 19.5 System error support [syserr]
   *  This subclause describes components that the standard library and C++ programs may use to report error
   *  conditions originating from the operating system or other low-level application program interfaces.
   *  @{
   **/

  class error_category;
  class error_code;
  class error_condition;
  class system_error;

  inline const error_category& generic_category();  // POSIX
  inline const error_category& system_category();   // 

  error_code& throws();
  error_code& throw_system_error(const error_code& actual, error_code& holder);
  void        throw_system_error(error_code& ec);

  template<class T>
  struct is_error_code_enum: public false_type {};

  template<class T>
  struct is_error_condition_enum: public false_type {};


  /**
   *	@brief 19.5.1 Class error_category [syserr.errcat]
   *
   *  The class error_category serves as a base class for types used to identify the source and encoding of a
   *  particular category of error code.
   **/
  class error_category
  {
  protected:
    error_category()
    {}
  public:
    virtual ~error_category()
    {}
    
    /** Returns a string naming the error category. */
    virtual const char* name() const __ntl_nothrow = 0;

    /** Returns a string that describes the error condition denoted by \c ev */
    virtual string message(int ev) const = 0;

    /**
     *  @brief Returns an object of type error_condition that corresponds to ev.
     *	@note  If the argument \c ev corresponds to a POSIX \c errno value \c posv, 
     *  the function shall return <tt> error_condition(posv, generic_category) </tt>.
     *  Otherwise, the function shall return <tt> error_condition(ev, system_category) </tt>.
     **/
    virtual error_condition default_error_condition(int ev) const __ntl_nothrow;

    virtual bool equivalent(int code, const error_condition& condition) const __ntl_nothrow;
    
    virtual bool equivalent(const error_code& code, int condition) const __ntl_nothrow;

    bool operator==(const error_category& rhs) const
    {
      return this == &rhs;
    }
    bool operator!=(const error_category& rhs) const
    {
      return !(this == &rhs);
    }
    bool operator<(const error_category& rhs) const __ntl_nothrow
    {
      return less<const error_category*>()(this, &rhs);
    }
  private:
    error_category(const error_category&) __deleted;
    error_category& operator=(const error_category&) __deleted;
  };

  /**
   *	@brief 19.5.2 Class error_code [syserr.errcode]
   *
   *  The class error_code describes an object used to hold error code values, such as those originating from the
   *  operating system or other low-level application program interfaces.
   *  @note Class error_code is an adjunct to error reporting by exception.
   **/
  class error_code
  {
  public:
    // 19.5.2.2 constructors:
    error_code() __ntl_nothrow
      :v(0), c(&system_category())
    {}
    error_code(int val, const error_category& cat) __ntl_nothrow
      :v(val), c(&cat)
    {}

    template <class ErrorCodeEnum>
    error_code(ErrorCodeEnum e, typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type * = 0) __ntl_nothrow
    {
      *this = make_error_code(e);
    }

    // 19.5.2.3 modifers:
    void assign(int val, const error_category& cat) __ntl_nothrow
    {
      v = val, c = &cat;
    }

    template <class ErrorCodeEnum>
    typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type& operator=(ErrorCodeEnum e) __ntl_nothrow
    {
      return *this = make_error_code(e);
    }

    void clear()
    {
      if(this)
        v = 0, c = &system_category();
    }

    // 19.5.2.4 observers:
    int value() const  __ntl_nothrow { return v; }

    const error_category& category() const  __ntl_nothrow { return *c; }

    error_condition default_error_condition() const  __ntl_nothrow;

    string message() const 
    {
      if(!c)
        _assert_msg("C++ static objects wasn't initialized!");
      return c->message(v);
    }

    __explicit_operator_bool() const __ntl_nothrow { return __explicit_bool(v != 0); }
private:
    int v;
    const error_category* c;
  };


  /**
   *	@brief 19.5.3 Class error_condition [syserr.errcondition]
   *
   *  The class error_condition describes an object used to hold values identifying error conditions.
   *  @note error_condition values are portable abstractions, while error_code values (19.5.2) are implementation specific.
   **/
  class error_condition
  {
  public:
    // 19.5.3.2 constructors:
    error_condition() __ntl_nothrow
      :v(0), c(&generic_category())
    {}
    
    error_condition(int val, const error_category& cat) __ntl_nothrow
      :v(val), c(&cat)
    {}

    template <class ErrorConditionEnum>
    error_condition(ErrorConditionEnum e, typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value>::type* = 0) __ntl_nothrow
    {
      *this = make_error_condition(e);
    }

    // 19.5.3.3 modifers:
    void assign(int val, const error_category& cat) __ntl_nothrow
    {
      v = val, c = &cat;
    }

    template<typename ErrorConditionEnum>
    typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value, error_condition>::type& operator= (ErrorConditionEnum e) __ntl_nothrow
    {
      return *this = make_error_code(e);
    }

    void clear() __ntl_nothrow
    {
      v = 0, c = &generic_category();
    }

    // 19.5.3.4 observers:
    int value() const __ntl_nothrow { return v; }

    const error_category& category() const __ntl_nothrow { return *c; }

    string message() const  __ntl_nothrow { return c->message(v); }

    /*explicit */operator bool() const __ntl_nothrow { return v != 0; }
  private:
    int v;
    const error_category* c;
  };

  /**
   *	@brief 19.5.5 Class system_error [syserr.syserr]
   *
   *  The class system_error describes an exception object used to report error conditions that have an associated error code. 
   *  Such error conditions typically originate from the operating system or other low-level application program interfaces.
   **/
  class system_error:
    public runtime_error
  {
  protected:
    error_code ec;
    mutable string msg;
  public:
    system_error(error_code ec, const string& what_arg)
      :runtime_error(what_arg), ec(ec)
    {}
    system_error(error_code ec, const char* what_arg)
      :runtime_error(what_arg), ec(ec)
    {}
    system_error(error_code ec)
      :runtime_error(""), ec(ec)
    {}
    system_error(int ev, const error_category& ecat, const string& what_arg)
      :runtime_error(what_arg), ec(ev, ecat)
    {}
    system_error(int ev, const error_category& ecat, const char* what_arg)
      :runtime_error(what_arg), ec(ev, ecat)
    {}
    system_error(int ev, const error_category& ecat)
      :runtime_error(""), ec(ev, ecat)
    {}

    const error_code& code() const __ntl_nothrow { return ec; }

    /** Returns a NTBS incorporating messages of the runtime_error and error_code. */
    const char* what() const __ntl_nothrow
    {
      if (msg.empty()) {
        string tmp = runtime_error::what();
        __ntl_try
        {
          if (ec) {
            bool nonempty = !tmp.empty() && tmp != "";
            if(nonempty)
              tmp += ": '";
            tmp += ec.message();
            if(nonempty)
              tmp += "'";
          }
          msg = move(tmp);
        }
        #pragma warning(push)
        #pragma warning(disable:4127)
        __ntl_catch(...) {
          return runtime_error::what(); //-V612
        }
        #pragma warning(pop)
      }
      return msg.c_str();
    }
  };


  //////////////////////////////////////////////////////////////////////////
  // 19.5.2.5 non-member functions:
  inline error_condition error_category::default_error_condition(int ev) const __ntl_nothrow
  {
    return error_condition(ev, *this);
  }

  inline error_condition error_code::default_error_condition() const  __ntl_nothrow
  {
    return c->default_error_condition(v); 
  }

  inline bool error_category::equivalent(int code, const error_condition& condition) const __ntl_nothrow
  {
    return default_error_condition(code) == condition;
  }

  inline bool error_category::equivalent(const error_code& code, int condition) const __ntl_nothrow
  {
    return *this == code.category() && code.value() == condition;
  }

  inline bool operator<(const error_code& lhs, const error_code& rhs) __ntl_nothrow
  {
    return lhs.category() < rhs.category() || lhs.category() == rhs.category() && lhs.value() < rhs.value();
  }

  template <class charT, class traits>
  inline basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, const error_code& ec)
  {
    return os << ec.category().name() << ':' << ec.value();
  }
  
  // 19.5.3.5 non-member functions:
  inline bool operator<(const error_condition& lhs, const error_condition& rhs) __ntl_nothrow
  {
    return lhs.category() < rhs.category() || lhs.category() == rhs.category() && lhs.value() < rhs.value();
  }

  // 19.5.4 Comparison operators [syserr.compare]
  inline bool operator==(const error_code& lhs, const error_code& rhs) __ntl_nothrow
  {
    return &lhs == &rhs || (&lhs && &rhs && lhs.category() == rhs.category() && lhs.value() == rhs.value());
  }

  inline bool operator==(const error_code& lhs, const error_condition& rhs) __ntl_nothrow
  {
    return &lhs && (lhs.category().equivalent(lhs.value(), rhs) || rhs.category().equivalent(lhs, rhs.value()));
  }

  inline bool operator==(const error_condition& lhs, const error_code& rhs) __ntl_nothrow
  {
    return &rhs && (rhs.category().equivalent(rhs.value(), lhs) || lhs.category().equivalent(rhs, lhs.value()));
  }

  inline bool operator==(const error_condition& lhs, const error_condition& rhs) __ntl_nothrow
  {
    return lhs.category() == rhs.category() && lhs.value() == rhs.value();
  }

  inline bool operator!=(const error_code& lhs, const error_code& rhs) __ntl_nothrow
  {
    return !(lhs == rhs);
  }

  inline bool operator!=(const error_code& lhs, const error_condition& rhs) __ntl_nothrow
  {
    return !(lhs == rhs);
  }

  inline bool operator!=(const error_condition& lhs, const error_code& rhs) __ntl_nothrow
  {
    return !(lhs == rhs);
  }

  inline bool operator!=(const error_condition& lhs, const error_condition& rhs) __ntl_nothrow
  {
    return !(lhs == rhs);
  }


  //////////////////////////////////////////////////////////////////////////
  error_code& throws();
  namespace __ 
  {
    /// 19.5.1.5 Error category objects [syserr.errcat.objects]
    class generic_error_category:
      public error_category
    {
    public:
      /** Returns a string naming the error category ("generic") */
      const char *name() const __ntl_nothrow { return "generic"; }

      virtual string message(int ev) const
      {
        char buf[40];
        std::snprintf(buf, _countof(buf), "generic error code #%d", ev);
        return string(buf);
      }

      error_condition default_error_condition(int ev) const __ntl_nothrow
      {
        return error_condition(ev, *this);
      }
      bool equivalent(int code, const error_condition& condition) const __ntl_nothrow
      {
        return default_error_condition(code) == condition;
      }
      bool equivalent(const error_code& code, int condition) const __ntl_nothrow
      {
        return *this == code.category() && code.value() == condition;
      }
    };

    /// 19.5.1.5 Error category objects [syserr.errcat.objects]
    class system_error_category:
      public error_category
    {
    public:
      /** Returns a string naming the error category ("system") */
      const char *name() const __ntl_nothrow { return "system"; }

      virtual string message(int ev) const
      {
        return ntl::nt::strerror(static_cast<ntl::nt::ntstatus>(ev));
      }

      /**
       *	If the argument \c ev corresponds to a POSIX \c errno value \c posv, 
       *  the function shall return <tt> error_condition(posv, generic_category) </tt>.
       *  Otherwise, the function shall return <tt> error_condition(ev, system_category) </tt>.
       **/
      error_condition default_error_condition(int ev) const __ntl_nothrow
      {
        // TODO: detect posix errno
        return error_condition(ev, system_category());
      }

      bool equivalent(int code, const error_condition& condition) const __ntl_nothrow
      {
        return default_error_condition(code) == condition;
      }
      bool equivalent(const error_code& code, int condition) const __ntl_nothrow
      {
        return *this == code.category() && code.value() == condition;
      }
    };

    class iostream_error_category:
      public error_category
    {
    public:
      /** Returns a string naming the error category ("iostream") */
      const char *name() const __ntl_nothrow { return "iostream"; }

      inline virtual string message(int ev) const;

      /**
      *	If the argument \c ev corresponds to a POSIX \c errno value \c posv, 
      *  the function shall return <tt> error_condition(posv, generic_category) </tt>.
      *  Otherwise, the function shall return <tt> error_condition(ev, system_category) </tt>.
      **/
      error_condition default_error_condition(int ev) const __ntl_nothrow
      {
        return error_condition(ev, *this);
      }
      bool equivalent(int code, const error_condition& condition) const __ntl_nothrow
      {
        return default_error_condition(code) == condition;
      }
      bool equivalent(const error_code& code, int condition) const __ntl_nothrow
      {
        return *this == code.category() && code.value() == condition;
      }
    };
  }
  //////////////////////////////////////////////////////////////////////////
  /// 19.5.1.5 Error category objects [syserr.errcat.objects]
  /** Returns a reference to an object of a type derived from class error_category. */
  inline const error_category& generic_category()
  {
    //static const __::generic_error_category instance;
    return *__::static_storage<__::generic_error_category>::get_object();
  }

  /** Returns a reference to an object of a type derived from class error_category. */
  inline const error_category& system_category()
  {
    return *__::static_storage<__::system_error_category>::get_object();
  }

  /** Returns a reference to an object of a type derived from class error_category. */
  inline const error_category& iostream_category()
  {
    return *__::static_storage<__::iostream_error_category>::get_object();
  }

  //////////////////////////////////////////////////////////////////////////
  /// N2838 (http://open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2838.html)
  ///

  /** Returns a null reference that can be used as a default function argument to signify an exception should be thrown when an error is detected ([semantics.throws]). 
    Dereferencing the returned reference results in undefined behavior. */
  __forceinline error_code& throws()
  {
    static error_code* tec = 0;
    return *tec;
  }

  /** Throws system_error with specified error code if provided. */
  __forceinline void throw_system_error(error_code& ec)
  {
    assert(&ec != &throws());
    if(ec)
      __ntl_throw(system_error(ec));
  }
  __forceinline error_code& throw_system_error(const error_code& error, error_code& holder)
  {
    const bool throwable = &holder == &throws();
    if(error){
      if(throwable)
        __ntl_throw(system_error(error));
    }
    if(!throwable)
      return holder = error;
    static error_code success;
    return success;
  }

  //////////////////////////////////////////////////////////////////////////
  ///hash specializations for error_code
  template<> struct hash<error_code>:
   unary_function<error_code, size_t>
   {
     /*** 
      * @brief error_code hash calculation
      *
      * Hash calculation maked as \code hash_combine( hash(ec.value()), hash(ec.category()) ) \endcode 
      **/
     size_t operator()(const argument_type& ec) const __ntl_nothrow
     {
       typedef __::FNVHash h;
       return h::hash_combine(ec.value(), reinterpret_cast<size_t>(&ec.category()));
     }
   };

  /**@} syserr */
  /**@} lib_diagnostics */
} // std

#include "posix_error.hxx"

#endif // NTL__STLX_SYSTEM_ERROR
