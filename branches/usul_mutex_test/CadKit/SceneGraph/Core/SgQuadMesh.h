
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgQuadMesh.h: Line set class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_QUAD_MESH_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_QUAD_MESH_H_

#include "SgVertexSet.h"

namespace CadKit
{
class SG_API SgQuadMesh : public SgVertexSet
{
public:

  SgQuadMesh();

  const SlInt32 &      getNumRows() const { return _numRows; }
  const SlInt32 &      getNumColumns() const { return _numColumns; }

  void          setSize ( const SlInt32 &numRows, const SlInt32 &numColumns );

protected:

  SlInt32 _numRows;
  SlInt32 _numColumns;

  virtual ~SgQuadMesh();

  SG_DECLARE_DYNAMIC_NODE(SgQuadMesh,0x00001023);
};
};

#endif
