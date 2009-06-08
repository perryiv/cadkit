
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  CtInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_CONTROLLER_INLINE_FUNCTIONS_H_
#define _CADKIT_CONTROLLER_INLINE_FUNCTIONS_H_

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Interfaces/ICommandLine.h"

#include "Standard/SlQueryPtr.h"
#include "Standard/SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
# include <string>
# include <iostream>
#endif

namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Main function for a translator.
//
///////////////////////////////////////////////////////////////////////////////

inline bool translate ( 
  CadKit::IUnknown *controller, 
  CadKit::IUnknown *source, 
  CadKit::IUnknown *target, 
  int argc, 
  char **argv )
{
  // It's ok to have a null target.
  if ( NULL == controller || NULL == source )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // We reference all three interfaces to make sure they aren't deleted.
  SlRefPtr<IUnknown> dummy1 ( controller );
  SlRefPtr<IUnknown> dummy2 ( source );
  SlRefPtr<IUnknown> dummy3 ( target );

  // Get the command-line interface.
  SlQueryPtr<ICommandLine> commandLine ( controller );
  if ( commandLine.isNull() )
  {
    std::cout << "Failed to obtain interface to ICommandLine from given controller." << std::endl;
    return false;
  }

  // Execute the command.
  return commandLine->execute ( argc, argv, source, target );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function for a translator.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ControllerType,
  class SourceType, 
  class TargetType
>
inline bool translate ( 
  ControllerType *controller, 
  SourceType *source, 
  TargetType *target, 
  int argc, 
  char **argv )
{
  // It's ok to have a null target.
  if ( NULL == controller || NULL == source )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // Call the other one.
  return CadKit::translate ( controller->queryInterface ( CadKit::IUnknown::IID ),
                             source->queryInterface ( CadKit::IUnknown::IID ),
                             ( target ) ? target->queryInterface ( CadKit::IUnknown::IID ) : NULL,
                             argc,
                             argv );
}


}; // namespace CadKit


#endif // _CADKIT_CONTROLLER_INLINE_FUNCTIONS_H_
