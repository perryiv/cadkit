
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OSG/Glue/Tools.h"

#include "osg/Group"

using namespace CadKit::OSG::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Organize the scene.
//
///////////////////////////////////////////////////////////////////////////////

Node^ Tools::organizeScene ( Node^ existing, Node^ given )
{
  // If the given node is null then just return the existing scene.
  if ( nullptr == given || 0x0 == given->node() )
  {
    return existing;
  }

  // If the existing scene is null then return the given node.
  if ( nullptr == existing || 0x0 == existing->node() )
  {
    return given;
  }

  // See if the existing scene has a group.
  osg::ref_ptr<osg::Group> group ( dynamic_cast < osg::Group * > ( existing->node() ) );

  // If there is a group then just add the given node.
  if ( true == group.valid() )
  {
    group->addChild ( given->node() );
  }

  // Otherwise, return a new node containing existing scene and given node.
  else
  {
    group = new osg::Group;
    group->addChild ( existing->node() );
    group->addChild ( given->node() );
  }

  return gcnew Node ( group.get() );
}
