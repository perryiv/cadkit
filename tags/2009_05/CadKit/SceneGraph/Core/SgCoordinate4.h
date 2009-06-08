
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgCoordinate4.h: 4D coordinates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_4_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_COORDINATE_4_H_

#include "SgCoordinate.h"

#include "Standard/SlVec4.h"


namespace CadKit
{
class SG_API SgCoordinate4 : public SgCoordinate
{
public:

  typedef SgVecSequence<SlVec4f,unsigned int> Points;

  SgCoordinate4();

  virtual unsigned int  getNumPoints() const { return _points.getNumElements(); }

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

  bool					        setNumPoints ( const unsigned int &numPoints ) { return _points.setNumElements ( numPoints ); }
  bool					        setPoint ( const SlVec4f &point, const unsigned int &index ) { return _points.setElement ( point, index ); }
	bool					        setPoint ( const float &wx, const float &wy, const float &wz, const float &w, const unsigned int &index ) { return _points.setElement ( SlVec4f ( wx, wy, wz, w ), index ); }

protected:

  Points _points;

  virtual ~SgCoordinate4();

  SG_DECLARE_DYNAMIC_NODE ( SgCoordinate4, 1033952649 );
};
};

#endif
