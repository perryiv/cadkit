
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A vector class that has error checking.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONTAINERS_s_CLASSES_H_
#define _USUL_CONTAINERS_s_CLASSES_H_


namespace Usul {
namespace Containers {


template < class Sequence_, class ErrorChecker_ > class Vector
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ErrorChecker_ ErrorChecker;
  typedef Sequence_ Sequence;
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;
  typedef typename Sequence::difference_type difference_type;
  typedef typename Sequence::iterator iterator;
  typedef typename Sequence::const_iterator const_iterator;
  typedef typename Sequence::reference reference;
  typedef typename Sequence::const_reference const_reference;
  typedef Vector<Sequence,ErrorChecker> this_type;



  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor/destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector() : _v()
  {
  }
  Vector ( const this_type &v ) : _v ( v._v )
  {
  }
  Vector ( const Sequence &v ) : _v ( v )
  {
  }
  Vector ( size_type n, const value_type &t ) : _v ( n, t )
  {
  }
  ~Vector()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector &operator = ( const Vector &v )
  {
    _v.operator = ( v._v );
    return *this;
  }
  Vector &operator = ( const Sequence &v )
  {
    _v.operator = ( v );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    return _v.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push an element onto the back of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void push_back ( const value_type &value )
  {
    _v.push_back ( value );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push an element onto the front of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void push_front ( const value_type &value )
  {
    _v.push_front ( value );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop an element off the back of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void pop_back ( const value_type &value )
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    _v.pop_back();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop an element off the front of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void pop_front ( const value_type &value )
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    _v.pop_front();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    return _v.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the capacity of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type capacity() const
  {
    return _v.capacity();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator begin()
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.begin();
  }
  const_iterator begin() const
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.begin();
  }
  iterator end()
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.end();
  }
  const_iterator end() const
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.end();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Random access.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference at ( size_type i )
  {
    ErrorChecker ( __FILE__, __LINE__, i < this->size() );
    return _v[i];
  }
  const_reference at ( size_type i ) const
  {
    ErrorChecker ( __FILE__, __LINE__, i < this->size() );
    return _v[i];
  }
  reference at ( difference_type i )
  {
    ErrorChecker ( __FILE__, __LINE__, i >= 0 );
    ErrorChecker ( __FILE__, __LINE__, i < static_cast < difference_type > ( this->size() ) );
    return _v[i];
  }
  const_reference at ( difference_type i ) const
  {
    ErrorChecker ( __FILE__, __LINE__, i >= 0 );
    ErrorChecker ( __FILE__, __LINE__, i < static_cast < difference_type > ( this->size() ) );
    return _v[i];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Array syntax.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference operator [] ( size_type i )
  {
    return this->at ( i );
  }
  const_reference operator [] ( size_type i ) const
  {
    return this->at ( i );
  }
  reference operator [] ( difference_type i )
  {
    return this->at ( i );
  }
  const_reference operator [] ( difference_type i ) const
  {
    return this->at ( i );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access first element.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference front()
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.front();
  }
  const_reference front() const
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.front();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access last element.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference back()
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.back();
  }
  const_reference back() const
  {
    ErrorChecker ( __FILE__, __LINE__, !this->empty() );
    return _v.back();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Resize.
  //
  /////////////////////////////////////////////////////////////////////////////

  void resize ( size_type i )
  {
    _v.resize ( i );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reserve.
  //
  /////////////////////////////////////////////////////////////////////////////

  void reserve ( size_type i )
  {
    _v.reserve ( i );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if the vectors are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const this_type &v ) const
  {
    return ( _v == v._v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Insert. Note: It is an error to insert beyond "end".
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator insert ( iterator pos, const value_type &x )
  {
    ErrorChecker ( __FILE__, __LINE__, pos >= _v.begin() );
    ErrorChecker ( __FILE__, __LINE__, pos <= _v.end() );
    return _v.insert ( pos, x );
  }

  template < class II > void insert ( iterator pos, II first, II last )
  {
    ErrorChecker ( __FILE__, __LINE__, pos >= _v.begin() );
    ErrorChecker ( __FILE__, __LINE__, pos <= _v.end() );
    return _v.insert ( pos, first, last );
  }

  void insert ( iterator pos, size_type n, const value_type &x )
  {
    ErrorChecker ( __FILE__, __LINE__, pos >= _v.begin() );
    ErrorChecker ( __FILE__, __LINE__, pos <= _v.end() );
    return _v.insert ( pos, n, x );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Erase. Note: It is an error to erase beyond "end".
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator erase ( iterator pos )
  {
    ErrorChecker ( __FILE__, __LINE__, pos >= _v.begin() );
    ErrorChecker ( __FILE__, __LINE__, pos <= _v.end() );
    return _v.erase ( pos );
  }

  iterator erase ( iterator first, iterator last )
  {
    ErrorChecker ( __FILE__, __LINE__, first >= _v.begin() );
    ErrorChecker ( __FILE__, __LINE__, first <= last );
    ErrorChecker ( __FILE__, __LINE__, last <= _v.end() );
    return _v.erase ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assign this vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void assign ( size_type count, const value_type &value )
  {
    return _v.assign ( count, value );
  }

  template < class II > void assign ( II first, II last )
  {
    return _v.assign ( first, last );
  }


private:

  Sequence _v;
};


}; // namespace Containers
}; // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Equality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class S, class E > inline bool operator == 
  ( const Usul::Containers::Vector<S,E> &c1, const Usul::Containers::Vector<S,E> &c2 )
{
  return c1.equal ( c2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class S, class E > inline bool operator != 
  ( const Usul::Containers::Vector<S,E> &c1, const Usul::Containers::Vector<S,E> &c2 )
{
  return !( c1.equal ( c2 ) );
}


#endif // _USUL_CONTAINERS_s_CLASSES_H_
