
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for scene files.
//
///////////////////////////////////////////////////////////////////////////////

#include "VectorFieldDocument.h"

#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/Mesh.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "MenuKit/Menu.h"
#include "MenuKit/ToggleButton.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"
#include "osg/PolygonMode"

#include "osgFX/Scribe"

#include "osgGA/GUIEventAdapter"

#include "osgManipulator/Translate2DDragger"

#include <fstream>
#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VectorFieldDocument, VectorFieldDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VectorFieldDocument::VectorFieldDocument() : BaseClass ( "VectorField Document" ),
_field(),
_commandManager ( new osgManipulator::CommandManager ),
_root ( new osg::Group ),
_allowAddParticles ( false )
{
  Grid2D grid ( 20, 20 );
  _field.grid ( grid );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VectorFieldDocument::~VectorFieldDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VectorFieldDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IIntersectListener::IID:
    return static_cast < Usul::Interfaces::IIntersectListener* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd* > ( this );
  case Usul::Interfaces::IMouseEventListener::IID:
    return static_cast < Usul::Interfaces::IMouseEventListener * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VectorFieldDocument::canExport ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VectorFieldDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VectorFieldDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VectorFieldDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" || ext == "osg" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

VectorFieldDocument::Filters VectorFieldDocument::filtersSave() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

VectorFieldDocument::Filters VectorFieldDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

VectorFieldDocument::Filters VectorFieldDocument::filtersOpen() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

VectorFieldDocument::Filters VectorFieldDocument::filtersInsert() const
{
  return this->filtersOpen();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *VectorFieldDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  _root->removeChild ( 0, _root->getNumChildren() );

  Grid2D& grid ( _field.grid() );

  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );

  for ( unsigned int i = 0; i < grid.rows(); ++i )
  {
    for ( unsigned int j = 0; j < grid.rows(); ++j )
    {
      Cell2D::RefPtr cell ( grid.cell ( i, j ) );

      if ( cell.valid() )
      {
        Triangle::RefPtr t0 ( cell->upper() );

        vertices->push_back ( grid.point ( t0->indices()[0] ) );
        vertices->push_back ( grid.point ( t0->indices()[1] ) );
        vertices->push_back ( grid.point ( t0->indices()[2] ) );
        normals->push_back ( osg::Vec3 ( 0.0, 0.0, 1.0 ) );

        Triangle::RefPtr t1 ( cell->lower() );
        vertices->push_back ( grid.point ( t1->indices()[0] ) );
        vertices->push_back ( grid.point ( t1->indices()[1] ) );
        vertices->push_back ( grid.point ( t1->indices()[2] ) );
        normals->push_back ( osg::Vec3 ( 0.0, 0.0, 1.0 ) );
      }
    }
  }

  {
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    geometry->setVertexArray ( vertices.get() );
    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_TRIANGLES, 0, vertices->size() ) );
    geode->addDrawable ( geometry.get() );
    
    // Make a polygon-mode.
    osg::ref_ptr<osg::PolygonMode> pm ( new osg::PolygonMode );
    pm->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );

    geode->getOrCreateStateSet()->setAttributeAndModes ( pm.get(), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

    osg::ref_ptr < osg::Material > material ( new osg::Material );
    material->setDiffuse ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
    geode->getOrCreateStateSet()->setAttributeAndModes ( material.get(), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

    osg::ref_ptr < osgFX::Scribe > scribe ( new osgFX::Scribe );
    scribe->setWireframeColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
    scribe->addChild ( geode.get() );

    //OsgTools::State::StateSet::setLighting ( geode.get(), false );

    _root->addChild ( scribe.get() );
  }

  for ( unsigned int i = 0; i < grid.numPoints(); ++i )
  {
    Grid2D::Position p ( grid.point ( i ) );
    Grid2D::Vector   v ( grid.vector ( i ) );
    if ( v.length() > 0 )
    {
      v.normalize();
      v = v * 0.2;

      OsgTools::Builders::Arrow arrow;
      arrow.start ( osg::Vec3 ( p[0], p[1], 0.0 ) );
      arrow.end ( arrow.start() + osg::Vec3 ( v[0], v[1], 0.0 ) );
      arrow.radius ( 0.025 );
      arrow.height ( 0.035 );

#if 0
      osg::ref_ptr < osgManipulator::Translate2DDragger > dragger ( new osgManipulator::Translate2DDragger ( osg::Plane ( osg::Vec3 ( 0.0, 0.0, 1.0 ), 0 ) ) );
      dragger->setMatrix ( osg::Matrix::translate ( arrow.start() ) );
      dragger->setupDefaultGeometry();
      _root->addChild ( dragger.get() );

      osg::ref_ptr < osgManipulator::Selection > selection ( new osgManipulator::Selection );
      selection->addChild ( arrow() );
      _root->addChild ( selection.get() );
      _commandManager->connect(*dragger, *selection);
#else
      _root->addChild ( arrow() );
#endif
    }
  }
  
  _root->addChild ( _field.buildTopographyGraph() );

#if 1
  
#endif

  // Return the scene
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the observer of the intersection.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::menuAdd ( MenuKit::Menu& m, Usul::Interfaces::IUnknown* caller )
{
  typedef MenuKit::ToggleButton ToggleButton;

  // Build the menu.
  MenuKit::Menu::RefPtr menu ( m.find ( "Vector Field", true ) );

  menu->append ( new ToggleButton ( Usul::Commands::genericToggleCommand ( "Add Particles", 
    Usul::Adaptors::memberFunction<void> ( this, &VectorFieldDocument::allowAddParticle ), 
    Usul::Adaptors::memberFunction<bool> ( this, &VectorFieldDocument::isAllowAddParticle ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a particle.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::_addParticle ( const osg::Vec3& position )
{
  _root->addChild ( _field.advectParticle ( position[0], position[1] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the add particle state.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::allowAddParticle( bool b )
{
  Guard guard ( this );
  _allowAddParticles = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are particles allowed to be added.
//
///////////////////////////////////////////////////////////////////////////////

bool VectorFieldDocument::isAllowAddParticle() const
{
  Guard guard ( this );
  return _allowAddParticles;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when mouse event occurs.
//
///////////////////////////////////////////////////////////////////////////////

void VectorFieldDocument::mouseEventNotify ( osgGA::GUIEventAdapter& ea, Usul::Interfaces::IUnknown * caller )
{
  // Query for the interface.
  Usul::Interfaces::ISceneIntersect::QueryPtr si ( caller );

  // See if it's the left button.
  const bool left ( Usul::Bits::has ( ea.getButton(), osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) );

  if ( left && si.valid() && osgGA::GUIEventAdapter::PUSH == ea.getEventType() && this->isAllowAddParticle())
  {
    osgUtil::LineSegmentIntersector::Intersection hit;
    if ( si->intersect ( ea.getX(), ea.getY(), hit ) )
    {
      this->_addParticle ( hit.getLocalIntersectPoint() );
    }
  }
}
