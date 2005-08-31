
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_READ_MODEL_H__
#define __OSG_TOOLS_READ_MODEL_H__

#include "OsgTools/Export.h"

#include <string>

namespace osg { class Node; }


namespace OsgTools
{
  OSG_TOOLS_EXPORT osg::Node* readModel ( const std::string& filename );
}


#endif //__OSG_TOOLS_READ_MODEL_H__
