/**\file*********************************************************************
 *                                                                     \brief
 *  Class template map [map]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_MAP
#define NTL__STLX_MAP
#pragma once

#include "stdexcept_fwd.hxx"
#include "ext/rbtree.hxx"
#include "range.hxx"

namespace std {

/**\defgroup  lib_containers *********** 23 Containers library [containers]
  *@{    Components to organize collections of information
  **/

/**\defgroup  lib_associative *********** 23.3 Associative containers [associative]
  *@{    Provide an ability for fast retrieval of data based on keys.
  **/

/// [23.3.1 map]
/// supports unique keys (contains at most one of each key value)
  template<class Key,class T,class Compare = less<Key>,class Allocator = allocator<pair<Key, T> > > class map;
  template<class Key,class T,class Compare,class Allocator> class multimap;

  namespace __
  {
    template <class Key,
              class T,
              class Compare,
              class Allocator
              >
    class value_compare:
      public binary_function<pair</*const*/ Key, T>, pair</*const*/ Key, T>, bool>
    {
    public:
      typedef pair</*const*/ Key, T> value_type;

      __forceinline
      value_compare(const value_compare& x)
        :comp(x.comp)
      {}

      #ifdef NTL_CXX_RV
      __forceinline
      value_compare(value_compare&& x)
        :comp(move(x.comp))
      {}

      __forceinline
      value_compare& operator=(value_compare&& c) { comp = move(c.comp); return *this; }
      #endif

      __forceinline
      value_compare& operator=(const value_compare& c) { comp = c.comp; return *this; }

      __forceinline
      bool operator()(const value_type& x, const value_type& y) const
      {
        return comp(x.first, y.first);
      }
      __forceinline
        bool operator()(const value_type& x, const Key& y) const
      {
        return comp(x.first, y);
      }
      __forceinline
        bool operator()(const Key& x, const value_type& y) const
      {
        return comp(x, y.first);
      }

      template<typename U>
      __forceinline
        typename enable_if<__::is_transparent<Compare, U>::value, bool>::type operator()(const value_type& x, const U& y) const
      {
        return comp(x.first, y);
      }
      template<typename U>
      __forceinline
        typename enable_if<__::is_transparent<Compare, U>::value, bool>::type operator()(const U& y, const value_type& x) const
      {
        return comp(y, x.first);
      }

    friend class std::map<Key, T, Compare, Allocator>;
    friend class std::multimap<Key, T, Compare, Allocator>;
    protected:
      Compare comp;
      value_compare(Compare c) : comp(c) {}
      value_compare();
    };
  } // __

template <class Key,
          class T,
          class Compare,
          class Allocator>
