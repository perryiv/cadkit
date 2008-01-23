
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
#include "StarSystem/ElevationLayerDem.h"
#include "StarSystem/Extents.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/RasterLayerOssim.h"
#include "StarSystem/RasterLayerWms.h"
#include "StarSystem/Scale.h"
#include "StarSystem/SetJobManager.h"
#include "StarSystem/ClampNearFar.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Network/Names.h"
#include "Usul/Strings/Case.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osg/MatrixTransform"
#include "osg/CoordinateSystemNode"

#include "osgUtil/CullVisitor"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( StarSystemDocument, StarSystemDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDocument::StarSystemDocument() : BaseClass ( "StarSystem Document" ),
  _system ( 0x0 ),
  _hud (),
  _manager ( 0x0 ),
  _scale ( 1 ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "system", _system );
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
  _system = 0x0;

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
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd* > ( this );
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
  return ( ext == "ball" || ext == "tif" || ext == "tiff" || ext == "jpg" || ext == "dem" );
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

void StarSystemDocument::write ( const std::string &name, Unknown *caller, Unknown *progress  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "ball" == ext )
  {
    Serialize::XML::serialize ( *this, name );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "ball" == ext )
  {
    // Deserialize the xml tree.
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( name );
    this->deserialize ( *document );

    // If we have a system...
    if ( true == _system.valid() )
    {
      // Set the job manager.
      StarSystem::SetJobManager::RefPtr setter ( new StarSystem::SetJobManager ( this->_getJobManager() ) );
      _system->accept ( *setter );
    }
  }

  else if ( "tiff" == ext || "tif" == ext || "jpg" == ext )
  {
    StarSystem::RasterLayerOssim::RefPtr layer ( new StarSystem::RasterLayerOssim );
    layer->open ( name );
    _system->body()->rasterAppend ( layer.get() );
  }

  else if ( "dem" == ext || "ras" == ext )
  {
    StarSystem::ElevationLayerDem::RefPtr layer ( new StarSystem::ElevationLayerDem );
    layer->open ( name );
    _system->body()->elevationAppend ( layer.get() );
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
  Guard guard ( this );
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
  filters.push_back ( Filter ( "Digital Elevation Model  (*.dem)", "*.dem" ) );
  filters.push_back ( Filter ( "OSSIM General Raster  (*.ras)", "*.ras" ) );
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
  Guard guard ( this );

  // Make sure we have a system.
  this->_makeSystem();

  osg::ref_ptr<osg::Group> group ( new osg::Group );
  
  StarSystem::BuildScene::RefPtr builder ( new StarSystem::BuildScene ( options, caller ) );
  _system->accept ( *builder );
  group->addChild ( builder->scene() );
  
  group->addChild ( _hud.buildScene() );
  
  return group.release();
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

  StarSystem::System::RefPtr system ( Usul::Threads::Safe::get ( this->mutex(), _system ) );
  if ( true == system.valid() )
    system->preRender ( caller );

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

  StarSystem::System::RefPtr system ( Usul::Threads::Safe::get ( this->mutex(), _system ) );
  if ( true == system.valid() )
    system->postRender ( caller );

  BaseClass::postRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to this document.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::addView ( Usul::Interfaces::IView *view )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Call the base classes on first.
  BaseClass::addView ( view );

  // Make sure we have a system.
  this->_makeSystem();

  // Get the system.
  StarSystem::System::RefPtr system ( Usul::Threads::Safe::get ( this->mutex(), _system ) );
  StarSystem::Body::RefPtr body ( ( true == system.valid() ) ? system->body() : 0x0 );
  if ( false == body.valid() )
    return;

  // Get the interface.
#if 0
  Usul::Interfaces::IClippingDistance::QueryPtr cd ( view );
  if ( cd.valid () )
    cd->setClippingDistances ( 0.001f, 10000 ); // Need to determine at run-time.
#endif
  
#if 1
  Usul::Interfaces::ICullSceneVisitor::QueryPtr csv ( view );
  if ( csv.valid() )
  {
    osg::ref_ptr<osgUtil::CullVisitor> cv ( csv->getCullSceneVisitor( 0x0 ) );
    cv->setClampProjectionMatrixCallback ( new StarSystem::ClampNearFar ( *cv ) );
    cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
  }
#endif
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

  const unsigned int queued    ( ( 0x0 == _manager ) ? 0 : _manager->numJobsQueued() );
  const unsigned int executing ( ( 0x0 == _manager ) ? 0 : _manager->numJobsExecuting() );
  
  _hud.requests ( queued );
  _hud.running ( executing );
  
  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  if ( vp.valid() )
    _hud.updateScene ( vp->width(), vp->height() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the job manager. Make it if we have to.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager *StarSystemDocument::_getJobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Only make it once.
  if ( 0x0 == _manager )
  {
    _manager = new Usul::Jobs::Manager ( 5, true );
  }

  return _manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the system.
//
//  http://nssdc.gsfc.nasa.gov/planetary/factsheet/index.html
//  http://www6.uniovi.es/solar/eng/sun.htm
//  http://nssdc.gsfc.nasa.gov/planetary/factsheet/marsfact.html
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::_makeSystem()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Only make it once.
  if ( true == _system.valid() )
    return;

  // Local typedefs to shorten the lines.
  typedef StarSystem::Body Body;
  typedef Body::Extents Extents;

  // Make the system.
  _system = new StarSystem::System ( this->_getJobManager() );

  // For use with the land model.
  typedef StarSystem::LandModelEllipsoid Land;
  const Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );

  // Make the sun.
  {
    // See http://www6.uniovi.es/solar/eng/sun.htm for sun's radius.
    Land::RefPtr land ( new Land ( radii * 108.97 ) );

    // Make a good split distance.
    const double splitDistance ( land->size() * 3 );

    // Size of the mesh.
    Body::MeshSize meshSize ( 17, 17 );

    // Add the body.
    Body::RefPtr body ( new Body ( land, this->_getJobManager(), meshSize, splitDistance ) );
    body->useSkirts ( true );
    _system->body ( body.get() );

    // Add tiles to the body.
    body->addTile ( Extents ( -180, -90,    0,   90 ) );
    body->addTile ( Extents (    0, -90,  180,   90 ) );
  }

  // Make earth.
  {
    Land::RefPtr land ( new Land ( radii ) );
    Body::RefPtr body ( new Body ( land, this->_getJobManager(), Body::MeshSize ( 17, 17 ), land->size() * 3 ) );
    body->useSkirts ( true );
    body->addTile ( Extents ( -180, -90,    0,   90 ) );
    body->addTile ( Extents (    0, -90,  180,   90 ) );
    body->center ( Body::Vec3d ( 149.6e9, 0, 0 ) );
    _system->satellites()->add ( body.get() );

    {
      const std::string url ( "http://onearth.jpl.nasa.gov/wms.cgi" );
      typedef StarSystem::RasterLayerWms::Options Options;
      Options options;
      options[Usul::Network::Names::LAYERS]  = "BMNG,global_mosaic";
      options[Usul::Network::Names::STYLES]  = "Jul,visual";
      options[Usul::Network::Names::SRS]     = "EPSG:4326";
      options[Usul::Network::Names::REQUEST] = "GetMap";
      options[Usul::Network::Names::FORMAT]  = "image/jpeg";
      typedef StarSystem::Body::Extents Extents;
      typedef Extents::Vertex Vertex;
      const Extents maxExtents ( Vertex ( -180, -90 ), Vertex ( 180, 90 ) );
      StarSystem::RasterLayerWms::RefPtr layer ( new StarSystem::RasterLayerWms ( maxExtents, url, options ) );
      body->rasterAppend ( layer.get() );
    }

    {
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
      body->rasterAppend ( layer.get() );
    }
  }

  // Make mars.
  {
    // See http://nssdc.gsfc.nasa.gov/planetary/factsheet/marsfact.html for mar's radius.
    Land::RefPtr land ( new Land ( radii * 0.532 ) );
    Body::RefPtr body ( new Body ( land, this->_getJobManager(), Body::MeshSize ( 17, 17 ), land->size() * 3 ) );
    body->useSkirts ( true );
    body->addTile ( Extents ( -180, -90,    0,   90 ) );
    body->addTile ( Extents (    0, -90,  180,   90 ) );
    body->center ( Body::Vec3d ( 227.9e9, 0, 0 ) );
    _system->satellites()->add ( body.get() );

    const std::string url ( "http://onmars.jpl.nasa.gov/wms.cgi" );
    typedef StarSystem::RasterLayerWms::Options Options;
    Options options;
    options[Usul::Network::Names::LAYERS]  = "mola";
    options[Usul::Network::Names::STYLES]  = "color";
    options[Usul::Network::Names::SRS]     = "EPSG:4326";
    options[Usul::Network::Names::REQUEST] = "GetMap";
    options[Usul::Network::Names::FORMAT]  = "image/jpeg";
    typedef StarSystem::Body::Extents Extents;
    typedef Extents::Vertex Vertex;
    const Extents maxExtents ( Vertex ( -180, -90 ), Vertex ( 180, 90 ) );
    StarSystem::RasterLayerWms::RefPtr layer ( new StarSystem::RasterLayerWms ( maxExtents, url, options ) );
    body->rasterAppend ( layer.get() );
  }

  StarSystem::Scale::RefPtr scale ( new StarSystem::Scale ( 1.0 / osg::WGS_84_RADIUS_EQUATOR ) );
  _system->accept ( *scale );
}

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

