
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/HeadSurface.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Model/Layer.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/ITriangulateGrid.h"
#include "Usul/Interfaces/ISubdivideTriangles.h"
#include "Usul/Interfaces/ISmoothTriangles.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Safe.h"

#include "osg/MatrixTransform"

#include <iostream>

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( HeadSurface );


///////////////////////////////////////////////////////////////////////////////
//
//  Output macro used below.
//
///////////////////////////////////////////////////////////////////////////////

#define OUTPUT(the_stream) if ( 0x0 != the_stream ) (*the_stream)

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

HeadSurface::HeadSurface ( const std::string &name, 
                           Modflow::Model::Layer *layer, 
                           unsigned int numSubdivide, 
                           unsigned int numSmooth,
                           const RegistryPath &regPath ) : 
  BaseClass ( name, layer, regPath ),
  _numSubdivide ( numSubdivide ),
  _numSmooth ( numSmooth )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

HeadSurface::~HeadSurface()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build timestep.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * HeadSurface::_buildTimeStep ( unsigned int timeStep, 
                                          Modflow::ModflowDocument *document, 
                                          Modflow::Model::Layer *layer, 
                                          bool hasNoData, 
                                          long noData, 
                                          const Vec3d &offset, 
                                          double scale, 
                                          std::ostream *out ) const
{
  typedef Usul::Components::Manager PluginManager;
  typedef Usul::Interfaces::ISubdivideTriangles ISubdivideTriangles;
  typedef Usul::Interfaces::ISmoothTriangles ISmoothTriangles;
  typedef Usul::Interfaces::ITriangulateGrid ITriangulateGrid;
  typedef ITriangulateGrid::Grid Grid;
  typedef OsgTools::Triangles::TriangleSet TriangleSet;
  typedef TriangleSet::Options Options;

  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );

  ITriangulateGrid::QueryPtr triangulate ( PluginManager::instance().getInterface ( ITriangulateGrid::IID ) );
  if ( ( false == triangulate.valid() ) || ( 0x0 == document ) || ( 0x0 == layer ) )
    return mt.release();

  // Get the properties we need.
  const Modflow::Model::Layer::Vec2ui gridSize ( layer->gridSize() );
  const Modflow::Model::Layer::Vec2d cellSize ( layer->cellSize() );

  // Make the grid.
  Grid grid ( gridSize[0], gridSize[1] );

  // Loop through the cells.
  for ( unsigned int i = 0; i < gridSize[0]; ++i )
  {
    for ( unsigned int j = 0; j < gridSize[1]; ++j )
    {
      double value ( noData );

      // Is the cell valid?
      Modflow::Model::Cell::RefPtr cell ( layer->cell ( gridSize[0] - i - 1, j ) );
      if ( true == cell.valid() )
      {
        // Get the cell's properties.
        const Modflow::Model::Cell::Vector &heads ( cell->vector ( Modflow::Names::HEAD_LEVELS ) );
        if ( timeStep < heads.size() )
        {
          // Get the head value.
          const double h ( heads.at ( timeStep ) );

          // See if it's the registered "no data" value.
          const bool isNoData ( ( hasNoData ) ? ( noData == ( static_cast<long> ( h * Modflow::Constants::NO_DATA_MULTIPLIER ) ) ) : false );
          if ( false == isNoData )
          {
            value = h * scale;
          }
        }
      }

      // Set the value.
      grid.at ( i, j ) = value;
    }
  }

  // Make the triangle-set.
  const Modflow::Model::Layer::Vec2d resolution ( cellSize[0] * scale, cellSize[1] * scale );
  OsgTools::Triangles::TriangleSet::RefPtr triangleSet ( triangulate->triangulateGrid ( grid, resolution, offset, noData, Usul::Interfaces::IUnknown::QueryPtr ( document ) ) );
  if ( false == triangleSet.valid() )
    return mt.release();

  // Post-processing of triangle set.
  ISubdivideTriangles::QueryPtr subdivide ( PluginManager::instance().getInterface( ISubdivideTriangles::IID ) );
  ISmoothTriangles::QueryPtr smooth ( PluginManager::instance().getInterface ( ISmoothTriangles::IID ) );

  // See if you will further process.
  const unsigned int numSubdivide ( Usul::Threads::Safe::get ( this->mutex(), _numSubdivide ) );
  const unsigned int numSmooth ( Usul::Threads::Safe::get ( this->mutex(), _numSmooth ) );
  const bool doSubdivide ( subdivide.valid() && numSubdivide > 0 );
  const bool doSmooth ( smooth.valid() && numSmooth > 0 );

  // Feedback.
  if ( doSubdivide || doSmooth )
    OUTPUT ( out ) << Usul::Strings::format ( "Smoothing head surface for: ", layer->name(), ", time step: ", timeStep ) << std::endl;

  // Subdivide.
  if ( doSubdivide )
    subdivide->subdivideTriangles ( triangleSet.get(), numSubdivide );

  // Smooth.
  if ( doSmooth )
    smooth->smoothTriangles ( triangleSet.get(), numSmooth );

  // Build the triangle-set.
  Options options;
  options["normals"] = "per-vertex";
  mt->addChild ( triangleSet->buildScene ( options, 0x0 ) );

  // Always show both sides.
  OsgTools::State::StateSet::setTwoSidedLighting ( mt.get(), true );

  // Default color.
  const Color color ( document->color ( Modflow::Names::HEAD_SURFACE ) );

  // Default material.
  osg::ref_ptr<osg::Material> mat ( OsgTools::State::StateSet::getMaterialDefault() );
  mat->setAmbient ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( color[0], color[1], color[2], color[3] ) );
  mat->setDiffuse ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( color[0], color[1], color[2], color[3] ) );
  OsgTools::State::StateSet::setMaterial ( mt.get(), mat.get() );

  // Return the node.
  return mt.release();
}
