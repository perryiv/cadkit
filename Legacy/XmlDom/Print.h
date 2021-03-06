
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback class that prints to stdout.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_CALLBACK_PRINT_H_
#define _XML_CALLBACK_PRINT_H_


namespace XML {
namespace Callback {


struct Print
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Print() : _indent()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when the start of a node is found.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class StringType > void start ( const StringType &name, const StringType &value )
  {
    std::cout << _indent << '<' << name << ">\n";
    _indent += "  ";
    if ( !value.empty() )
      std::cout << _indent << value << '\n';
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when the end of a node is found.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class StringType > end ( const StringType &name )
  {
    if ( _indent.size() >= 2 )
      _indent.erase ( _indent.size() - 2 );
    std::cout << _indent << "</" << name << ">\n";
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear any accumulated state.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
  }

private:

  std::string _indent;
};


}; // namespace Callback
}; // namespace XML


#endif // _XML_CALLBACK_PRINT_H_
