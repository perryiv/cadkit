
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgCoordinate.h: Abstract base coordinate class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_H_

#include "SgNode.h"
#include "SgVecSequence.h"


namespace CadKit
{
class SG_API SgCoordinate : public SgNode
{
public:

  virtual unsigned int      getNumPoints() const = 0;

protected:

  SgCoordinate();
  virtual ~SgCoordinate();

  SG_DECLARE_NODE ( SgCoordinate, 1033952647 );
};

}; // namespace CadKit;

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_H_
