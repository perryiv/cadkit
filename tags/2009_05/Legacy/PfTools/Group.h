
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

#ifndef _PF_TOOLS_GROUP_CLASS_H_
#define _PF_TOOLS_GROUP_CLASS_H_

#include "PfTools/Export.h"

class pfNode;
class pfGroup;


namespace PfTools {


struct PF_TOOLS_EXPORT Group
{
  // Remove all the children from the node.
  static void           removeAllChildren ( ::pfGroup *group );

  // Remove all occurances of the given child from the group.
  static void           removeAllOccurances ( ::pfNode *child, ::pfGroup *group );
};


}; // namespace PfTools


#endif // _PF_TOOLS_GROUP_CLASS_H_
