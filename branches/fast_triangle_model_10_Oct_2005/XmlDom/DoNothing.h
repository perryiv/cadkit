
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback class that does nothing.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_CALLBACK_DO_NOTHING_H_
#define _XML_CALLBACK_DO_NOTHING_H_


namespace XML {
namespace Callback {


template < class StringType > struct DoNothing
{
  typedef StringType String;
  explicit DoNothing()
  {
  }
  void start ( const String &name, const String &value )
  {
  }
  void end ( const String &name )
  {
  }
  void clear()
  {
  }
};


}; // namespace Callback
}; // namespace XML


#endif // _XML_CALLBACK_DO_NOTHING_H_
