
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there is an appropriate plugin.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Conditions/HasNewModelPlugin.h"

using namespace AFW::Conditions;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

HasNewModelPlugin::HasNewModelPlugin ( bool want ) : BaseClass ( want )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

HasNewModelPlugin::~HasNewModelPlugin()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there is an appropriate plugin.
//
///////////////////////////////////////////////////////////////////////////////

bool HasNewModelPlugin::evaluate ( AFW::Core::Object * )
{
  return ( false == _want ); // TODO
}
