
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgCoordinate3.h: 3D coordinates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_3_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_3_H_

#include "SgCoordinate.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
class SG_API SgCoordinate3 : public SgCoordinate
{
public:

  typedef SgVecSequence<SlVec3f,unsigned int> Points;

  SgCoordinate3();

  virtual unsigned int  getNumPoints() const { return _points.getNumElements(); }

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

  bool					        setNumPoints ( const unsigned int &numPoints ) { return _points.setNumElements ( numPoints ); }
  bool					        setPoint ( const SlVec3f &point, const unsigned int &index ) { return _points.setElement ( point, index ); }
	bool					        setPoint ( const float &x, const float &y, const float &z, const unsigned int &index ) { return _points.setElement ( SlVec3f ( x, y, z ), index ); }

protected:

  Points _points;

  virtual ~SgCoordinate3();

  SG_DECLARE_DYNAMIC_NODE ( SgCoordinate3, 1033952648 );
};
};

#endif
