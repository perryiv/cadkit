
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseTarget: Base class for other target database classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbBasePrecompiled.h"
#include "DbBaseTarget.h"

#include "Standard/SlPrint.h"
#include "Standard/SlPathname.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

using namespace CadKit;

SL_IMPLEMENT_CLASS ( DbBaseTarget, DbBaseObject );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseTarget::DbBaseTarget() : DbBaseObject()
{
  SL_PRINT2 ( "In DbBaseTarget::DbBaseTarget(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseTarget::~DbBaseTarget()
{
  SL_PRINT2 ( "In DbBaseTarget::~DbBaseTarget(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbBaseTarget::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbBaseTarget::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IDataTarget::IID:
    return static_cast<IDataTarget *>(this);
  default:
    return DbBaseObject::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default output filename, based on the given filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbBaseTarget::getDefaultOutputName ( const std::string &filename )
{
  SL_PRINT3 ( "In DbBaseTarget::getDefaultOutputName(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Parse the path.
  SlPathname<std::string> path ( filename );

  // Make a copy of the input filename.
  std::string out ( filename );

  // Drop the extension.
  out.resize ( out.size() - path.getExtension().size() );

  // Add the given extension.
  out += this->getFileExtension();

  // Return the output filename.
  return out;
}
