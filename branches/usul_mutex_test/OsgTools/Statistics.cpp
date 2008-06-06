
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for gathering statistics about a scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "Statistics.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"

#include "OsgTools/Visitor.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/MatrixTransform"

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Statistics::Statistics() :
  _counts(),
  _greedy ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Statistics::Statistics ( const Statistics &s ) :
  _counts ( s._counts ),
  _greedy ( s._greedy )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Statistics::~Statistics()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Statistics &Statistics::operator = ( const Statistics &s )
{
  _counts = s._counts;
  _greedy = s._greedy;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear accumulated state.
//
///////////////////////////////////////////////////////////////////////////////

void Statistics::clear()
{
  _counts.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the count of a particular type.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Statistics::count ( Statistics::Type t ) const
{
  Counts::const_iterator i = _counts.find ( t );
  return ( _counts.end() == i ) ? 0 : i->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate statistics.
//
///////////////////////////////////////////////////////////////////////////////

void Statistics::accumulate ( const osg::Node *node )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  // Clear accumulated state.
  this->clear();

  // Declare the visitors.
  osg::ref_ptr<osg::NodeVisitor> geodeVisitor ( 
    OsgTools::MakeVisitor<osg::Geode>::make ( 
      Usul::Adaptors::memberFunction ( this, &Statistics::_countGeodes ) ) );
  osg::ref_ptr<osg::NodeVisitor> groupVisitor ( 
    OsgTools::MakeVisitor<osg::Group>::make ( 
    Usul::Adaptors::memberFunction ( this, &Statistics::_countGroups ) ) );
  osg::ref_ptr<osg::NodeVisitor> transformVisitor ( 
    OsgTools::MakeVisitor<osg::Transform>::make ( 
    Usul::Adaptors::memberFunction ( this, &Statistics::_countTransforms ) ) );

  // Get a non-const pointer.
  osg::Node *n = const_cast < osg::Node * > ( node );

  // Have the node accept the visitors.
  n->accept ( *geodeVisitor );
  n->accept ( *groupVisitor );
  n->accept ( *transformVisitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the group.
//
///////////////////////////////////////////////////////////////////////////////

void Statistics::_countGroups ( osg::Group * )
{
  _counts[GROUP] += 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the transform.
//
///////////////////////////////////////////////////////////////////////////////

void Statistics::_countTransforms ( osg::Transform * )
{
  _counts[TRANSFORM] += 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the geode.
//
///////////////////////////////////////////////////////////////////////////////

void Statistics::_countGeodes ( osg::Geode *geode )
{
  // Get the number of drawables.
  const unsigned int numDrawables ( geode->getNumDrawables() );
  _counts[DRAWABLE] += numDrawables;

  // Loop through all the drawables.
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode->getDrawable ( i );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if ( geometry )
    {
      // One more geometry node.
      _counts[GEOMETRY] += 1;

      // Get the number of vertices.
      _counts[VERTICES] += ( geometry->getVertexArray() ) ? geometry->getVertexArray()->getNumElements() : 0;
      _counts[NORMALS]  += ( geometry->getNormalArray() ) ? geometry->getNormalArray()->getNumElements() : 0;
      _counts[COLORS]   += ( geometry->getColorArray()  ) ? geometry->getColorArray() ->getNumElements() : 0;

      // Get the number of primitive-sets.
      const unsigned int numPrimSets ( geometry->getNumPrimitiveSets() );
      _counts[PRIMITIVE_SETS] += numPrimSets;

      // Loop through all the primitive-sets.
      for ( unsigned int j = 0; j < numPrimSets; ++j )
      {
        // Get the primitive-set. This is a single tri-strip, for example.
        const osg::PrimitiveSet *primSet = geometry->getPrimitiveSet ( j );

        // Count the various modes.
        this->_countModes ( primSet );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the modes. Note: This function is largely untested.
//
///////////////////////////////////////////////////////////////////////////////

void Statistics::_countModes ( const osg::PrimitiveSet *primSet )
{
  // Handle bad input.
  if ( 0x0 == primSet )
    return;

  // Get the number of primitives in this primitive-set. If the set is a 
  // tri-strip, then the number of primitives will be 1. If the set is
  // triangles then it will be the number of vertices divided by 3.
  const unsigned int numPrims   ( primSet->getNumPrimitives() );
  const unsigned int numIndices ( primSet->getNumIndices() );

  // See what mode it is...
  switch ( primSet->getMode() )
  {
    case osg::PrimitiveSet::TRIANGLES:
      USUL_ASSERT ( numPrims * 3 == numIndices );
      _counts[TRIANGLES] += numPrims;
      break;

    case osg::PrimitiveSet::TRIANGLE_STRIP:
      USUL_ASSERT ( numIndices >= 2 );
      _counts[TRIANGLE_STRIP] += numPrims;
      _counts[TRIANGLES]      += ( this->greedy() ) ? numIndices - 2 : 0;
      break;

    case osg::PrimitiveSet::TRIANGLE_FAN:
      USUL_ASSERT ( numIndices >= 2 );
      _counts[TRIANGLE_FAN] += numPrims;
      _counts[TRIANGLES]    += ( this->greedy() ) ? numIndices - 2 : 0;
      break;

    case osg::PrimitiveSet::QUADS:
      USUL_ASSERT ( numIndices * 4 == numPrims );
      _counts[QUADS]     += numPrims;
      _counts[TRIANGLES] += ( this->greedy() ) ? numPrims * 2 : 0;
      break;

    case osg::PrimitiveSet::QUAD_STRIP:
      USUL_ASSERT ( 0 == numIndices % 2 );
      _counts[QUAD_STRIP] += numPrims;
      _counts[QUADS]      += ( this->greedy() ) ? numIndices / 2 - 1 : 0;
      _counts[TRIANGLES]  += ( this->greedy() ) ? numIndices - 2 : 0;
      break;

    case osg::PrimitiveSet::POLYGON:
      _counts[POLYGON]   += numPrims;
      _counts[TRIANGLES] += ( this->greedy() ) ? numIndices - 2 : 0;
      break;

    default:
      USUL_ASSERT ( false ); // TODO
      break;
  }
}
