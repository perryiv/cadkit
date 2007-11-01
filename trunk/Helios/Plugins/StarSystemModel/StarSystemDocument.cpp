
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for star-systems.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystemDocument.h"

#include "StarSystem/BuildScene.h"
#include "StarSystem/Pager.h"
#include "StarSystem/System.h"
#include "StarSystem/RasterLayerOssim.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Case.h"
#include "Usul/Trace/Trace.h"

#include "osg/MatrixTransform"
#include "osg/CoordinateSystemNode"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( StarSystemDocument, StarSystemDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::StarSystemDocument() : BaseClass ( "StarSystem Document" ),
  _system ( new StarSystem::System() )
{
  USUL_TRACE_SCOPE;
  _system->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::~StarSystemDocument()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &StarSystemDocument::_destroy ), "1397556136" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::_destroy()
{
  USUL_TRACE_SCOPE;
  _system->unref(); _system = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *StarSystemDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IDatabasePager::IID:
    return static_cast < Usul::Interfaces::IDatabasePager* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool StarSystemDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool StarSystemDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ball" || ext == "tif" || ext == "tiff" || ext == "jpg" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool StarSystemDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ball" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool StarSystemDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ball" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::write ( const std::string &name, Unknown *caller  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );
  if ( "ball" == ext )
  {
#if 1

    StarSystem::Body::ValidRefPtr body ( new StarSystem::Body ( Usul::Math::Vec2d ( osg::WGS_84_RADIUS_EQUATOR * 0.75, osg::WGS_84_RADIUS_POLAR * 0.5 ) ) );
    const Usul::Math::Vec3d c ( _system->center() );
    body->center ( Usul::Math::Vec3d ( c[0] + 50000000, c[1], c[2] ) );
    _system->add ( body.get() );

#endif

#if 0

    Usul::Adaptors::Random<double> random ( 0, 500000000 );
    for ( unsigned int i = 0; i < 2; ++i )
    {
      StarSystem::System::ValidRefPtr system ( new StarSystem::System() );
      system->center ( StarSystem::System::Vec3d ( random(), random(), random() ) );
      _system->add ( system.get() );
    }

#endif
  }
  else if ( "tiff" == ext || "tif" == ext || "jpg" == ext )
  {
    StarSystem::RasterLayerOssim::RefPtr layer ( new StarSystem::RasterLayerOssim );
    layer->open ( name );
    _system->body()->rasterAppend ( layer.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::Filters StarSystemDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "StarSystem (*.ball)", "*.ball" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::Filters StarSystemDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  return this->filtersSave();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::Filters StarSystemDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "StarSystem (*.ball)", "*.ball" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::Filters StarSystemDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "StarSystem (*.ball)", "*.ball" ) );
  filters.push_back ( Filter ( "Tiff (*.tiff *.tif)", "*.tiff *.tif" ) );
  filters.push_back ( Filter ( "Jpeg (*.jpg)",        "*.jpg" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *StarSystemDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0x0 == _system )
    return new osg::Group();

  osg::ref_ptr<osg::Node> node ( 0x0 );
  {
    StarSystem::BuildScene::RefPtr builder ( new StarSystem::BuildScene ( options, caller ) );
    _system->accept ( *builder );
    node = builder->scene();
  }
  return node.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database pager.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::DatabasePager *StarSystemDocument::getDatabasePager()
{
  USUL_TRACE_SCOPE;
  return 0x0; // StarSystem::Pager::instance().pager();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::preRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  StarSystem::Pager::instance().preRender ( caller );
  BaseClass::preRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::postRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::postRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to this document.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::addView ( Usul::Interfaces::IView *view )
{
  BaseClass::addView ( view );
  StarSystem::Pager::instance().initVisitors ( view );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the view from this document.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::removeView ( Usul::Interfaces::IView *view )
{
  BaseClass::removeView ( view );
}
