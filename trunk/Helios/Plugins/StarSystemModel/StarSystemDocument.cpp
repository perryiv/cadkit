
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
#include "StarSystem/Extents.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/System.h"
#include "StarSystem/RasterLayerOssim.h"
#include "StarSystem/RasterLayerWms.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Network/Names.h"
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
  _system ( 0x0 ),
  _manager ( 0x0 )
{
  USUL_TRACE_SCOPE;
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

  // Delete the star-system.
  Usul::Pointers::unreference ( _system ); _system = 0x0;

  // Clean up job manager.
  if ( 0x0 != _manager )
  {
    // Remove all queued jobs and cancel running jobs.
    _manager->cancel();

    // Wait for remaining jobs to finish.
    _manager->wait();

    // Delete the manager.
    delete _manager; _manager = 0x0;
  }
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
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
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

  // Make sure we have a system.
  this->_makeSystem();

  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );
  if ( "tiff" == ext || "tif" == ext || "jpg" == ext )
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
  filters.push_back ( Filter ( "Jpeg (*.jpg)", "*.jpg" ) );
  filters.push_back ( Filter ( "PNG  (*.png)", "*.png" ) );
  filters.push_back ( Filter ( "All Images (*.tiff *.tif * jpg *.png)", "*.tiff *.tif * jpg *.png" ) );
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

  // Make sure we have a system.
  this->_makeSystem();

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
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::preRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _system )
    _system->preRender ( caller );

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

  if ( 0x0 != _system )
    _system->postRender ( caller );

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


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::ITextMatrix::QueryPtr tm ( caller );
  if ( tm.valid() )
  {
    const unsigned int queued    ( ( 0x0 == _manager ) ? 0 : _manager->numJobsQueued() );
    const unsigned int executing ( ( 0x0 == _manager ) ? 0 : _manager->numJobsExecuting() );
    const unsigned int total     ( queued + executing );

    const std::string out ( ( total > 0 ) ? ( Usul::Strings::format ( "Queued: ", queued, ", Running: ", executing ) ) : "" );

    tm->setText ( 15, 15, out, osg::Vec4f ( 1.0, 1.0, 1.0, 1.0 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the system.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::_makeSystem()
{
  USUL_TRACE_SCOPE;

  // Only make it once.
  if ( 0x0 == _manager )
  {
    _manager = new Usul::Jobs::Manager ( 5, true );
  }

  // Only make it once.
  if ( 0x0 == _system )
  {
    _system = new StarSystem::System ( *_manager );
    Usul::Pointers::reference ( _system );
  }

#if 1

  {
    const std::string url ( "http://onearth.jpl.nasa.gov/wms.cgi" );

    typedef StarSystem::RasterLayerWms::Options Options;
    Options options;
    options[Usul::Network::Names::LAYERS]  = "modis,global_mosaic";
    options[Usul::Network::Names::STYLES]  = ",visual";
    options[Usul::Network::Names::SRS]     = "EPSG:4326";
    options[Usul::Network::Names::REQUEST] = "GetMap";
    options[Usul::Network::Names::FORMAT]  = "image/jpeg";

    typedef StarSystem::Body::Extents Extents;
    typedef Extents::Vertex Vertex;
    const Extents maxExtents ( Vertex ( -180, -90 ), Vertex ( 180, 90 ) );

    StarSystem::RasterLayerWms::RefPtr layer ( new StarSystem::RasterLayerWms ( maxExtents, url, options ) );
    _system->body()->rasterAppend ( layer.get() );
  }

#endif

#if 0

  {
    // See http://sco.az.gov/imagery.htm
    const std::string url ( "http://129.219.93.216:8080/wmsconnector/com.esri.wms.Esrimap" );

    typedef StarSystem::RasterLayerWms::Options Options;
    Options options;
    options[Usul::Network::Names::LAYERS]      = "imagery";
    options[Usul::Network::Names::SRS]         = "EPSG:4326";
    options[Usul::Network::Names::REQUEST]     = "GetMap";
    options[Usul::Network::Names::FORMAT]      = "image/jpeg";
    options[Usul::Network::Names::VERSION]     = "1.1.1";
    options["transparent"]                     = "true";
    options["service"]                         = "wms";
    options["ServiceName"]                     = "arizona";

    typedef StarSystem::Body::Extents Extents;
    typedef Extents::Vertex Vertex;
    const Extents maxExtents ( Vertex ( -115.0261211891, 31.2774721445 ), Vertex ( -108.9149437491, 37.0170157412 ) );

    StarSystem::RasterLayerWms::RefPtr layer ( new StarSystem::RasterLayerWms ( maxExtents, url, options ) );
    layer->alpha ( 255, 255, 255,   0 );
    layer->alpha (   0,   0,   0,   0 );
    _system->body()->rasterAppend ( layer.get() );
  }

#endif
}