class map:
  protected std::ext::tree::rb_tree<pair</*const*/ Key, T>, __::value_compare<Key, T, Compare, Allocator>, Allocator>
{
  ///////////////////////////////////////////////////////////////////////////
  typedef __::value_compare<Key, T, Compare, Allocator>  value_compare;
  typedef std::ext::tree::rb_tree<pair</*const*/ Key, T>, value_compare, Allocator> tree_type;
  typedef typename tree_type::node node;
  public:

    ///\name  types
    typedef Key                                       key_type;
    typedef T                                         mapped_type;
    typedef pair</*const*/ Key, T>                    value_type;
    typedef Compare                                   key_compare;

    typedef Allocator                                 allocator_type;
    typedef typename
      Allocator::template rebind<value_type>::other   allocator;

    typedef typename  allocator::pointer              pointer;
    typedef typename  allocator::const_pointer        const_pointer;
    typedef       value_type&                         reference;
    typedef const value_type&                         const_reference;
    typedef typename  allocator::size_type            size_type;
    typedef typename  allocator::difference_type      difference_type;

    typedef typename tree_type::iterator              iterator;
    typedef typename tree_type::const_iterator        const_iterator;

    typedef typename tree_type::reverse_iterator       reverse_iterator;
    typedef typename tree_type::const_reverse_iterator const_reverse_iterator;

public:
    ///\name 23.3.1.1 construct/copy/destroy:
    explicit map(const Compare& comp = Compare(), const Allocator& a = Allocator())
      :tree_type(val_comp_, a), val_comp_(comp)
    {}

    template <class InputIterator>
    map(InputIterator   first,
        InputIterator   last,
        const Compare&  comp = Compare(),
        const Allocator& a = Allocator())
        :tree_type(val_comp_, a), val_comp_(comp)
    {
      insert(first, last);
    }

    map(const map<Key, T, Compare, Allocator> & x)
      :tree_type(static_cast<const tree_type&>(x)), val_comp_(x.val_comp_)
    {}

#ifdef NTL_CXX_RV
    map(map<Key,T,Compare,Allocator>&& x)
      // Compare must be a CopyConstructible
      :tree_type(val_comp_, x.get_allocator()), val_comp_(x.val_comp_)
    {
      swap(x);
    };
#endif

    map(const Allocator& a)
      :tree_type(val_comp_, a), val_comp_(Compare())
    {}

    map(const map& x, const Allocator& a)
      :tree_type(val_comp_, a), val_comp_(x.val_comp_)
    {}

#ifdef NTL_CXX_RV
    map(map&& x, const Allocator& a)
      :tree_type(val_comp_, a), val_comp_(x.val_comp_)
    {}
#endif

    map(initializer_list<value_type> il, const Compare comp = Compare(), const Allocator& a = Allocator())
      :tree_type(val_comp_, a), val_comp_(comp)
    {
      tree_type::insert_range(il.begin(), il.end());
    }

    ~map(){}

    map<Key, T, Compare, Allocator>& operator=(const map<Key, T, Compare, Allocator> & x)
    {
      if(this != &x){
        val_comp_ = x.val_comp_;
        tree_type::operator=(x);
      }
      return *this;
    }

#ifdef NTL_CXX_RV
    map<Key,T,Compare,Allocator>& operator=(map<Key,T,Compare,Allocator>&& x)
    {
      if(this != &x){
        clear();
        swap(x);
      }
      return *this;
    }

#ifdef NTL_STLX_RANGE
    ///\name Range extension
    template<class Iter>
    explicit map(std::range<Iter>&& R)
    {
      insert(forward<Range>(R));
    }
    template<class Iter>
    map& operator=(std::range<Iter>&& R)
    {
      clear();
      insert(forward<Range>(R));
      return *this;
    }
    template<class Iter>
    void insert(std::range<Iter>&& R)
    {
      insert(__::ranged::adl_begin(R), __::ranged::adl_end(R));
    }
    ///\}
#endif // NTL_STLX_RANGE
#endif

    map& operator=(initializer_list<value_type> il)
    {
      return *this = map(il);
    }

    using tree_type::get_allocator;
    using tree_type::begin;
    using tree_type::rbegin;
    using tree_type::cbegin;
    using tree_type::crbegin;
    using tree_type::end;
    using tree_type::rend;
    using tree_type::cend;
    using tree_type::crend;

    using tree_type::empty;
    using tree_type::size;
    using tree_type::max_size;

    using tree_type::erase;
    using tree_type::clear;
    using tree_type::insert;

    // 23.3.1.2 element access:
    T& operator[](const key_type& x)
    {
      iterator iter = find(x);
      if(iter == end())
        iter = insert(value_type(x, mapped_type())).first;
      return iter->second;
    }

#ifdef NTL_CXX_RV
    T& operator[](key_type&& x)
    {
      iterator iter = find(x);
      if(iter == end())
        iter = insert(value_type(x, mapped_type())).first;
      return iter->second;
    }
#endif

    T& at(const key_type& x) __ntl_throws(out_of_range)
    {
      iterator i = find(x);
      if(i == end())
        __throw_out_of_range("specified key isn't exists in the map");
      return i->second;
    }

    const T& at(const key_type& x) const __ntl_throws(out_of_range)
    {
      const_iterator i = const_cast<map*>(this)->find(x);
      if(i == end())
        __throw_out_of_range("specified key isn't exists in the map");
      return i->second;
    }

    // modifiers:
#ifdef NTL_CXX_VT
    template <class... Args> pair<iterator, bool> emplace(Args&&... args)
    {
      return tree_type::emplace_hint(end(), std::forward<Args>(args)...);
    }
    template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args)
    {
      return tree_type::emplace_hint(position, std::forward<Args>(args)...).first;
    }
