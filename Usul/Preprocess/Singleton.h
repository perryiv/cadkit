
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macros for making a singleton class.
//  Note: could not get this to work as a template.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREPROCESS_SINGLETON_MACRO_
#define _USUL_PREPROCESS_SINGLETON_MACRO_

#include "Usul/Export/Export.h"
#include "Usul/Factory/BaseFactory.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Define a singleton class.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DEFINE_SINGLETON(singleton_type,base_type)\
singleton_type : public base_type\
{\
public:\
  typedef base_type BaseClass;\
  typedef Usul::Threads::Mutex Mutex;\
  typedef Usul::Threads::Guard<Mutex> Guard;\
  static BaseClass &instance()\
  {\
    Guard guard ( _classMutex );\
    if ( 0x0 == _instance )\
    {\
      _instance = new BaseClass;\
    }\
    return *_instance;\
  }\
  static void instance ( BaseClass *f )\
  {\
    Guard guard ( _classMutex );\
    if ( 0x0 != _instance )\
    {\
      delete _instance;\
    }\
    _instance = f;\
  }\
private:\
  singleton_type();\
  ~singleton_type();\
  static Mutex _classMutex;\
  static BaseClass *_instance;\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Instantiate a singleton class.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_INSTANTIATE_SINGLETON(singleton_type)\
  singleton_type::Mutex singleton_type::_classMutex;\
  singleton_type::BaseClass *singleton_type::_instance = 0x0


#endif // _USUL_PREPROCESS_SINGLETON_MACRO_
