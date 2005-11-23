
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to set the dirty bounds flag.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_UTILITIES_DIRTY_BOUNDS_H__
#define __OSG_TOOLS_UTILITIES_DIRTY_BOUNDS_H__


#include "OsgTools/Visitor.h"

#include "osg/Node"

namespace OsgTools {
namespace Utilities {

struct SetDirtyBounds
{
  void operator () ( osg::Node * node )
  {
    node->dirtyBound();
  }
};


void setDirtyBounds( osg::Node * node )
{
  // Funtor to set the dirty bounds flag to true.
  SetDirtyBounds setter;

  // Declare the visitor that uses the functor.
  typedef OsgTools::Visitor<osg::Node,SetDirtyBounds> Visitor;
  typedef Visitor::Ptr VisitorPtr;
  VisitorPtr visitor ( new Visitor ( setter ) );

  node->accept ( *visitor );
}

}
}

#endif // __OSG_TOOLS_UTILITIES_DIRTY_BOUNDS_H__

