
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

#include "Precompiled.h"
#include "SceneStatsOSGComponent.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/INotify.h"

#include "OsgTools/Visitor.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/MatrixTransform"
#include "osg/Array"

#include <iterator>
#include <algorithm>

using namespace SceneStats;
using namespace SceneStats::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneStatsOSGComponent , SceneStatsOSGComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Macro used for type identifications.
//
///////////////////////////////////////////////////////////////////////////////

#define TYPE_ID(Type) ( & ( typeid ( Type ) ) )


///////////////////////////////////////////////////////////////////////////////
//
//  Types used as keys.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct Vertices{};
  struct Normals{};
  struct Colors{};
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneStatsOSGComponent::SceneStatsOSGComponent() : BaseClass(),
  _objects(),
  _modes(),
  _vertices(),
  _normals(),
  _colors()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneStatsOSGComponent::~SceneStatsOSGComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneStatsOSGComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast<Usul::Interfaces::IMenuAdd*>(this);
  case ICommand::IID:
    return static_cast<ICommand*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<ICommand*>(this));
  case Usul::Interfaces::IPlugin::IID:
    return static_cast<Usul::Interfaces::IPlugin*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Collect statistical information about the scene.
//
///////////////////////////////////////////////////////////////////////////////

void SceneStatsOSGComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  // Get the interface we need.
	Usul::Interfaces::IOpenSceneGraph::ValidQueryPtr osg ( Usul::Documents::Manager::instance().activeView() );
  Usul::Interfaces::INotifyString::ValidQueryPtr hello ( caller );

  // Get the scene.
  osg::ref_ptr<osg::Node> model ( dynamic_cast < osg::Node * > ( osg->osgReferenced() ) );

  // Handle bad input.
  if ( false == model.valid() )
    return;

  // Clear the counts.
  _objects.clear();
  _modes.clear();

  // Declare the visitors.
  osg::ref_ptr<osg::NodeVisitor> geodeVisitor ( 
    OsgTools::MakeVisitor<osg::Geode>::make ( 
      Usul::Adaptors::memberFunction ( this, &SceneStatsOSGComponent::_accumulateStatsGeode ) ) );
  osg::ref_ptr<osg::NodeVisitor> mtVisitor ( 
    OsgTools::MakeVisitor<osg::MatrixTransform>::make ( 
      Usul::Adaptors::memberFunction ( this, &SceneStatsOSGComponent::_accumulateStatsMT ) ) );

  // Have the scene accept the visitors.
  model->accept ( *geodeVisitor );
  model->accept ( *mtVisitor );

  // Print the results.
  std::ostringstream out;
  this->_print ( out );

  // Notify the caller.
  hello->notify ( Usul::Interfaces::IUnknown::ValidQueryPtr ( this ), out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void SceneStatsOSGComponent::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller )
{
	// TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Compare two types.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1, class T2 > bool operator < ( const T1 &t1, const T2 &t2 )
{
  return ( ::strcmp ( t1.raw_name(), t2.raw_name() ) < 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate the statistics for the geode (part).
//
///////////////////////////////////////////////////////////////////////////////

void SceneStatsOSGComponent::_accumulateStatsMT ( osg::MatrixTransform *mt )
{
  // One more assembly.
  _objects[TYPE_ID(osg::MatrixTransform)] += 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate the statistics for the geode (part).
//
///////////////////////////////////////////////////////////////////////////////

void SceneStatsOSGComponent::_accumulateStatsGeode ( osg::Geode *geode )
{
  // Loop through all the drawables.
  unsigned int numDrawables ( geode->getNumDrawables() );
  _objects[TYPE_ID(osg::Drawable)] += numDrawables;
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode->getDrawable ( i );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if ( geometry )
    {
      // One more geometry node.
      _objects[TYPE_ID(osg::Geometry)] += 1;

      // Get the number of vertices.
      _vertices[geometry->getVertexArray()] = ( geometry->getVertexArray() ) ? geometry->getVertexArray()->getNumElements() : 0;
      _normals [geometry->getNormalArray()] = ( geometry->getNormalArray() ) ? geometry->getNormalArray()->getNumElements() : 0;
      _colors  [geometry->getColorArray()]  = ( geometry->getColorArray() )  ? geometry->getColorArray()->getNumElements()  : 0;
      //_objects[TYPE_ID(Detail::Vertices)] += ( geometry->getVertexArray() ) ? geometry->getVertexArray()->getNumElements() : 0;
      //_objects[TYPE_ID(Detail::Normals)]  += ( geometry->getNormalArray() ) ? geometry->getNormalArray()->getNumElements() : 0;
      //_objects[TYPE_ID(Detail::Colors)]   += ( geometry->getColorArray()  ) ? geometry->getColorArray() ->getNumElements() : 0;

      // Loop through all the vertex-sets. (Note: Robert calls them primitive-sets.)
      unsigned int numVertexSets ( geometry->getNumPrimitiveSets() );
      _objects[TYPE_ID(osg::PrimitiveSet)] += numVertexSets;
      for ( unsigned int j = 0; j < numVertexSets; ++j )
      {
        // Get the vertex-set.
        const osg::PrimitiveSet *vertexSet = geometry->getPrimitiveSet ( j );

        // Make sure the mode falls within our index range.
        unsigned int mode ( vertexSet->getMode() );

        // Get the number of primitives in this vertex-set. If the set is a 
        // tri-strip, then the number of primitives will be 1. If the set is
        // triangles then it will be the number of vertices divided by 3.
        unsigned int numPrims ( vertexSet->getNumPrimitives() );

        // Increment this particular vertex-set mode (e.g., tri-strip).
        _modes[mode] += numPrims;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the results.
//
///////////////////////////////////////////////////////////////////////////////

void SceneStatsOSGComponent::_print ( std::ostream &out )
{
  unsigned long numVertices ( 0 );
  for ( Referenceds::const_iterator i = _vertices.begin(); i != _vertices.end(); ++ i )
    numVertices += i->second;

  unsigned long numNormals ( 0 );
  for ( Referenceds::const_iterator i = _normals.begin(); i != _normals.end(); ++ i )
    numNormals += i->second;

  unsigned long numColors ( 0 );
  for ( Referenceds::const_iterator i = _colors.begin(); i != _colors.end(); ++ i )
    numColors += i->second;

  out << "vertex sets ........ " << _objects[TYPE_ID(osg::PrimitiveSet)] << '\n';
  out << "vertices ........... " << numVertices                          << '\n';
  out << "normals ............ " << numNormals                           << '\n';
  out << "colors ............. " << numColors                            << '\n';
  out << "drawables .......... " << _objects[TYPE_ID(osg::Drawable)]     << '\n';
  out << "geometry ........... " << _objects[TYPE_ID(osg::Geometry)]     << '\n';
  out << "point sets ......... " << _modes[GL_POINTS]                    << '\n';
  out << "lines .............. " << _modes[GL_LINES]                     << '\n';
  out << "line loops ......... " << _modes[GL_LINE_LOOP]                 << '\n';
  out << "line strips ........ " << _modes[GL_LINE_STRIP]                << '\n';
  out << "triangle sets ...... " << _modes[GL_TRIANGLES]                 << '\n';
  out << "triangle strips .... " << _modes[GL_TRIANGLE_STRIP]            << '\n';
  out << "triangle fans ...... " << _modes[GL_TRIANGLE_FAN]              << '\n';
  out << "quad sets .......... " << _modes[GL_QUADS]                     << '\n';
  out << "quad strips ........ " << _modes[GL_QUAD_STRIP]                << '\n';
}
