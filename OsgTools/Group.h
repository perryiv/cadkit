
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

#ifndef _OSG_TOOLS_GROUP_CLASS_H_
#define _OSG_TOOLS_GROUP_CLASS_H_

#include "Export.h"
#include "Declarations.h"


namespace OsgTools {


struct OSG_TOOLS_EXPORT Group
{
  // Remove all the children from the node.
  static void           removeAllChildren ( osg::Group *group );

  // Remove all occurances of the given child from the group.
  static void           removeAllOccurances ( osg::Node *child, osg::Group *group );
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_GROUP_CLASS_H_
