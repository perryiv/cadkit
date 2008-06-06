
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

#include "Precompiled.h"
#include "Group.h"

#include "Performer/pf/pfGroup.h"

using namespace PfTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children from the group.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllChildren ( ::pfGroup *group )
{
  if ( group )
  {
    while ( group->getNumChildren() )
    {
      // Banking on it being faster to remove them off the end.
      ::pfNode *child = group->getChild ( group->getNumChildren() - 1 );
      group->removeChild ( child );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all occurances of the given child from the group.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllOccurances ( ::pfNode *child, ::pfGroup *group )
{
  // Handle bad input.
  if ( child && group )
  {
    // Loop until the child is no longer under the group.
    while ( group->removeChild ( child ) ){}
  }
}
