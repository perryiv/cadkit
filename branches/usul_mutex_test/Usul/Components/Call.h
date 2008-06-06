
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Template classes and functions that help call interface functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMPONENT_INTERFACE_FUNCTION_CALL_H_
#define _USUL_COMPONENT_INTERFACE_FUNCTION_CALL_H_

#include "Usul/Components/Manager.h"
#include "Usul/Exceptions/Catch.h"


namespace Usul {
namespace Components {


///////////////////////////////////////////////////////////////////////////////
//
//  Call the given function for each plugin that supports the interface.
//  Wraps each call with a try-catch.
//
///////////////////////////////////////////////////////////////////////////////

template < class InterfaceType, class ErrorIdType, class StreamType, class FunctionType > 
inline void call ( const ErrorIdType &errorId, StreamType &out, FunctionType f )
{
  typedef typename Usul::Components::Manager PM;
  PM::UnknownSet plugs ( PM::instance().getInterfaces ( InterfaceType::IID ) );
  for ( PM::UnknownSet::iterator i = plugs.begin(); i != plugs.end(); ++i )
  {
    InterfaceType::QueryPtr plug ( *i );
    if ( plug.valid() )
    {
      InterfaceType *ptr ( plug.get() );
      if ( ptr )
      {
        try
        {
          ((*ptr).*f) ( 0x0 );
        }
        USUL_CATCH_ALL_EXCEPTIONS ( errorId, out );
      }
    }
  }
}


}; // namespace Components
}; // namespace Usul


#endif // _USUL_COMPONENT_INTERFACE_FUNCTION_CALL_H_
