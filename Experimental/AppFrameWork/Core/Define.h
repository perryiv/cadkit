
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macro definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_DEFINITIONS_H_
#define _APP_FRAME_WORK_DEFINITIONS_H_



///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define catch-block.
//
///////////////////////////////////////////////////////////////////////////////

#define AFW_CATCH_BLOCK(error_id)\
catch ( const std::exception &e )\
{\
  std::cout << "Error " << error_id << ": Standard exception caught." << std::endl;\
  if ( e.what() )\
    std::cout << ". " << e.what() << std::endl;\
}\
catch ( const std::string &e )\
{\
  std::cout << "Error " << error_id << ": Standard string exception caught." << std::endl;\
  if ( false == e.empty() )\
    std::cout << ". " << e << std::endl;\
}\
catch ( const char *e )\
{\
  std::cout << "Error " << error_id << ": String exception caught." << std::endl;\
  if ( e && ( ::strlen ( e ) > 0 ) )\
    std::cout << ". " << e << std::endl;\
}\
catch ( unsigned int e )\
{\
  std::cout << "Error " << error_id << ": Unsigned integer exception caught: " << e << std::endl;\
}\
catch ( int e )\
{\
  std::cout << "Error " << error_id << ": Integer exception caught: " << e << std::endl;\
}\
catch ( ... )\
{\
  std::cout << "Error " << error_id << ": Unknown exception caught." << std::endl;\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help declare a window.
//
///////////////////////////////////////////////////////////////////////////////

#define AFW_DECLARE_NEW_OBJECT_FUNCTOR(class_name)\
  public:\
  class New##class_name : public AFW::Core::NewObjectFunctor\
  {\
  public:\
    USUL_DECLARE_TYPE_ID ( New##class_name );\
    New##class_name();\
    virtual AFW::Core::Object *operator()();\
  protected:\
    virtual ~New##class_name();\
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help declare a window.
//
///////////////////////////////////////////////////////////////////////////////

#define AFW_DECLARE_OBJECT(class_name)\
  public:\
  USUL_DECLARE_REF_POINTERS ( class_name );\
  USUL_DECLARE_TYPE_ID ( class_name );\
  AFW_DECLARE_NEW_OBJECT_FUNCTOR ( class_name );\
  friend class New##class_name


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help implement a window.
//
///////////////////////////////////////////////////////////////////////////////

#define AFW_IMPLEMENT_OBJECT(class_name)\
  USUL_IMPLEMENT_TYPE_ID ( class_name );\
  USUL_IMPLEMENT_TYPE_ID ( class_name::New##class_name );\
  class_name::New##class_name::New##class_name(){}\
  class_name::New##class_name::~New##class_name(){}\
  AFW::Core::Object *class_name::New##class_name::operator()()\
  {\
    return new class_name;\
  }


#endif // _APP_FRAME_WORK_DEFINITIONS_H_
