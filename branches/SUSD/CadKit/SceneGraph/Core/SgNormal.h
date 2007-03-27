
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgNormal.h: The normals.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_NORMAL_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_NORMAL_H_

#include "SgNode.h"
#include "SgVecSequence.h"

#include "Standard/SlVec3.h"


namespace CadKit
{
class SG_API SgNormal : public SgNode
{
public:

  typedef SgVecSequence<SlVec3f,unsigned int> Normals;

  SgNormal();

  virtual unsigned int  getNumNormals() const { return _normals.getNumElements(); }

  virtual bool          isEqualVisibleProperties ( const SgNode &node ) const;

  bool					        setNumNormals ( const unsigned int &numNormals ) { return _normals.setNumElements ( numNormals ); }
  bool					        setNormal ( const SlVec3f &normal, const unsigned int &index ) { return _normals.setElement ( normal, index ); }
	bool					        setNormal ( const float &x, const float &y, const float &z, const unsigned int &index ) { return _normals.setElement ( SlVec3f ( x, y, z ), index ); }

protected:

  Normals _normals;

  virtual ~SgNormal();

  SG_DECLARE_DYNAMIC_NODE ( SgNormal, 1033952650 );
};
};

#endif
