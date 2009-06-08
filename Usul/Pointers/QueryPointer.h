
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Works like ATL's CComQIPtr.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_QUERY_INTERFACE_POINTER_CLASS_H_
#define _USUL_QUERY_INTERFACE_POINTER_CLASS_H_

#include "Usul/Pointers/SmartPointer.h"


namespace Usul {
namespace Pointers {


template
<
  class NewInterface_, 
  class Config_
>
class QueryPointer : public SmartPointer
<
  NewInterface_,
  Config_
>
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef NewInterface_ NewInterface;
  typedef Config_ Config;
  typedef SmartPointer < NewInterface, Config > BaseClass;
  typedef QueryPointer < NewInterface, Config > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor. Needed for containers.
  //
  /////////////////////////////////////////////////////////////////////////////

  QueryPointer() : BaseClass ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from arbitrary interface pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_ > QueryPointer ( OldInterface_ *ptr ) : 
    BaseClass ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor from another smart-pointer (the base class).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_, class ConfigType_ > 
  QueryPointer ( SmartPointer < OldInterface_, ConfigType_ > &ptr ) : 
    BaseClass ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  QueryPointer ( const QueryPointer &ptr ) : BaseClass ( ptr )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from arbitrary interface pointer.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_ > QueryPointer &operator = ( OldInterface_ *ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    BaseClass::operator = ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) );

    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from another smart-pointer (the base class).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_, class ConfigType_ >
  QueryPointer &operator = ( SmartPointer < OldInterface_, ConfigType_ > &ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    BaseClass::operator = ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) );

    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment from another query-pointer. 
  //  This is needed so the compiler does not generate this function.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OldInterface_, class ConfigType_ >
  QueryPointer &operator = ( QueryPointer < OldInterface_, ConfigType_ > &ptr )
  {
    // Set this instance from the result of the safe query (which may be null).
    BaseClass::operator = ( static_cast<NewInterface *> 
      ( ( ptr ) ? ptr->queryInterface ( NewInterface::IID ) : 0x0 ) );

    return *this;
  }
};


} // namespace Pointers
} // namespace Usul


#endif // _USUL_QUERY_INTERFACE_POINTER_CLASS_H_