#endif

#ifdef NTL_CXX_RV
    //std::pair<iterator, bool> insert(const value_type& x)
    //{
    //  return tree_type::insert_impl(tree_type::construct_node(x));
    //}

    //iterator insert(const_iterator /*position*/, const value_type& x)
    //{
    //  // TODO: implement fast insert function based on position
    //  return tree_type::insert_impl(tree_type::construct_node(x)).first;
    //}

    template<class P>
    std::pair<iterator, bool> insert(P&& x)
    {
      return tree_type::insert_reference(std::forward<P>(x));
    }
    template<class P>
    iterator insert(const_iterator /*position*/, P&& x)
    {
      // TODO: implement fast insert function based on position
      return tree_type::insert_reference(std::forward<P>(x)).first;
    }
#endif

    __forceinline
    void insert(initializer_list<value_type> il)
    {
      tree_type::insert_range(il.begin(), il.end());
    }

    size_type erase(const key_type& x)
    {
      if(empty())
        return 0;
      iterator val = find(x);
      return val == end() ? 0 : (erase(val), 1);
    }

    void swap(map<Key,T,Compare,Allocator>& x)
    {
      if(this != &x){
        tree_type::swap(x);
        using std::swap;
        swap(val_comp_, x.val_comp_);
      }
    }

    // observers:
    key_compare key_comp() const { return val_comp_.comp; }
    value_compare value_comp() const { return val_comp_; }

    // 23.3.1.3 map operations:
    iterator find(const key_type& x)
    {
      node* p = tree_type::root_;
      while ( p )
      {
        if ( val_comp_(x, p->elem) )
          p = p->child[tree_type::left];
        else if(val_comp_(p->elem, x))
          p = p->child[tree_type::right];
        else
          return tree_type::make_iterator(p);
      }
      return end();
    }

    const_iterator find(const key_type& x) const
    {
      return const_cast<map*>(this)->find(x);
    }

#ifdef NTL_CXX_TYPEOF
    template<typename K>
    typename enable_if<__::is_transparent<Compare, K>::value, iterator>::type find(const K& x)
    {
      node* p = tree_type::root_;
      while ( p )
      {
        if ( val_comp_(x, p->elem) )
          p = p->child[tree_type::left];
        else if(val_comp_(p->elem, x))
          p = p->child[tree_type::right];
        else
          return tree_type::make_iterator(p);
      }
      return end();
    }
    template<typename K>
    typename enable_if<__::is_transparent<Compare, K>::value, const_iterator>::type find(const K& x) const
    {
      return const_cast<map*>(this)->find(x);
    }

    template<typename K>
    typename enable_if<__::is_transparent<Compare, K>::value, size_type>::type count(const K& x)
    {
      return find(x) != end() ? 1 : 0;
    }

#endif

    size_type count(const key_type& x) const
    {
      return find(x) != end() ? 1 : 0;
    }

    iterator        lower_bound(const key_type& x)        { return equal_range(x).second; }
    const_iterator  lower_bound(const key_type& x) const  { return equal_range(x).second; }
    iterator        upper_bound(const key_type& x)        { return equal_range(x).first;  }
    const_iterator  upper_bound(const key_type& x) const  { return equal_range(x).first;  }

    pair<iterator,iterator> equal_range(const key_type& x)
    {
      // find a node with value which are equal or nearest to the x
      node* p = tree_type::root_;
      while ( p )
      {
        if ( val_comp_(value_type(x, mapped_type()), p->elem) )
        {
          if(p->child[tree_type::left])
            p = p->child[tree_type::left];
          else
          {
            iterator re(tree_type::make_iterator(p));
            return make_pair(re, re); // is a closest nodes
          }
        }
        else if ( val_comp_(p->elem, value_type(x, mapped_type())) ) // greater
          p = p->child[tree_type::right];
        else
          return make_pair(tree_type::make_iterator(p),
              tree_type::make_iterator(tree_type::next(p, tree_type::right)));
      }
      iterator re(tree_type::make_iterator(nullptr));
      return make_pair(re, re);
    }

    pair<const_iterator,const_iterator> equal_range(const key_type& x) const
    {
      return const_cast<map*>(this)->equal_range(x);
    }

    friend bool operator==(const map<Key,T,Compare,Allocator>& x, const map<Key,T,Compare,Allocator>& y)
    {
      return static_cast<const tree_type&>(x) == static_cast<const tree_type&>(y);
    }

    friend bool operator< (const map<Key,T,Compare,Allocator>& x, const map<Key,T,Compare,Allocator>& y)
    {
      return static_cast<const tree_type&>(x) < static_cast<const tree_type&>(y);
    }
      

  private:
    value_compare val_comp_;

}; //class template map



