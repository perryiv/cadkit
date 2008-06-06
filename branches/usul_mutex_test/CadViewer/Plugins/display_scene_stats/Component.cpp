
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Component.h"
#include "OpenGlCheck.h"

#include "CadViewer/ErrorChecker.h"

#include "Usul/Adaptors/MemberFunction.h"

#include "OsgTools/Visitor.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/MatrixTransform"

#include <iterator>
#include <algorithm>

using namespace CV;
using namespace CV::Plugins;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Component , Component::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Indices used in the count-vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  namespace Plugins
  {
    namespace Detail
    {
      const unsigned long MIN_PRIM_MODE   ( CV::Plugins::Detail::PrimitiveModes::MIN_MODE );  // 3
      const unsigned long MAX_PRIM_MODE   ( CV::Plugins::Detail::PrimitiveModes::MAX_MODE );  // 7
      const unsigned long NUM_PRIM_MODES  ( CV::Plugins::Detail::PrimitiveModes::NUM_MODES ); // 5
      const unsigned long NUM_ASSEMBLIES  ( NUM_PRIM_MODES );
      const unsigned long NUM_PARTS       ( 1 + NUM_ASSEMBLIES );
      const unsigned long NUM_SHAPES      ( 1 + NUM_PARTS );
      const unsigned long NUM_VERTEX_SETS ( 1 + NUM_SHAPES );
      const unsigned long NUM_VERTICES    ( 1 + NUM_VERTEX_SETS );
      const unsigned long NUM_NORMALS     ( 1 + NUM_VERTICES );
      const unsigned long NUM_COLORS      ( 1 + NUM_NORMALS );
      const unsigned long NUM_DRAWABLES   ( 1 + NUM_COLORS );
      const unsigned long NUM_GEOMETRY    ( 1 + NUM_DRAWABLES );
      const unsigned long VECTOR_SIZE     ( 1 + NUM_GEOMETRY );
    };
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::Component() : BaseClass(),
  _counts ( Detail::VECTOR_SIZE, 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::~Component()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Component::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case VRV::Interfaces::IPostModelLoad::IID:
    return static_cast<VRV::Interfaces::IPostModelLoad*>(this);
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Display some statistical information about the model.
//
///////////////////////////////////////////////////////////////////////////////

void Component::postProcessModelLoad ( const std::string &filename, osg::Node *model )
{
  CV::ErrorChecker ( 1075447107, 0x0 != model );
  CV::ErrorChecker ( 1075447108, false == filename.empty() );

  // Make sure the counts are all zero.
  _counts.assign ( _counts.size(), 0 );

  // Declare the visitors.
  osg::ref_ptr<osg::NodeVisitor> geodeVisitor ( 
    OsgTools::MakeVisitor<osg::Geode>::make ( 
      Usul::Adaptors::memberFunction ( this, &Component::_accumulateStatsGeode ) ) );
  osg::ref_ptr<osg::NodeVisitor> mtVisitor ( 
    OsgTools::MakeVisitor<osg::MatrixTransform>::make ( 
      Usul::Adaptors::memberFunction ( this, &Component::_accumulateStatsMT ) ) );

  // Have the scene accept the visitors.
  model->accept ( *geodeVisitor );
  model->accept ( *mtVisitor );

  // Print the results.
  this->_print ( filename, std::cout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate the statistics for the geode (part).
//
///////////////////////////////////////////////////////////////////////////////

void Component::_accumulateStatsMT ( osg::MatrixTransform *mt )
{
  CV::ErrorChecker ( 1075513185, 0x0 != mt );

  // One more assembly.
  _counts[Detail::NUM_ASSEMBLIES] += 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate the statistics for the geode (part).
//
///////////////////////////////////////////////////////////////////////////////

void Component::_accumulateStatsGeode ( osg::Geode *geode )
{
  CV::ErrorChecker ( 1075454714, 0x0 != geode );

  // One more part.
  _counts[Detail::NUM_PARTS] += 1;

  // Loop through all the drawables.
  unsigned int numDrawables ( geode->getNumDrawables() );
  _counts[Detail::NUM_DRAWABLES] += numDrawables;
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode->getDrawable ( i );
    CV::ErrorChecker ( 1075456069, 0x0 != drawable );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if ( geometry )
    {
      // One more geometry node.
      _counts[Detail::NUM_GEOMETRY] += 1;

      // Get the number of vertices.
      _counts[Detail::NUM_VERTICES] += ( geometry->getVertexArray() ) ? geometry->getVertexArray()->getNumElements() : 0;
      _counts[Detail::NUM_NORMALS]  += ( geometry->getNormalArray() ) ? geometry->getNormalArray()->getNumElements() : 0;
      _counts[Detail::NUM_COLORS]   += ( geometry->getColorArray()  ) ? geometry->getColorArray() ->getNumElements() : 0;

      // Loop through all the vertex-sets. (Note: Robert calls them primitive-sets.)
      unsigned int numVertexSets ( geometry->getNumPrimitiveSets() );
      _counts[Detail::NUM_VERTEX_SETS] += numVertexSets;
      for ( unsigned int j = 0; j < numVertexSets; ++j )
      {
        // Get the vertex-set.
        const osg::PrimitiveSet *vertexSet = geometry->getPrimitiveSet ( j );
        CV::ErrorChecker ( 1075457359, 0x0 != vertexSet );

        // Make sure the mode falls within our index range.
        unsigned long mode ( vertexSet->getMode() );
        CV::ErrorChecker ( 1075503364u, mode >= Detail::MIN_PRIM_MODE );
        CV::ErrorChecker ( 1075503365u, mode <= Detail::MAX_PRIM_MODE );

        // Get the number of primitives in this vertex-set. If the set is a 
        // tri-strip, then the number of primitives will be 1. If the set is
        // triangles then it will be the number of vertices divided by 3.
        unsigned int numPrims ( vertexSet->getNumPrimitives() );

        // Increment this particular vertex-set mode (e.g., tri-strip).
        _counts[vertexSet->getMode()] += numPrims;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the results.
//
///////////////////////////////////////////////////////////////////////////////

void Component::_print ( const std::string &filename, std::ostream &out ) const
{
  CV::ErrorChecker ( 1075505934, false == filename.empty() );
  CV::ErrorChecker ( 1075505935, false == !out );

  out << "****  Statistics for: " << filename << '\n';
  out << "****    jt-assemblies....... " << _counts[Detail::NUM_ASSEMBLIES]    << '\n';
  out << "****    jt-parts ........... " << _counts[Detail::NUM_PARTS]         << '\n';
  out << "****    jt-shapes .......... " << _counts[Detail::NUM_SHAPES]        << '\n';
  out << "****    vertex sets ........ " << _counts[Detail::NUM_VERTEX_SETS]   << '\n';
  out << "****    vertices ........... " << _counts[Detail::NUM_VERTICES]      << '\n';
  out << "****    normals ............ " << _counts[Detail::NUM_NORMALS]       << '\n';
  out << "****    colors ............. " << _counts[Detail::NUM_COLORS]        << '\n';
  out << "****    drawables .......... " << _counts[Detail::NUM_DRAWABLES]     << '\n';
  out << "****    geometry ........... " << _counts[Detail::NUM_GEOMETRY]      << '\n';
  out << "****    point sets ......... " << _counts[GL_POINTS]         << '\n';
  out << "****    lines .............. " << _counts[GL_LINES]          << '\n';
  out << "****    line loops ......... " << _counts[GL_LINE_LOOP]      << '\n';
  out << "****    line strips ........ " << _counts[GL_LINE_STRIP]     << '\n';
  out << "****    triangle sets ...... " << _counts[GL_TRIANGLES]      << '\n';
  out << "****    triangle strips .... " << _counts[GL_TRIANGLE_STRIP] << '\n';
  out << "****    triangle fans ...... " << _counts[GL_TRIANGLE_FAN]   << '\n';
  out << "****    quad sets .......... " << _counts[GL_QUADS]          << '\n';
  out << "****    quad strips ........ " << _counts[GL_QUAD_STRIP]     << '\n';
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the plugin name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::getPluginName() const
{
  return "Display Scene Stats";
}
