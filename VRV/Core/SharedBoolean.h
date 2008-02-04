
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for sharing a double across all nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SHARED_BOOLEAN_H__
#define __SHARED_BOOLEAN_H__

#include "VRV/Core/SharedData.h"

namespace VRV {
namespace Core {

  typedef SharedData<bool> SharedBoolean;
}
}

#endif // __SHARED_BOOLEAN_H__