template <class Key, class T, class Compare, class Allocator>
inline bool operator!=(const map<Key,T,Compare,Allocator>& x, const map<Key,T,Compare,Allocator>& y)
{
  return rel_ops::operator !=(x,y);
}

template <class Key, class T, class Compare, class Allocator>
inline bool operator> (const map<Key,T,Compare,Allocator>& x, const map<Key,T,Compare,Allocator>& y)
{
  return rel_ops::operator >(x,y);
}

template <class Key, class T, class Compare, class Allocator>
inline bool operator>=(const map<Key,T,Compare,Allocator>& x, const map<Key,T,Compare,Allocator>& y)
{
  return rel_ops::operator >=(x,y);
}


template <class Key, class T, class Compare, class Allocator>
inline bool operator<=(const map<Key,T,Compare,Allocator>& x, const map<Key,T,Compare,Allocator>& y)
{
  return rel_ops::operator <=(x,y);
}


template <class Key, class T, class Compare, class Allocator>
inline void swap(map<Key,T,Compare,Allocator>& x, map<Key,T,Compare,Allocator>& y) { x.swap(y); }


/// [23.3.2 multimap]
template <class Key, class T, class Compare = less<Key>, class Allocator = allocator<pair<const Key, T> > >
class multimap:
  protected std::ext::tree::rb_tree<pair</*const*/ Key, T>, __::value_compare<Key, T, Compare, Allocator>, Allocator>
{
  ///////////////////////////////////////////////////////////////////////////
  typedef __::value_compare<Key, T, Compare, Allocator>  value_compare;
  typedef std::ext::tree::rb_tree<pair</*const*/ Key, T>, value_compare, Allocator> tree_type;
  typedef typename tree_type::node node;
public:

  ///\name  types
  typedef Key                                       key_type;
  typedef T                                         mapped_type;
  typedef pair</*const*/ Key, T>                    value_type;
  typedef Compare                                   key_compare;

  typedef Allocator                                 allocator_type;
  typedef typename
    Allocator::template rebind<value_type>::other   allocator;

  typedef typename  allocator::pointer              pointer;
  typedef typename  allocator::const_pointer        const_pointer;
  typedef       value_type&                         reference;
  typedef const value_type&                         const_reference;
  typedef typename  allocator::size_type            size_type;
  typedef typename  allocator::difference_type      difference_type;

  typedef typename tree_type::iterator              iterator;
  typedef typename tree_type::const_iterator        const_iterator;

  typedef typename tree_type::reverse_iterator       reverse_iterator;
  typedef typename tree_type::const_reverse_iterator const_reverse_iterator;

public:
  ///\name 23.3.1.1 construct/copy/destroy:
  explicit multimap(const Compare& comp = Compare(), const Allocator& a = Allocator())
    :tree_type(val_comp_, a), val_comp_(comp)
  {}

  template <class InputIterator>
  multimap(InputIterator   first,
    InputIterator   last,
    const Compare&  comp = Compare(),
    const Allocator& a = Allocator())
    :tree_type(val_comp_, a), val_comp_(comp)
  {
    insert(first, last);
  }

  multimap(const multimap<Key, T, Compare, Allocator> & x)
    :tree_type(static_cast<const tree_type&>(x)), val_comp_(x.val_comp_)
  {}

#ifdef NTL_CXX_RV
  multimap(multimap<Key,T,Compare,Allocator>&& x)
    // Compare must be a CopyConstructible
    :tree_type(val_comp_, x.get_allocator()), val_comp_(x.val_comp_)
  {
    swap(x);
  };
#endif

  multimap(const Allocator& a)
    :tree_type(val_comp_, a), val_comp_(Compare())
  {}

  multimap(const multimap& x, const Allocator& a)
    :tree_type(val_comp_, a), val_comp_(x.val_comp_)
  {}

#ifdef NTL_CXX_RV
  multimap(multimap&& x, const Allocator& a)
    :tree_type(val_comp_, a), val_comp_(x.val_comp_)
  {}
#endif

  multimap(initializer_list<value_type> il, const Compare comp = Compare(), const Allocator& a = Allocator())
    :tree_type(val_comp_, a), val_comp_(comp)
  {
    tree_type::insert_range(il.begin(), il.end());
  }

  ~multimap(){}

  multimap<Key, T, Compare, Allocator>& operator=(const multimap<Key, T, Compare, Allocator> & x)
  {
    if(this != &x){
      val_comp_ = x.val_comp_;
      tree_type::operator=(x);
    }
    return *this;
  }

#ifdef NTL_CXX_RV
  multimap<Key,T,Compare,Allocator>& operator=(multimap<Key,T,Compare,Allocator>&& x)
  {
    if(this != &x){
      clear();
      swap(x);
    }
    return *this;
  }

#ifdef NTL_STLX_RANGE
  ///\name Range extension
  template<class Iter>
  explicit multimap(std::range<Iter>&& R)
  {
    insert(forward<Range>(R));
  }
  template<class Iter>
  multimap& operator=(std::range<Iter>&& R)
  {
    clear();
    insert(forward<Range>(R));
    return *this;
  }
  template<class Iter>
  void insert(std::range<Iter>&& R)
  {
    insert(__::ranged::adl_begin(R), __::ranged::adl_end(R));
  }
  ///\}
#endif // NTL_STLX_RANGE
#endif

  multimap& operator=(initializer_list<value_type> il)
  {
    return *this = multimap(il);
  }

  using tree_type::get_allocator;
  using tree_type::begin;
  using tree_type::rbegin;
  using tree_type::cbegin;
  using tree_type::crbegin;
  using tree_type::end;
  using tree_type::rend;
  using tree_type::cend;
  using tree_type::crend;

  using tree_type::empty;
  using tree_type::size;
  using tree_type::max_size;

  using tree_type::erase;
  using tree_type::clear;
  using tree_type::insert;

  // 23.3.1.2 element access:
  T& operator[](const key_type& x)
  {
    iterator iter = find(x);
    if(iter == end())
      iter = insert(value_type(x, mapped_type())).first;
    return iter->second;
  }

#ifdef NTL_CXX_RV
  T& operator[](key_type&& x)
  {
    iterator iter = find(x);
    if(iter == end())
      iter = insert(value_type(x, mapped_type())).first;
    return iter->second;
  }
#endif

  T& at(const key_type& x) __ntl_throws(out_of_range)
  {
    iterator i = find(x);
    if(i == end())
      __throw_out_of_range("specified key isn't exists in the multimap");
    return i->second;
  }

  const T& at(const key_type& x) const __ntl_throws(out_of_range)
  {
    const_iterator i = const_cast<multimap*>(this)->find(x);
    if(i == end())
      __throw_out_of_range("specified key isn't exists in the multimap");
    return i->second;
  }

  // modifiers:
#ifdef NTL_CXX_VT
  template <class... Args> pair<iterator, bool> emplace(Args&&... args);
  template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
#endif

#ifdef NTL_CXX_RV
  //std::pair<iterator, bool> insert(const value_type& x)
  //{
  //  return tree_type::insert_impl(tree_type::construct_node(x));
  //}

  //iterator insert(const_iterator /*position*/, const value_type& x)
  //{
  //  // TODO: implement fast insert function based on position
  //  return tree_type::insert_impl(tree_type::construct_node(x)).first;
  //}

  template<class P>
  std::pair<iterator, bool> insert(P&& x)
  {
    return tree_type::insert_reference(std::forward<P>(x));
  }
  template<class P>
  iterator insert(const_iterator /*position*/, P&& x)
  {
    // TODO: implement fast insert function based on position
    return tree_type::insert_reference(std::forward<P>(x)).first;
  }
#endif

  __forceinline
    void insert(initializer_list<value_type> il)
  {
    tree_type::insert_range(il.begin(), il.end());
  }

  size_type erase(const key_type& x)
  {
    if(empty())
      return 0;
    iterator val = find(x);
    return val == end() ? 0 : (erase(val), 1);
  }

  void swap(multimap<Key,T,Compare,Allocator>& x)
  {
    if(this != &x){
      tree_type::swap(x);
      using std::swap;
      swap(val_comp_, x.val_comp_);
    }
  }

  // observers:
  key_compare key_comp() const { return val_comp_.comp; }
  value_compare value_comp() const { return val_comp_; }

  // 23.3.1.3 multimap operations:
  iterator find(const key_type& x)
  {
    node* p = tree_type::root_;
    while ( p )
    {
      if ( val_comp_.comp(x, p->elem.first) )
        p = p->child[tree_type::left];
      else if(val_comp_.comp(p->elem.first, x))
        p = p->child[tree_type::right];
      else
        return tree_type::make_iterator(p);
    }
    return end();
  }

  const_iterator find(const key_type& x) const
  {
    return const_cast<multimap*>(this)->find(x);
  }

  size_type count(const key_type& x) const
  {
    return find(x) != end() ? 1 : 0;
  }

  iterator        lower_bound(const key_type& x)        { return equal_range(x).second; }
  const_iterator  lower_bound(const key_type& x) const  { return equal_range(x).second; }
  iterator        upper_bound(const key_type& x)        { return equal_range(x).first;  }
  const_iterator  upper_bound(const key_type& x) const  { return equal_range(x).first;  }

  pair<iterator,iterator> equal_range(const key_type& x)
  {
    // find a node with value which are equal or nearest to the x
    node* p = tree_type::root_;
    while ( p )
    {
      if ( val_comp_(value_type(x, mapped_type()), p->elem) )
      {
        if(p->child[tree_type::left])
          p = p->child[tree_type::left];
        else
        {
          iterator re(tree_type::make_iterator(p));
          return make_pair(re, re); // is a closest nodes
        }
      }
      else if ( val_comp_(p->elem, value_type(x, mapped_type())) ) // greater
        p = p->child[tree_type::right];
      else
        return make_pair(tree_type::make_iterator(p),
        tree_type::make_iterator(tree_type::next(p, tree_type::right)));
    }
    iterator re(tree_type::make_iterator(nullptr));
    return make_pair(re, re);
  }

  pair<const_iterator,const_iterator> equal_range(const key_type& x) const
  {
    return const_cast<multimap*>(this)->equal_range(x);
  }

  friend bool operator==(const multimap<Key,T,Compare,Allocator>& x, const multimap<Key,T,Compare,Allocator>& y)
  {
    return static_cast<const tree_type&>(x) == static_cast<const tree_type&>(y);
  }

  friend bool operator< (const multimap<Key,T,Compare,Allocator>& x, const multimap<Key,T,Compare,Allocator>& y)
  {
    return static_cast<const tree_type&>(x) < static_cast<const tree_type&>(y);
  }


private:
  value_compare val_comp_;

}; //class template multimap


#if 0
template <class Key, class T, class Compare, class Allocator>
bool operator==(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator< (const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator!=(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator> (const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator>=(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator<=(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
void swap(multimap<Key,T,Compare,Allocator>& x,
          multimap<Key,T,Compare,Allocator>& y);
#endif
///@}
/**@} lib_associative */
/**@} lib_containers */

}//namespace std

#endif//#ifndef NTL__STLX_MAP
