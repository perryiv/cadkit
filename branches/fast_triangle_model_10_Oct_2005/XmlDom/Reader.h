
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An xml reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_READER_CLASS_H_
#define _XML_READER_CLASS_H_

#include "XmlDom/DoNothing.h"

#include <algorithm>


namespace XML {


template < class PolicyType, class CallbackType > class Reader
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef PolicyType                    Policy;
  typedef CallbackType                  NodeCallback;
  typedef typename Policy::TrimPolicy   TrimPolicy;
  typedef typename Policy::String       String;
  typedef typename Policy::ErrorPolicy  ErrorPolicy;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Reader() :
    _errorPolicy(),
    _callback(),
    _trimPolicy()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor. Takes bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Reader ( const Bi &first, const Bi &last ) :
    _errorPolicy(),
    _callback(),
    _trimPolicy()
  {
    this->read ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Reader ( const Reader &r ) :
    _errorPolicy ( r._errorPolicy ),
    _callback    ( r._callback ),
    _trimPolicy  ( r._trimPolicy )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Reader()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Reader &operator = ( const Reader &r )
  {
    _errorPolicy = r._errorPolicy;
    _callback    = r._callback;
    _trimPolicy  = r._trimPolicy;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the contents from the bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > void read ( Bi start, const Bi &stop )
  {
    _errorPolicy ( 2285651998u, start != stop );

    // Tell the callback to clear its state.
    _callback.clear();

    // Skip the header.
    start = this->_skipHeader ( start, stop );

    // Call the other one.
    start = this->_read ( start, stop );

    // Should be true.
    _errorPolicy ( 3953694704u, start == stop );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the error policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  const ErrorPolicy &errorPolicy() const
  {
    return _errorPolicy;
  }
  ErrorPolicy &errorPolicy()
  {
    return _errorPolicy;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the trim policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  const TrimPolicy &trimPolicy() const
  {
    return _trimPolicy;
  }
  TrimPolicy &trimPolicy()
  {
    return _trimPolicy;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the callback policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  const NodeCallback &callback() const
  {
    return _callback;
  }
  NodeCallback &callback()
  {
    return _callback;
  }


protected:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Skip the header.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Bi _skipHeader ( Bi start, const Bi &stop ) const
  {
    _errorPolicy ( 2208648077u, start != stop );

    // TODO.

    // Return the new starting position.
    return start;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called at the start of the name.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > void _start ( const String &name, const Bi &start, const Bi &stop )
  {
    _errorPolicy ( 2191500792u, !name.empty() );

    // Find the value.
    Bi end = std::find ( start, stop, '<' );

    // Grab the string.
    String value ( start, end );

    // Lose leading and trailing white space if we are supposed to.
    _trimPolicy ( value );

    // Call the callback.
    _callback.start ( name, value );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called at the end of the name.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _end ( const String &name )
  {
    _errorPolicy ( 2527909981u, !name.empty() );

    // Call the callback.
    _callback.end ( name );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when a name is read.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > void _notify ( const String &name, const Bi &start, const Bi &stop )
  {
    _errorPolicy ( 1930609223u, !name.empty() );

    if ( '/' == name[0] )
      this->_end ( String ( name.begin() + 1, name.end() ) );
    else
      this->_start ( name, start, stop );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read from the bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Bi _read ( Bi start, const Bi &stop )
  {
    _errorPolicy ( 4242675863u, start != stop );

    while ( start != stop )
    {
      start = this->_readNode ( start, stop );
    }

    return stop;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read from the bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Bi _readNode ( Bi start, const Bi &stop )
  {
    // See if we are at the end.
    if ( start == stop )
      return stop;

    // Find the start of the next name.
    Bi i0 = std::find ( start, stop, '<' );

    // Did we get to the end?
    if ( i0 == stop )
      return stop;

    // Find the end of the next name.
    std::advance ( i0, 1 );
    Bi i1 = std::find ( i0, stop, '>' );

    // Grab the name.
    String name ( i0, i1 );

    // Look for the end of this name.
    String s ( String ( "</" ) + name + String ( ">" ) );
    std::advance ( i1, 1 );
    Bi i2 = std::search ( i1, stop, s.begin(), s.end() );

    // Notify the client.
    this->_notify ( name, i1, i2 );

    // Call this function recursively.
    return this->_readNode ( i1, i2 );
  }


private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  ErrorPolicy  _errorPolicy;
  NodeCallback _callback;
  TrimPolicy   _trimPolicy;
};


}; // namespace XML


#endif // _XML_READER_CLASS_H_
