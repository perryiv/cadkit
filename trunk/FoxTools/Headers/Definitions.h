
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOX_TOOLS_DEFINITION_HEADERS_H__
#define __FOX_TOOLS_DEFINITION_HEADERS_H__


///////////////////////////////////////////////////////////////////////////////
//
//  These have to be before fox headers.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <math.h>

#include "fxver.h"
#include "fxdefs.h"
#include "fxkeys.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Header files supporting local error handling.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/LastError.h"
#include "Usul/Exceptions/Canceled.h"

#include <stdexcept>
#include <iostream>
#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Template function to help with the plumbing.
//
///////////////////////////////////////////////////////////////////////////////

namespace FoxTools
{
  namespace Handler
  {
    template < class FoxClassPointer, class MapEntryPointer > 
    inline long handle ( FoxClassPointer fox, MapEntryPointer entry, const char *className, FX::FXObject *sender, FX::FXuint selector, void *ptr )
    {
      // Safely...
      try
      {
        Usul::System::LastError::init();
        return ( ( fox->* entry->func ) ( sender, selector, ptr ) );
      }

      // Catch cencel-exceptions.
      catch ( const Usul::Exceptions::Canceled & )
      {
        throw;
      }

      // Catch standard exceptions.
      catch ( const std::exception &e )
      {
        std::cout << ( ( e.what() ) ? e.what() : "Error 2381027582: Standard exception caught while dispatching event" );
      }

      // Catch all other exceptions.
      catch ( ... )
      {
        std::cout << "Error 1838942514: Unknown exception caught while dispatching event";
      }

      // Print this every time.
      std::cout << "; class name = " << ( ( className ) ? className : "" )
                << ", instance = " << fox
                << ", entry = " << entry
                << ", sender = " << ( ( sender ) ? sender->getClassName() : "" )
                << ", selector = " << selector
                << ", data = " << ptr;

      // Print system error if relevant.
      if ( false == Usul::System::LastError::message().empty() )
      {
        std::cout << ". " << Usul::System::LastError::message();
      }

      // Add a new-line and return.
      std::cout << std::endl;
      return 1;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for implementing plumbing for concrete classes.
//
///////////////////////////////////////////////////////////////////////////////

#define FOX_TOOLS_IMPLEMENT(class_name,base_class_name,mapping,num_mappings)\
FX::FXObject* class_name::manufacture()\
{\
  return new class_name;\
}\
const FX::FXMetaClass class_name::metaClass ( #class_name, class_name::manufacture, &base_class_name::metaClass, mapping, num_mappings, sizeof ( class_name::FXMapEntry ) );\
long class_name::handle ( FX::FXObject *sender, FX::FXuint selector, void *ptr )\
{\
  const FXMapEntry *entry ( ( const FXMapEntry * ) metaClass.search ( selector ) );\
  return ( ( entry ) ? ( FoxTools::Handler::handle ( this, entry, #class_name, sender, selector, ptr ) ) : ( base_class_name::handle ( sender, selector, ptr ) ) );\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for implementing plumbing for abstract classes.
//
///////////////////////////////////////////////////////////////////////////////

#define FOX_TOOLS_IMPLEMENT_ABSTRACT(class_name,base_class_name,mapping,num_mappings)\
const FX::FXMetaClass class_name::metaClass ( #class_name, 0x0, &base_class_name::metaClass, mapping, num_mappings, sizeof ( class_name::FXMapEntry ) );\
long class_name::handle ( FX::FXObject *sender, FX::FXSelector selector, void *ptr )\
{\
  const FXMapEntry *entry ( ( const FXMapEntry * ) metaClass.search ( selector ) );\
  return ( ( entry ) ? ( FoxTools::Handler::handle ( this, entry, #class_name, sender, selector, ptr ) ) : ( base_class_name::handle ( sender, selector, ptr ) ) );\
}



#endif // __FOX_TOOLS_DEFINITION_HEADERS_H__
