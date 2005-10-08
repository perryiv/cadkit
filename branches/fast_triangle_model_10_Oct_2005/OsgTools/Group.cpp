
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for working with an osg::Group.
//
///////////////////////////////////////////////////////////////////////////////

#include "Group.h"

#include "osg/Group"

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children from the group.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllChildren ( osg::Group *group )
{
  if ( group )
  {
    while ( group->getNumChildren() )
    {
      // Since the children are held in an std::vector it should be faster to
      // remove them off the end.
      group->removeChild ( group->getNumChildren() - 1 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all occurances of the given child from the group.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllOccurances ( osg::Node *child, osg::Group *group )
{
  // Handle bad input.
  if ( 0x0 == child || 0x0 == group )
    return;

  // Loop until the child is no longer under the group.
  while ( group->removeChild ( child ) ){}
}
