
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGetCount: The counting visitor.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgGetCount.h"
#include "SgBackground.h"
#include "SgBaseColor.h"
#include "SgBillboard.h"
#include "SgCamera.h"
#include "SgCircle.h"
#include "SgColorWheel.h"
#include "SgShape.h"
#include "SgCone.h"
#include "SgCoordinate.h"
#include "SgCoordinate3.h"
#include "SgCoordinate4.h"
#include "SgCube.h"
#include "SgCylinder.h"
#include "SgDirectionalLight.h"
#include "SgLight.h"
#include "SgDrawStyle.h"
#include "SgEllipse.h"
#include "SgGroup.h"
#include "SgIdentity.h"
#include "SgLineSet.h"
#include "SgMaterial.h"
#include "SgNormal.h"
#include "SgOrthographicCamera.h"
#include "SgPerspectiveCamera.h"
#include "SgPointSet.h"
#include "SgQuadMesh.h"
#include "SgRectangle.h"
#include "SgRotation.h"
#include "SgScale.h"
#include "SgSelection.h"
#include "SgSeparator.h"
#include "SgShading.h"
#include "SgSphere.h"
#include "SgSquare.h"
#include "SgSwitch.h"
#include "SgTransform.h"
#include "SgTranslation.h"
#include "SgTriStrip.h"
#include "SgTriangle.h"
#include "SgUnScale.h"
#include "SgVertexSet.h"
#include "SgPrimitive.h"
#include "SgText.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include <map>
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_VISITOR(SgGetCount,SgVisitor);


///////////////////////////////////////////////////////////////////////////////
//
//  Hiding the std::map because of VC++ warning 4786. A pragma isn't helping.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
typedef std::map<const SlType *, unsigned int> CountMap;

inline CountMap *_getCountMap ( void *mapPtr ) 
{
  return (CountMap *) mapPtr;
}

#define _MAP ( CadKit::_getCountMap ( _countMap ) )
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGetCount::SgGetCount() : SgVisitor(),
  _countMap ( new CountMap )
{
  SL_PRINT2 ( "SgGetCount::SgGetCount(), this = %X\n", this );

  // Keep this a pointer so you don't get VC++ 4786 warnings.
  SL_ASSERT ( _countMap );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGetCount::~SgGetCount()
{
  SL_PRINT2 ( "SgGetCount::~SgGetCount(), this = %X\n", this );

  // Delete the map.
  if ( _MAP )
    delete _MAP;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the items.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetCount::count ( SgNode &scene )
{
  SL_PRINT2 ( "SgGetCount::count(), this = %X", this );
  SL_ASSERT ( this );

  // Visit the node.
  if ( false == scene.accept ( *this ) )
  {
    SL_ASSERT ( 0 ); // Why didn't this work?
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the count for the specified type. Call count() first.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int SgGetCount::getCount ( const SlType *type ) const
{
  // Look for a corresponding iterator.
  CountMap::const_iterator i = _MAP->find ( type );

  // Return the count, or zero if the given type is not in the map.
  return ( i != _MAP->end() ) ? i->second : 0;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Increment the count for the specified type.
//
///////////////////////////////////////////////////////////////////////////////

void SgGetCount::_incrementCount ( const SlType *type )
{
  // Look for a corresponding iterator.
  CountMap::iterator i = _MAP->find ( type );

  // If we got a good one...
  if ( i != _MAP->end() )
  {
    // Increment the count and return.
    i->second += 1;
    return;
  }

  // If we get to here then insert the first counter for this type.
  (*_MAP)[type] = 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetCount::visit ( SgNode &node )
{
  SL_ASSERT ( this );
  this->_incrementCount ( SgNode::getClassType() );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGetCount::visit ( SgGroup &group )
{
  SL_ASSERT ( this );

  // Increment this type's count.
  this->_incrementCount ( SgGroup::getClassType() );

  // Get the number of children.
  unsigned int numChildren = group.getNumChildren();

  // Initialize the result.
  bool result = true;

  // Loop through all the children.
  for ( unsigned int i = 0; i < numChildren; ++i )
  {
    // Give the i'th child this visitor.
    if ( false == group.getChild ( i )->accept ( *this ) ) 
    {
      SL_ASSERT ( 0 ); // Why didn't it work?
      result = false;  // Set this and keep going.
    }
  }

  // Return the result. This may be false but we could still have partially 
  // accurate counting.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit these kinds of nodes.
//
///////////////////////////////////////////////////////////////////////////////

SG_VISIT_FUNCTION_GET_COUNT ( SgBackground, SgAttribute );
SG_VISIT_FUNCTION_GET_COUNT ( SgBaseColor, SgAttribute );
SG_VISIT_FUNCTION_GET_COUNT ( SgBillboard, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgCamera, SgNode );
SG_VISIT_FUNCTION_GET_COUNT ( SgCircle, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgColorWheel, SgShape );
SG_VISIT_FUNCTION_GET_COUNT ( SgShape, SgNode );
SG_VISIT_FUNCTION_GET_COUNT ( SgCone, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgCoordinate, SgNode );
SG_VISIT_FUNCTION_GET_COUNT ( SgCoordinate3, SgCoordinate );
SG_VISIT_FUNCTION_GET_COUNT ( SgCoordinate4, SgCoordinate );
SG_VISIT_FUNCTION_GET_COUNT ( SgCube, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgCylinder, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgDirectionalLight, SgLight );
SG_VISIT_FUNCTION_GET_COUNT ( SgLight, SgNode );
SG_VISIT_FUNCTION_GET_COUNT ( SgDrawStyle, SgAttribute );
SG_VISIT_FUNCTION_GET_COUNT ( SgEllipse, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgIdentity, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgLineSet, SgVertexSet );
SG_VISIT_FUNCTION_GET_COUNT ( SgMaterial, SgAttribute );
SG_VISIT_FUNCTION_GET_COUNT ( SgNormal, SgNode );
SG_VISIT_FUNCTION_GET_COUNT ( SgOrthographicCamera, SgCamera );
SG_VISIT_FUNCTION_GET_COUNT ( SgPerspectiveCamera, SgCamera );
SG_VISIT_FUNCTION_GET_COUNT ( SgPointSet, SgVertexSet );
SG_VISIT_FUNCTION_GET_COUNT ( SgQuadMesh, SgVertexSet );
SG_VISIT_FUNCTION_GET_COUNT ( SgRectangle, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgRotation, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgScale, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgSelection, SgSeparator );
SG_VISIT_FUNCTION_GET_COUNT ( SgSeparator, SgGroup );
SG_VISIT_FUNCTION_GET_COUNT ( SgShading, SgAttribute );
SG_VISIT_FUNCTION_GET_COUNT ( SgSphere, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgSquare, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgSwitch, SgGroup );
SG_VISIT_FUNCTION_GET_COUNT ( SgTransform, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgTranslation, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgTriStrip, SgVertexSet );
SG_VISIT_FUNCTION_GET_COUNT ( SgTriangle, SgPrimitive );
SG_VISIT_FUNCTION_GET_COUNT ( SgUnScale, SgTransformation );
SG_VISIT_FUNCTION_GET_COUNT ( SgVertexSet, SgShape );
SG_VISIT_FUNCTION_GET_COUNT ( SgPrimitive, SgShape );
SG_VISIT_FUNCTION_GET_COUNT ( SgText, SgShape );