#if 0

  {
    // See http://onmars.jpl.nasa.gov
    const std::string url ( "http://onmars.jpl.nasa.gov/wms.cgi" );

    typedef StarSystem::RasterLayerWms::Options Options;
    Options options;
    options[Usul::Network::Names::LAYERS]  = "mola";
    options[Usul::Network::Names::STYLES]  = "color";
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

    //_body ( new Body ( new LandModelFlat ( 32612 ), manager ) ), // UTM 12
    //_body ( new Body ( new LandModelFlat ( "ossimLlxyProjection" ), manager ) ),

    const osg::Vec2d mn ( -115, 0 );
    const osg::Vec2d mx ( -109, 90 );

    //const osg::Vec2d mn ( -115, 31 );
    //const osg::Vec2d mx ( -109, 37 );

    //const osg::Vec2d mn ( -180, -90 );
    //const osg::Vec2d mx (  180,  90 );

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::menuAdd ( MenuKit::Menu& menuBar, Usul::Interfaces::IUnknown* caller )
{
  typedef MenuKit::Button Button;
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::RadioButton RadioButton;

  // Build the menu.
  MenuKit::Menu::RefPtr m ( menuBar.find ( "StarSystem", true ) );

  MenuKit::Menu::RefPtr scale ( new MenuKit::Menu ( "Scale" ) );
  scale->append ( new RadioButton ( 
                Usul::Commands::genericCheckCommand ( "1.0", 
                Usul::Adaptors::bind1<void> ( 1.0, 
                                              Usul::Adaptors::memberFunction<void> ( this, &StarSystemDocument::scale ) ), 
                Usul::Adaptors::bind1<bool> ( 1.0, 
                                              Usul::Adaptors::memberFunction<bool> ( this, &StarSystemDocument::isScale ) ) ) ) );
  scale->append ( new RadioButton ( 
                Usul::Commands::genericCheckCommand ( "1.0 / 1000.0", 
                Usul::Adaptors::bind1<void> ( 1.0 / 1000.0, 
                                              Usul::Adaptors::memberFunction<void> ( this, &StarSystemDocument::scale ) ), 
                Usul::Adaptors::bind1<bool> ( 1.0 / 1000.0, 
                                              Usul::Adaptors::memberFunction<bool> ( this, &StarSystemDocument::isScale ) ) ) ) );
  scale->append ( new RadioButton ( 
                Usul::Commands::genericCheckCommand ( "1.0 / Earth radius", 
                Usul::Adaptors::bind1<void> ( 1.0 / osg::WGS_84_RADIUS_EQUATOR, 
                                              Usul::Adaptors::memberFunction<void> ( this, &StarSystemDocument::scale ) ), 
                Usul::Adaptors::bind1<bool> ( 1.0 / osg::WGS_84_RADIUS_EQUATOR, 
                                              Usul::Adaptors::memberFunction<bool> ( this, &StarSystemDocument::isScale ) ) ) ) );


  m->append ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
///////////////////////////////////////////////////////////////////////////////

void StarSystemDocument::scale ( double scale )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _scale = scale;

  StarSystem::Scale::RefPtr visitor ( new StarSystem::Scale ( scale ) );
  _system->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the scale the current value?
//
///////////////////////////////////////////////////////////////////////////////

bool StarSystemDocument::isScale ( double scale ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _scale == scale;
}
