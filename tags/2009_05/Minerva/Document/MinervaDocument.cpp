
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/MinervaDocument.h"
#include "Minerva/Document/KmlWriter.h"
#include "Minerva/Document/Manipulator.h"
#include "Minerva/Core/Commands/StopAnimation.h"
#include "Minerva/Core/Commands/StartAnimation.h"
#include "Minerva/Core/Commands/PauseAnimation.h"
#include "Minerva/Core/Commands/AnimationSpeed.h"
#include "Minerva/Core/Commands/AddLayer.h"
#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Core/Commands/ShowLayer.h"
#include "Minerva/Core/Commands/HideLayer.h"
#include "Minerva/Core/Commands/ToggleShown.h"
#include "Minerva/Core/Commands/ShowPastEvents.h"
#include "Minerva/Core/Commands/ChangeTimestepType.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/UserData.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Functions/MakeBody.h"
#include "Minerva/Core/Visitors/TemporalAnimation.h"
#include "Minerva/Core/Visitors/FindMinMaxDates.h"
#include "Minerva/Core/Visitors/FindObject.h"
#include "Minerva/Core/Visitors/StackPoints.h"
#include "Minerva/Core/Visitors/ResizePoints.h"
#include "Minerva/Core/Visitors/BuildLegend.h"
#include "Minerva/Core/Utilities/ClampNearFar.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Navigator.h"
#include "Minerva/Interfaces/IDataObject.h"
#include "Minerva/Interfaces/IIntersectNotify.h"
#include "Minerva/Interfaces/IRefreshData.h"

#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "OsgTools/Convert.h"
#include "OsgTools/Convert/Matrix.h"
#include "OsgTools/Font.h"
#include "OsgTools/Group.h"
#include "OsgTools/Widgets/Text.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "GN/Algorithms/Fill.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Config/UsulConfig.h"
#include "GN/Evaluate/Point.h"
#include "GN/Interpolate/Global.h"
#include "GN/Splines/Curve.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Interfaces/IAnimateMatrices.h"
#include "Usul/Interfaces/IAxes.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewMode.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Constants.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Case.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/System/ClipBoard.h"
#include "Usul/System/Host.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geode"
#include "osg/Light"

#include "osgGA/GUIEventAdapter"

#include "osgUtil/IntersectVisitor"
#include "osgUtil/CullVisitor"

#include <algorithm>
#include <limits>
#include <sstream>
#include <vector>

using namespace Minerva::Document;

USUL_IMPLEMENT_TYPE_ID ( MinervaDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::MinervaDocument ( LogPtr log ) :
  BaseClass( "Minerva Document" ),
#if USE_DISTRIBUTED == 1
  _commandsSend ( false ),
  _commandsReceive ( false ),
  _sessionName(),
  _sender ( new CommandSender ),
  _receiver ( new CommandReceiver ),
  _connection ( 0x0 ),
  _commandUpdate ( 5000 ),
  _commandJob ( 0x0 ),
#endif
  _dirty ( false ),
  _layersMenu ( new MenuKit::Menu ( "&Layers" ) ),
  _root ( new osg::Group ),
  _camera ( new osg::Camera ),
  _balloon ( 0x0 ),
  _bodies (),
  _activeBody ( 0x0 ),
  _manager ( 0x0 ),
  _hud(),
  _callback ( 0x0 ),
  _animateSettings ( new Minerva::Core::Animate::Settings ),
  _lastDate ( boost::date_time::min_date_time ),
  _global ( new TimeSpan ),
  _current ( _global ),
  _timeSpans(),
  _lastTime ( -1.0 ),
  _animationSpeed ( 1.0 ),
  _timeSpanMenu ( new MenuKit::Menu ( "Time Spans" ) ),
  _legend ( new OsgTools::Widgets::Legend ),
  _showLegend ( true ),
  _legendWidth ( 0.40f ),
  _legendHeightPerItem ( 30 ),
  _legendPadding ( 20.0f, 20.0f ),
  _legendPosition ( LEGEND_BOTTOM_RIGHT ),
  _width ( 0 ),
  _height ( 0 ),
  _showCompass ( true ),
  _allowSplit ( true ),
  _keepDetail ( false ),
  _log ( log ),
  _navigator ( new Minerva::Core::Navigator ( 0x0, 0x0 ) ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  // Serialization glue.
#if USE_DISTRIBUTED == 1
  this->_addMember ( "commands_send", _commandsSend );
  this->_addMember ( "commands_receive", _commandsReceive );
  this->_addMember ( "session_name", _sessionName );
  this->_addMember ( "connection", _connection );
#endif
  this->_addMember ( "time_spans", _timeSpans );
  this->_addMember ( "bodies", _bodies );

  _camera->setName ( "Minerva_Camera" );
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
  _camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  _camera->setCullingMode ( osg::CullSettings::NO_CULLING );

  osg::ref_ptr<osg::StateSet> ss ( _camera->getOrCreateStateSet() );

  {
    osg::ref_ptr< osg::Light > light ( new osg::Light );
    light->setLightNum ( 1 );
    light->setDiffuse( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
    light->setDirection( osg::Vec3 ( 0.0, 0.0, -1.0f ) );
    light->setPosition ( osg::Vec4 ( 0.0, 0.0, 1.0f, 0.0 ) );

    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }

  {
    osg::ref_ptr< osg::Light > light ( new osg::Light );
    light->setLightNum ( 0 );

    ss->setAttributeAndModes ( light.get(), osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  }

#ifndef _MSC_VER
#ifndef __APPLE__

  this->showLegend ( false );
  this->showCompass ( false );

  if( Usul::System::Host::name() == "viz4" )
  {
    this->showLegend ( true );
    this->legendWidth ( 0.85 );
    this->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
    this->legendHeightPerItem ( 60 );
    this->legendPosition( LEGEND_TOP_LEFT );
  }

  if ( Usul::System::Host::name() == "viz0" )
    this->showCompass ( true );

#endif
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::~MinervaDocument()
{
#if USE_DISTRIBUTED == 1
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _sender, &CommandSender::deleteSession ), "5415547030" );

  _sender = 0x0;
  _receiver = 0x0;
#endif

  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MinervaDocument::_destroy ), "1582439358" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MinervaDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IAnimationNotify::IID:
    return static_cast < Usul::Interfaces::IAnimationNotify* > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case Minerva::Interfaces::IAnimationControl::IID:
    return static_cast < Minerva::Interfaces::IAnimationControl * > ( this );
  case Minerva::Interfaces::IDirtyScene::IID:
    return static_cast < Minerva::Interfaces::IDirtyScene * > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::ICommandExecuteListener::IID:
    return static_cast < Usul::Interfaces::ICommandExecuteListener * > ( this );
  case Usul::Interfaces::IIntersectListener::IID:
    return static_cast < Usul::Interfaces::IIntersectListener * > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IJobFinishedListener::IID:
    return static_cast < Usul::Interfaces::IJobFinishedListener* > ( this );
  case Usul::Interfaces::IMouseEventListener::IID:
    return static_cast < Usul::Interfaces::IMouseEventListener* > ( this );
  case Minerva::Interfaces::ILookAtLayer::IID:
    return static_cast < Minerva::Interfaces::ILookAtLayer * > ( this );
  case Usul::Interfaces::IBusyState::IID:
    return static_cast < Usul::Interfaces::IBusyState * > ( this );
  case Usul::Interfaces::IContextMenuAdd::IID:
    return static_cast < Usul::Interfaces::IContextMenuAdd * > ( this );
#if 1 == USE_PLANETARY_NAVIGATOR
  case Usul::Interfaces::IMatrixManipulator::IID:
    return static_cast < Usul::Interfaces::IMatrixManipulator * > ( this );
  case Usul::Interfaces::IHandleSeek::IID:
    return static_cast < Usul::Interfaces::IHandleSeek * > ( this );
#endif
  case Usul::Interfaces::IProjectionMatrix::IID:
    return static_cast < Usul::Interfaces::IProjectionMatrix * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canExport ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "minerva" || ext == "kml" || ext == "kmz" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canInsert ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "minerva" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "minerva" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersExport() const
{
  Filters filters;
  filters.push_back ( Filter ( "Minerva (*.minerva)", "*.minerva" ) );
  filters.push_back ( Filter ( "Google Earth (*.kml)", "*.kml" ) );
  filters.push_back ( Filter ( "Google Earth Archive (*.kmz)", "*.kmz" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersInsert() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
//////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersOpen()   const
{
  Filters filters;
  filters.push_back ( Filter ( "Minerva (*.minerva)", "*.minerva" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Filters MinervaDocument::filtersSave()   const
{
  Filters filters;
  filters.push_back ( Filter ( "Minerva (*.minerva)", "*.minerva" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing document's data.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::read ( const std::string &filename, Unknown *caller, Unknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if ( "minerva" == ext )
  {
    // Deserialize the xml tree.
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( filename );

    // Change the current working directory to where the file lives.
    {
      Usul::Scope::CurrentDirectory cwd ( Usul::File::directory ( filename, true ) );
      this->deserialize ( *document );
    }

    // Loop through the bodies.
    for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
    {
      Body::RefPtr body ( *iter );
      if ( body.valid() )
      {
        // Set the job manager for each body.
        body->jobManager ( this->_getJobManager() );
      }
    }

    if ( false == _bodies.empty() )
      this->activeBody ( _bodies.front() );

    Guard guard ( this );
    _navigator->body ( this->activeBody() );
    _navigator->projectionMatrix ( Usul::Interfaces::IProjectionMatrix::QueryPtr ( this ) );
  }

  // Reset all the log pointers.
  this->_setLog();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name. Does not rename this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::write ( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if( "minerva" == ext )
  {
    Serialize::XML::serialize ( *this, filename );
  }
  else if( "kml" == ext || "kmz" == ext )
  {
    KmlWriter writer ( filename, caller, this );
    writer();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear any existing data.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::clear ( Unknown *caller )
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MinervaDocument::_clear ), "2171542707" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_clear()
{
  Guard guard ( this );

  this->_connectToDistributedSession();

#if USE_DISTRIBUTED == 1
  if ( _sender.valid() ) _sender->deleteSession();
#endif

  if ( _legend.valid() ) _legend->clear();

  // Clear the navigator.
  if ( _navigator.valid() )
  {
    _navigator->body ( 0x0 );
    _navigator->projectionMatrix ( 0x0 );
  }

  // Clear the bodies.
  std::for_each ( _bodies.begin(), _bodies.end(), std::mem_fun ( &Minerva::Core::TileEngine::Body::clear ) );

  // Delete the tiles.
  OsgTools::Group::removeAllChildren ( _root.get() );
  if ( 1 != _root->referenceCount() )
    std::cout << "Warning 3380491951: Dangling reference to Minerva's scene root, memory leak?" << std::endl;

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

  // Clear the bodies.
  _bodies.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_destroy()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &MinervaDocument::_clear ), "3972867384" );

  // Set other members.
  _layersMenu = 0x0;
  _root = 0x0;
  _camera = 0x0;
  _balloon = 0x0;
  _activeBody = 0x0;
  _callback = 0x0;
  _animateSettings = 0x0;
  _global = 0x0;
  _current = 0x0;
  _timeSpans.clear();
  _timeSpanMenu = 0x0;
  _legend = 0x0;
  _log = 0x0;
  _navigator = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * MinervaDocument::buildScene ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make sure we have at least one body.
  this->_makePlanet();

  // Set needed navigator data.
  _navigator->body ( this->activeBody() );
  _navigator->projectionMatrix ( Usul::Interfaces::IProjectionMatrix::QueryPtr ( this ) );
  _navigator->home();

  osg::ref_ptr<osg::Group> group ( new osg::Group );
  group->setName ( "Minerva document" );
  for ( Bodies::const_iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );

    if ( body.valid () )
      group->addChild ( body->scene() );
  }

  group->addChild ( _root.get() );
  group->addChild ( _hud.buildScene() );

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * MinervaDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  return this->buildScene ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a matrix from lat, lon, and altitude.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Matrixd makeLookAtMatrix ( double lat, double lon, double altitude, double heading, double pitch, double roll, Minerva::Core::TileEngine::Body& body )
  {
    // Get the elevation.
    const double elevation ( body.elevationAtLatLong ( lat, lon ) );

    // The height above sea level.
    const double heightAboveSeaLevel ( elevation + static_cast<double> ( Usul::Math::maximum ( 2500.0, altitude ) ) );

    // Get the rotation.
    osg::Matrixd matrix ( body.planetRotationMatrix ( lat, lon, heightAboveSeaLevel, heading ) );

    // Rotation about x.
    osg::Matrixd RX ( osg::Matrixd::rotate ( Usul::Math::DEG_TO_RAD * pitch, 1, 0, 0 ) );

    // Rotation about y.
    osg::Matrixd RY ( osg::Matrixd::rotate ( Usul::Math::DEG_TO_RAD * roll, 0, 1, 0 ) );

    osg::Matrix M ( matrix * RX * RY );
    return M;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance in meters between two points.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  double distance ( const Usul::Math::Vec3d& from, const Usul::Math::Vec3d& to, Minerva::Core::TileEngine::Body& body )
  {
    Usul::Math::Vec3d p0, p1;
    body.convertToPlanet ( from, p0 );
    body.convertToPlanet ( to, p1 );
    const double d ( p0.distance ( p1 ) );
    return d;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use trig to slow down the beginning and end of the path.
//  See http://mathworld.wolfram.com/Cosine.html
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline double slowBothEnds ( double param, double mn, double mx )
  {
    param *= Usul::Math::PIE;
    param += Usul::Math::PIE;
    param  = Usul::Math::cos ( param );
    param += 1.0;
    param /= 2.0;

    // Keep in range.
    param = ( ( param < mn ) ? mn : param );
    param = ( ( param > mx ) ? mx : param );

    return param;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from where the camera happens to be to the new coordinate.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void animatePath ( const Usul::Math::Vec3d& llh1, 
                     const Usul::Math::Vec3d& llh2, 
                     double percentMidpointHeightAtTransition,
                     const Usul::Math::Vec3ui &numPoints,
                     Minerva::Core::TileEngine::Body& body,
                     Usul::Interfaces::IUnknown::RefPtr notify )
  {
    // This is needed.
    Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );
    if ( false == vm.valid() )
      return;

    // We can live without this one.
    Usul::Interfaces::IRedraw::QueryPtr painter ( vm );

    // Needed below.
    typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorChecker;
    typedef GN::Config::UsulConfig < double, double, unsigned int, ErrorChecker > Config;
    typedef GN::Splines::Curve < Config > Curve;
    typedef Usul::Interfaces::IAnimateMatrices::Matrices Matrices;
    typedef Usul::Convert::Type < osg::Matrixd, Matrices::value_type > Converter1;
    typedef Usul::Convert::Type < Matrices::value_type, osg::Matrixd > Converter2;

    // Calculate the midpoint.
    const double distance ( Detail::distance ( llh1, llh2, body ) );
    Usul::Math::Vec3d mid ( ( llh1[0] + llh2[0] ) / 2.0, ( llh1[1] + llh2[1] ) / 2.0, distance / 2.0 );

    // Midpoint has to be as high or higher than first and last points.
    mid[2] = ( ( mid[2] < llh1[2] ) ? llh1[2] : mid[2] );
    mid[2] = ( ( mid[2] < llh2[2] ) ? llh2[2] : mid[2] );

    // Calculate the transition points.
    Usul::Math::Vec3d tp1 ( llh1[0], llh1[1], mid[2] * percentMidpointHeightAtTransition );
    Usul::Math::Vec3d tp2 ( llh2[0], llh2[1], mid[2] * percentMidpointHeightAtTransition );

    // Transition points have to be as high or higher than first and last points.
    tp1[2] = ( ( tp1[2] < llh1[2] ) ? llh1[2] : tp1[2] );
    tp1[2] = ( ( tp1[2] < llh2[2] ) ? llh2[2] : tp1[2] );
    tp2[2] = ( ( tp2[2] < llh1[2] ) ? llh1[2] : tp2[2] );
    tp2[2] = ( ( tp2[2] < llh2[2] ) ? llh2[2] : tp2[2] );

    // Container to hold the matrices.
    Matrices matrices;
    matrices.reserve ( numPoints[0] + numPoints[1] + numPoints[2] );

    // First path.
    {
      // Get the first matrix. Have to get the view matrix because we don't 
      // have enough information in llh1.
      const osg::Matrixd m1 ( vm->getViewMatrix() );
      osg::Vec3d e1, c1, up1;
      m1.getLookAt ( e1, c1, up1 );

      // Get the second matrix.
      const osg::Matrixd m2 ( osg::Matrixd::inverse ( Detail::makeLookAtMatrix ( tp1[1], tp1[0], tp1[2], 0, 0, 0, body ) ) );
      osg::Vec3d e2, c2, up2;
      m2.getLookAt ( e2, c2, up2 );

      // Loop through the path.
      unsigned int total ( numPoints[0] );
      for ( unsigned int i = 0; i < total; ++i )
      {
        double param ( static_cast < double > ( i ) / static_cast < double > ( total - 1 ) );
        param = Detail::slowBothEnds ( param, 0.0, 1.0 );
        param = Detail::slowBothEnds ( param, 0.0, 1.0 );

        const osg::Vec3d eye    (  e1 + ( (  e2 -  e1 ) * param ) );
        const osg::Vec3d center (  c1 + ( (  c2 -  c1 ) * param ) );
        const osg::Vec3d up     ( up1 + ( ( up2 - up1 ) * param ) );
        matrices.push_back ( Converter1::convert ( osg::Matrixd::lookAt ( eye, center, up ) ) );
      }
    }

    // Next path
    {
      // Make the control points.
      const Usul::Math::Vec3d llha ( tp1 );
      const Usul::Math::Vec3d llhb ( llh1[0], llh1[1],  mid[2] );
      const Usul::Math::Vec3d llhc ( mid );
      const Usul::Math::Vec3d llhd ( llh2[0], llh2[1],  mid[2] );
      const Usul::Math::Vec3d llhe ( tp2 );

      // Make the curve.
      Curve curve;
      curve.resize ( 3, 4, 5, false );

      curve.knot ( 0 ) = 0.0;
      curve.knot ( 1 ) = 0.0;
      curve.knot ( 2 ) = 0.0;
      curve.knot ( 3 ) = 0.0;
      curve.knot ( 4 ) = 0.5;
      curve.knot ( 5 ) = 1.0;
      curve.knot ( 6 ) = 1.0;
      curve.knot ( 7 ) = 1.0;
      curve.knot ( 8 ) = 1.0;

      curve.controlPoint ( 0, 0 ) = llha[0];
      curve.controlPoint ( 1, 0 ) = llha[1];
      curve.controlPoint ( 2, 0 ) = llha[2];

      curve.controlPoint ( 0, 1 ) = llhb[0];
      curve.controlPoint ( 1, 1 ) = llhb[1];
      curve.controlPoint ( 2, 1 ) = llhb[2];

      curve.controlPoint ( 0, 2 ) = llhc[0];
      curve.controlPoint ( 1, 2 ) = llhc[1];
      curve.controlPoint ( 2, 2 ) = llhc[2];

      curve.controlPoint ( 0, 3 ) = llhd[0];
      curve.controlPoint ( 1, 3 ) = llhd[1];
      curve.controlPoint ( 2, 3 ) = llhd[2];

      curve.controlPoint ( 0, 4 ) = llhe[0];
      curve.controlPoint ( 1, 4 ) = llhe[1];
      curve.controlPoint ( 2, 4 ) = llhe[2];

      // Loop through the path.
      Curve::Vector llh ( curve.dimension(), 0 );
      unsigned int total ( numPoints[1] );
      for ( unsigned int i = 0; i < total; ++i )
      {
        double param ( static_cast < double > ( i ) / static_cast < double > ( total - 1 ) );
        param = Detail::slowBothEnds ( param, 0.0, 1.0 );
        param = Detail::slowBothEnds ( param, 0.0, 1.0 );

        GN::Evaluate::point ( curve, param, llh );
        matrices.push_back ( Converter1::convert ( osg::Matrixd::inverse ( Detail::makeLookAtMatrix ( llh[1], llh[0], llh[2], 0, 0, 0, body ) ) ) );
      }
    }

    // Next path
    {
      // Get the points.
      const Usul::Math::Vec3d llha ( tp2 );
      const Usul::Math::Vec3d llhb ( llh2 );

      // Loop through the path.
      unsigned int total ( numPoints[2] );
      for ( unsigned int i = 0; i < total; ++i )
      {
        double param ( static_cast < double > ( i ) / static_cast < double > ( total - 1 ) );
        param = Detail::slowBothEnds ( param, 0.0, 1.0 );
        param = Detail::slowBothEnds ( param, 0.0, 1.0 );

        const Usul::Math::Vec3d llh ( llha + ( ( llhb - llha ) * param ) );
        matrices.push_back ( Converter1::convert ( osg::Matrixd::inverse ( Detail::makeLookAtMatrix ( llh[1], llh[0], llh[2], 0, 0, 0, body ) ) ) );
      }
    }

    // If there are no matrices then punt. This can happen if the 
    // number of points requested is (0,0,0).
    if ( true == matrices.empty() )
      return;

    // Look for animation interface.
    Usul::Interfaces::IAnimateMatrices::QueryPtr animator ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IAnimateMatrices::IID ) );
    if ( true == animator.valid() )
    {
      const unsigned int milliSeconds ( Usul::Registry::Database::instance()[Usul::Registry::Sections::PATH_ANIMATION]["curve"]["milliseconds"].get<unsigned int> ( 15, true ) );
      animator->animateMatrices ( matrices, milliSeconds, false, notify );
    }

    // Otherwise, just slam in the last one.
    else
    {
      // Set the new matrix.
      const Matrices::value_type mat ( matrices.back() );
      vm->setViewMatrix ( Converter2::convert ( mat ) );
    }

    // Simulate a seek so that mouse navigation is well-behaved.
    // Ideally this should be done in some kind of "animation finished" 
    // callback because the tiles that we are flying to have not been created 
    // yet, and the intersection test with the tiles is not very accurate.
#if 0
    Usul::Interfaces::ITrackball::QueryPtr tb ( vm );
    if ( tb.valid() )
    {
      // The translate portion of the matrix is where the eye position will be.
      const osg::Matrixd mat ( osg::Matrixd::inverse ( vm->getViewMatrix() ) );
      Usul::Math::Vec3d point ( mat ( 3, 0 ), mat ( 3, 1 ), mat ( 3, 2 ) );

      // Find the intersection point from the eye to the center of the body.  This will become the new center of the trackball.
      Usul::Math::Vec3d center;
      if ( body.intersectWithTiles ( point, Usul::Math::Vec3d ( 0.0, 0.0, 0.0 ), center ) )
      {
        // Get the distance between the eye and the center.
        const double d ( center.distance ( point ) );

        // Get the rotation and set the trackball.
        osg::Quat rot; mat.get ( rot );
        const osg::Vec3d c ( Usul::Convert::Type<Usul::Math::Vec3d,osg::Vec3d>::convert ( center ) );
        tb->setTrackball ( c, d, rot, true, true );
      }
    }
#endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the layer extents.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::lookAtLayer ( Usul::Interfaces::IUnknown * layer )
{
  Usul::Interfaces::ILayerExtents::QueryPtr le ( layer );
  const double minLon ( le.valid() ? le->minLon() : -180.0 );
  const double minLat ( le.valid() ? le->minLat() :  -90.0 );
  const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
  const double maxLat ( le.valid() ? le->maxLat() :   90.0 );

  Minerva::Core::Extents<osg::Vec2d> extents ( minLon, minLat, maxLon, maxLat );

  Body::RefPtr body ( this->activeBody() );

  if ( body.valid() )
  {
    // Get the center.
    osg::Vec2 center ( extents.center() );

    // For now use this formula to get meters per degree.
    // http://books.google.com/books?id=wu7zHtd2LO4C&pg=PA167&lpg=PA167&dq=meters+per+degree+average+earth&source=web&ots=yF1Q6sp1nP&sig=S8gdbKLvNzrXGKoUn3ha-oqYMaU&hl=en
    const double lat ( Usul::Math::DEG_TO_RAD * center[1] );
    const double metersPerDegree ( 111132.09 - 566.05 * Usul::Math::sin ( 2 * lat ) + 120 * Usul::Math::cos ( 4 * lat ) - 0.0002 * Usul::Math::cos ( 6 * lat ) );

    // Calculate an altitude.
    const double altitude ( ( extents.maximum() - extents.minimum() ).length() * metersPerDegree );

    // Make an itermediate point.
    osg::Vec3d eye ( _callback.valid() ? _callback->_eye : osg::Vec3d() );

    // Convert the eye to lat,lon, height.
    Usul::Math::Vec3d point ( eye[0], eye[1], eye[2] );
    Usul::Math::Vec3d from;
    body->convertFromPlanet( point, from );

    // Point we want to go to.
    Usul::Math::Vec3d to ( center[0], center[1], altitude );

    const double percentMidpointHeightAtTransition ( 0.75 );
    const Usul::Math::Vec3ui numPoints ( 30, 30, 30 );
    Detail::animatePath ( from, to, percentMidpointHeightAtTransition, numPoints, *body, Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to a point.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::lookAtPoint ( const Usul::Math::Vec2d& location )
{
  Body::RefPtr body ( this->activeBody() );

  if ( body.valid() )
  {
    // Make an itermediate point.
    osg::Vec3d eye ( _callback.valid() ? _callback->_eye : osg::Vec3d() );

    // Convert the eye to lat, lon, height.
    Usul::Math::Vec3d point ( eye[0], eye[1], eye[2] );
    Usul::Math::Vec3d from;
    body->convertFromPlanet ( point, from );

    // Point we want to go to.
    Usul::Math::Vec3d to ( location[0], location[1], 2500.0 /*altitude*/ );

    const double percentMidpointHeightAtTransition ( 0.75 );
    const Usul::Math::Vec3ui numPoints ( 30, 30, 30 );
    Detail::animatePath ( from, to, percentMidpointHeightAtTransition, numPoints, *body, Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::timestepType( IAnimationControl::TimestepType type )
{
  Guard guard ( this->mutex () );
  _animateSettings->timestepType( static_cast < Minerva::Core::Animate::Settings::TimestepType > ( type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep type.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Interfaces::IAnimationControl::TimestepType MinervaDocument::timestepType( ) const
{
  Guard guard ( this->mutex () );
  return static_cast < Minerva::Interfaces::IAnimationControl::TimestepType > ( _animateSettings->timestepType() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

osgGA::MatrixManipulator * MinervaDocument::getMatrixManipulator ()
{
  Guard guard ( this );
  osg::ref_ptr<Manipulator> manipulator ( new Manipulator ( _navigator.get() ) );
  return manipulator.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect the the distributed session.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_connectToDistributedSession()
{
#if USE_DISTRIBUTED == 1
  if ( _commandsSend && !_sender->connected() )
  {
    _sender->connection ( _connection.get() );
    _sender->connectToSession ( _sessionName );
  }

  if ( _commandsReceive && !_receiver->connected() )
  {
    _receiver->connection ( _connection.get() );
    _receiver->connectToSession ( _sessionName );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialze.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;

  _dataMemberMap.deserialize ( node );

  // Connect.
  this->_connectToDistributedSession ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Command has executed..
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::commandExecuteNotify ( Usul::Commands::Command* command )
{
  USUL_TRACE_SCOPE;

#if USE_DISTRIBUTED == 1

  if ( 0x0 != command )
  {
    // Send the command to the distributed client.
    if ( _sender.valid() && _commandsSend )
    {
      // Send the command.
      _sender->sendCommand ( command );
    }
  }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::startAnimation()
{
  USUL_TRACE_SCOPE;
  
  {
    Guard guard ( this->mutex () );
    _animateSettings->animate ( true );
  }
  
  this->_findFirstLastDate();
  
  // Rebuild the time spans menu.
  this->_buildTimeSpanMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::stopAnimation()
{
  USUL_TRACE_SCOPE;

  {
    Guard guard ( this->mutex () );
    _animateSettings->animate ( false );

    // Reset dates.
    if ( _global.valid () )
    {
      typedef Minerva::Core::Visitors::TemporalAnimation Visitor;
      Visitor::RefPtr visitor ( new Visitor ( _global->begin(), _global->end() ) );
      this->accept ( *visitor );
    }
  }

  // Update the text.
  _hud.dateFeedback ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::pauseAnimation()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );

  // Stop animation, but don't send the visitor to reset what nodes are shown.
  _animateSettings->animate ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::animateSpeed ( double speed )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  _animationSpeed = speed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

double MinervaDocument::animateSpeed () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _animationSpeed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show past events.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showPastEvents ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _animateSettings->showPastDays ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show past events.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::showPastEvents() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _animateSettings->showPastDays();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::preRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->preRender ( caller );
  }

  BaseClass::preRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::postRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
    {
      body->postRender ( caller );

      // This is needed.  When a job finishes, the finished callback requests a redraw.
      // In the subsequent cull traversal, the new texture will be added to the scene
      // (which sets the Body's needs redraw flag). Sometimes another redraw is needed 
      // for the texture to appear on the screen (Not sure why this is true).
      if ( body->needsRedraw() )
      {
        this->requestRedraw();
      }

      // Request has been made.  Reset state.
      body->needsRedraw ( false );
    }
  }

  BaseClass::postRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::addView ( Usul::Interfaces::IView *view )
{
  USUL_TRACE_SCOPE;

  // Call the base class' on first.
  BaseClass::addView ( view );

  // Hide the axes.
  Usul::Interfaces::IAxes::QueryPtr axes ( view );
  if ( axes.valid() )
    axes->axesShown ( false );

  Usul::Interfaces::ICullSceneVisitor::QueryPtr csv ( view );
  if ( csv.valid() )
  {
    osg::ref_ptr<osgUtil::CullVisitor> cv ( csv->getCullSceneVisitor( 0x0 ) );

#if 1
    //cv->setComputeNearFarMode ( osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES );
    cv->setClampProjectionMatrixCallback ( new Minerva::Core::Utilities::ClampNearFar ( *cv ) );
    cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
#else
    cv->setComputeNearFarMode ( osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES );
#endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to this document.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::removeView ( Usul::Interfaces::IView *view )
{
  // Call the base class' first.
  BaseClass::removeView ( view );

  // If there are no more views, remove the job finished listener.
  if ( 0 == this->numViews() )
  {
    Usul::Jobs::Manager *manager ( this->_getJobManager() );
    manager->removeJobFinishedListener ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
}


#if USE_DISTRIBUTED == 1

///////////////////////////////////////////////////////////////////////////////
//
//  Job to check for new commands.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class CheckForCommands : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;

    CheckForCommands ( Usul::Interfaces::IUnknown* caller, CommandReceiver *receiver ) :
      BaseClass ( caller, false ),
      _caller ( caller ),
      _receiver ( receiver )
    {
      Usul::Interfaces::IProgressBar::QueryPtr pb ( this->progress () );
      if ( pb.valid () )
        pb->hideProgressBar ();
    }
  protected:

    virtual ~CheckForCommands () { }

    virtual void _started ()
    {
      if ( _receiver.valid () )
      {
        std::cout << "Processing Commands" << std::endl;
        _receiver->processCommands ( _caller.get () );
      }
    }

  private:
    Usul::Interfaces::IUnknown::QueryPtr _caller;
    CommandReceiver::RefPtr _receiver;
  };
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( this->dirty() )
  {
    Minerva::Core::Visitors::StackPoints::RefPtr visitor ( new Minerva::Core::Visitors::StackPoints );
    this->accept ( *visitor );

    this->_buildLayerMenu();
  }

  // Update.
  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  std::for_each ( _bodies.begin(), _bodies.end(), std::bind2nd ( std::mem_fun ( &Body::updateNotify ), unknown.get() ) );

  // Build the scene.
  this->_buildScene ( caller );

#if USE_DISTRIBUTED == 1
  const bool jobFinished ( _commandJob.valid() ? _commandJob->isDone () : true );

  // Check to see if we should receive commands...
  if ( _commandsReceive && _commandUpdate () && jobFinished )
  {
    Usul::Jobs::Job::RefPtr job ( new Detail::CheckForCommands ( caller, _receiver.get() ) );
    Usul::Jobs::Manager::instance().addJob ( job.get () );
    _commandJob = job;
  }
#endif

  // Animate.
  this->_animate ( caller );

  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::dirtyScene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::dirtyScene ( bool b, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _dirty = b;

  // Get the active body.
  Body::RefPtr body ( this->activeBody() );
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( caller );

  if ( body.valid() && rl.valid() )
    body->rasterChanged ( rl.get() );

  // This is causing the document to prompt about saving even though 
  // no changes have been made.
  //this->modified ( true );
}


#if USE_DISTRIBUTED == 1
///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::connection ( Minerva::DataSources::PG::Connection* connection )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _connection = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::DataSources::PG::Connection* MinervaDocument::connection ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the session.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::session ( const std::string& session )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _sessionName = session;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the session.
//
///////////////////////////////////////////////////////////////////////////////

const std::string & MinervaDocument::session () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _sessionName;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to the session.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::connectToSession ()
{
  USUL_TRACE_SCOPE;
  this->_connectToDistributedSession ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the commands receive flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::commandsReceive ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _commandsReceive = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the commands receive flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::commandsReceive () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _commandsReceive;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Animate.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_animate ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );

  // If we are suppose to animate...
  if ( _animateSettings->animate() )
  {
    if ( false == _animateSettings->pause () && _current.valid() )
    {
      Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
      const double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );

      // Set the last time if it hasn't been set.
      if ( -1.0 == _lastTime )
        _lastTime = time;

      // Duration between last time the date was incremented.
      const double duration ( time - _lastTime );

      // Animate if we should.
      if ( duration > _animationSpeed )
      {
        Minerva::Core::Animate::Date lastDate ( _lastDate );

        // Increment the last date to show.
        lastDate.increment ( static_cast<Minerva::Core::Animate::Date::IncrementType> ( _animateSettings->timestepType() ), 1 );

        // Wrap if we've gone beyond the last date.
        if( lastDate > _current->end() )
        {
          lastDate = _current->begin();
        }

        // The first date to show.
        Minerva::Core::Animate::Date firstDate ( lastDate );

        // Should we show past events?
        if ( _animateSettings->showPastDays() )
        {
          firstDate = _current->begin();
        }

        if( _animateSettings->timeWindow() )
        {
          firstDate = lastDate;
          firstDate.moveBackNumDays( _animateSettings->windowLength() );
        }

        // Update the text.
        _hud.dateFeedback ( lastDate.toString() );
        _lastDate = lastDate;

        // Set the dates to show.
        Minerva::Core::Visitors::TemporalAnimation::RefPtr visitor ( new Minerva::Core::Visitors::TemporalAnimation ( firstDate, lastDate ) );
        this->accept ( *visitor );

        // Set the last time that we've updated.
        _lastTime = time;
      }
    }
    
    // Request a redraw while we are animating.
    this->requestRedraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Have visitor visit all layes.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::accept ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->accept ( visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;

  // Typedefs to help shorten lines.
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  RadioButton;

  // Namespace aliases to help shorten lines.
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;

  MenuKit::Menu::RefPtr m ( menu.find ( "&Options", true ) );
	MenuKit::Menu::RefPtr animate ( menu.find ( "&Animate", true ) );

  if ( false == m->items().empty() )
    m->addSeparator();

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  animate->append ( new Button       ( new Minerva::Core::Commands::StartAnimation ( me ) ) );
  animate->append ( new Button       ( new Minerva::Core::Commands::StopAnimation  ( me ) ) );
  animate->append ( new Button       ( new Minerva::Core::Commands::PauseAnimation ( me ) ) );
  animate->append ( new ToggleButton ( new Minerva::Core::Commands::ShowPastEvents ( me ) ) );

  MenuKit::Menu::RefPtr speed ( new MenuKit::Menu ( "Speed" ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 0.1, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 0.5, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 1.0, me ) ) );
  speed->append ( new RadioButton ( new Minerva::Core::Commands::AnimationSpeed ( 2.0, me ) ) );
  animate->append ( speed );

  MenuKit::Menu::RefPtr type ( new MenuKit::Menu ( "Timestep" ) );
  typedef Minerva::Core::Commands::ChangeTimestepType ChangeTimestepType;
  typedef Minerva::Interfaces::IAnimationControl IAnimationControl;
  type->append ( new RadioButton ( new ChangeTimestepType ( IAnimationControl::TIMESTEP_TYPE_SECOND,  me ) ) );
  type->append ( new RadioButton ( new ChangeTimestepType ( IAnimationControl::TIMESTEP_TYPE_MINUTE,  me ) ) );
  type->append ( new RadioButton ( new ChangeTimestepType ( IAnimationControl::TIMESTEP_TYPE_HOUR,  me ) ) );
  type->append ( new RadioButton ( new ChangeTimestepType ( IAnimationControl::TIMESTEP_TYPE_DAY,   me ) ) );
  type->append ( new RadioButton ( new ChangeTimestepType ( IAnimationControl::TIMESTEP_TYPE_MONTH, me ) ) );
  type->append ( new RadioButton ( new ChangeTimestepType ( IAnimationControl::TIMESTEP_TYPE_YEAR,  me ) ) );
  animate->append ( type );

	// Time spans.
  this->_buildTimeSpanMenu();
  animate->append ( _timeSpanMenu.get() );

  // Points sub menu.
  MenuKit::Menu::RefPtr points ( new MenuKit::Menu ( "Points" ) );
  points->append ( new Button ( UC::genericCommand ( "Size * 2", UA::bind1<void> ( 2.0, UA::memberFunction<void> ( this, &MinervaDocument::_resizePoints ) ), UC::TrueFunctor() ) ) );
  points->append ( new Button ( UC::genericCommand ( "Size / 2", UA::bind1<void> ( 0.5, UA::memberFunction<void> ( this, &MinervaDocument::_resizePoints ) ), UC::TrueFunctor() ) ) );
  m->append ( points.get() );

  MenuKit::Menu::RefPtr split ( new MenuKit::Menu ( "Split" ) );
  split->append ( new Button ( UC::genericCommand ( "Increase Split", UA::memberFunction<void> ( this, &MinervaDocument::_increaseSplitDistance ), UC::TrueFunctor() ) ) );
  split->append ( new Button ( UC::genericCommand ( "Decrease Split", UA::memberFunction<void> ( this, &MinervaDocument::_decreaseSplitDistance ), UC::TrueFunctor() ) ) );

  m->append ( split.get() );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Legend",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showLegend ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowLegend ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Compass",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showCompass ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowCompass ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Position",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showLatLonText ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowLatLonText ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Eye Altitude",
                                                           UA::memberFunction<void> ( this, &MinervaDocument::showEyeAltitude ),
                                                           UA::memberFunction<bool> ( this, &MinervaDocument::isShowEyeAltitude ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Job Feedback",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showJobFeedback ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowJobFeedback ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Meta-Data",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showMetaString ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowMetaString ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Date Feedback",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showDateFeedback ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowDateFeedback ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Borders",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showBorders ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowBorders ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Show Skirts",
                                                            UA::memberFunction<void> ( this, &MinervaDocument::showSkirts ),
                                                            UA::memberFunction<bool> ( this, &MinervaDocument::isShowSkirts ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Allow Splitting",
                                                           UA::memberFunction<void> ( this, &MinervaDocument::allowSplit ),
                                                           UA::memberFunction<bool> ( this, &MinervaDocument::isAllowSplit ) ) ) );

  m->append ( new ToggleButton ( UC::genericToggleCommand ( "Keep Detail",
                                                           UA::memberFunction<void> ( this, &MinervaDocument::keepDetail ),
                                                           UA::memberFunction<bool> ( this, &MinervaDocument::isKeepDetail ) ) ) );

	this->_buildLayerMenu();
  menu.append ( _layersMenu.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build time span name.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  std::string buildName ( Minerva::Core::Data::TimeSpan *span )
  {
    if ( 0x0 != span )
    {
      return span->begin().toString() + "->" + span->end().toString();
    }
    return "";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build time span menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildTimeSpanMenu()
{
  USUL_TRACE_SCOPE;

  typedef MenuKit::RadioButton  RadioButton;

  // Namespace aliases to help shorten lines.
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;

  _timeSpanMenu->clear();

  if ( _global.valid() )
    _timeSpanMenu->append ( new RadioButton ( UC::genericCheckCommand ( Detail::buildName ( _global.get() ),
                                                                        UA::bind1<void> ( _global, UA::memberFunction<void> ( this, &MinervaDocument::currentTimeSpan ) ),
                                                                        UA::bind1<bool> ( _global, UA::memberFunction<bool> ( this, &MinervaDocument::isCurrentTimeSpan ) ) ) ) );

  for ( TimeSpans::iterator iter = _timeSpans.begin(); iter != _timeSpans.end(); ++iter )
  {
    _timeSpanMenu->append ( new RadioButton ( UC::genericCheckCommand ( Detail::buildName ( (*iter).get() ),
                                                                        UA::bind1<void> ( *iter, UA::memberFunction<void> ( this, &MinervaDocument::currentTimeSpan ) ),
                                                                        UA::bind1<bool> ( *iter, UA::memberFunction<bool> ( this, &MinervaDocument::isCurrentTimeSpan ) ) ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time span.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::currentTimeSpan ( TimeSpan::RefPtr span )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _current = span;

  if ( _current.valid() )
    _lastDate = _current->begin();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current time span.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isCurrentTimeSpan ( TimeSpan::RefPtr span ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return span.get() ==  _current.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find first and last date.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_findFirstLastDate()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  Minerva::Core::Visitors::FindMinMaxDates::RefPtr findMinMax ( new Minerva::Core::Visitors::FindMinMaxDates );
  this->accept ( *findMinMax );

  TimeSpan::RefPtr global ( new TimeSpan );

  global->begin ( findMinMax->first() );
  global->end ( findMinMax->last() );

  // Set the current to the new global.
  if ( _current == _global )
    _current = global;

  _lastDate = findMinMax->first();

  _global = global;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increase split distance.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_increaseSplitDistance()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->splitDistance ( body->splitDistance() * 1.1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrease split distance.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_decreaseSplitDistance()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->splitDistance ( body->splitDistance() / 1.1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert matrix to heading, pitch, roll.
//  See:
//  http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/
//  http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
//
//  Implementation adapted from ossimPlanet (www.ossim.org).
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void matrixToHpr( osg::Vec3d& hpr, const osg::Matrixd& rotation )
  {
    // Initialize answer.
    hpr.set ( 0.0, 0.0, 0.0 );

    osg::Matrixd mat;

    // I'm not really sure what the following code inside {} is about.
    {
      osg::Vec3d col1 ( rotation( 0, 0 ), rotation( 0, 1 ), rotation( 0, 2 ) );
      const double s ( col1.length() );

      if ( s <= 0.00001 )
      {
        hpr.set(0.0f, 0.0f, 0.0f);
        return;
      }

      const double oneOverS ( 1.0f / s );
      for( int i = 0; i < 3; i++ )
        for( int j = 0; j < 3; j++ )
          mat(i, j) = rotation(i, j) * oneOverS;
    }

    // Set the pitch
    hpr[1] = ::asin ( Usul::Math::clamp ( mat ( 1, 2 ), -1.0, 1.0 ) );

    double cp ( ::cos( hpr[1] ) );

    // See if the cosine of the pitch is close to zero.
    // This is for singularities at the north and south poles.
    if ( cp > -0.00001 && cp < 0.00001 )
    {
      const double cr ( Usul::Math::clamp (  mat( 0, 1 ), -1.0, 1.0 ) );
      const double sr ( Usul::Math::clamp ( -mat( 2, 1 ), -1.0, 1.0 ) );

      hpr[0] = 0.0f;
      hpr[2] = ::atan2 ( sr, cr );
    }
    else
    {
      // Remove the cosine of pitch from these elements.
      cp = 1.0 / cp ;
      double sr ( Usul::Math::clamp ( ( -mat( 0, 2 ) * cp ), -1.0, 1.0 ) );
      double cr ( Usul::Math::clamp ( (  mat( 2, 2 ) * cp ), -1.0, 1.0 ) );
      double sh ( Usul::Math::clamp ( ( -mat( 1, 0 ) * cp ), -1.0, 1.0 ) );
      double ch ( Usul::Math::clamp ( (  mat( 1, 1 ) * cp ), -1.0, 1.0 ) );

      if ( ( sh == 0.0f && ch == 0.0f ) || ( sr == 0.0f && cr == 0.0f ) )
      {
        cr = Usul::Math::clamp (  mat( 0, 1 ), -1.0, 1.0 );
        sr = Usul::Math::clamp ( -mat( 2, 1 ), -1.0, 1.0 );

        hpr[0] = 0.0f;
      }
      else
      {
        hpr[0] = ::atan2 ( sh, ch );
      }

      hpr[2] = ::atan2 ( sr, cr );
    }

    // Convert to degress.
    hpr[0] *= Usul::Math::RAD_TO_DEG;
    hpr[1] *= Usul::Math::RAD_TO_DEG;
    hpr[2] *= Usul::Math::RAD_TO_DEG;

    // Sign flip.
    hpr[0] *= -1.0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildScene ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Clear what we have.
  _root->removeChild( 0, _root->getNumChildren() );

  bool viewportChanged ( false );
  bool buildLegend ( false );

  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  if ( vp.valid () )
  {
    const unsigned int width  ( static_cast < unsigned int > ( vp->width() ) );
    const unsigned int height ( static_cast < unsigned int > ( vp->height() ) );

    if ( width > 0 && height > 0 && ( width != _width || height != _height ) )
    {
      _width = width;
      _height = height;
      viewportChanged = true;

      // Set the build legend flag.
      buildLegend = true;
    }
  }

  if ( viewportChanged )
  {
    _camera->setViewport ( 0, 0, _width, _height );
    _camera->setProjectionMatrixAsOrtho ( 0, _width, 0, _height, -100.0, 100.0 );

    // Set the build legend flag.
    buildLegend = true;
  }

  if ( buildLegend || this->dirty() )
  {
    _camera->removeChild ( 0, _camera->getNumChildren() );

    // Build the legend.
    this->_buildLegend( caller );
  }

  _root->addChild ( _camera.get() );

  if ( _callback.valid() )
  {
    osg::Vec3d hpr ( _callback->_hpr );
    _hud.hpr (  hpr[0], hpr[1], hpr[2] );

    Usul::Math::Vec3d eye ( _callback->_eyePosition );
    _hud.eyeAltitude( eye[2] );
  }

  Usul::Jobs::Manager *manager ( this->_getJobManager() );
  const unsigned int queued ( ( 0x0 == manager ) ? 0 : manager->numJobsQueued() );

  Usul::Jobs::Manager::Strings names;
  if ( 0x0 != manager )
    manager->executingNames ( names );

  _hud.requests ( queued );
  _hud.running ( names );

  if ( vp.valid() )
    _hud.updateScene( static_cast<unsigned int> ( vp->width() ), static_cast<unsigned int> ( vp->height() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the planet.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_makePlanet()
{
  // Only make it once.
  {
    Guard guard ( this );
    if ( false == _bodies.empty() )
      return;
  }

  // Make the earth.
  Body::RefPtr body ( Minerva::Core::Functions::makeEarth ( this->_getJobManager() ) );

  {
    Guard guard ( this );
    _bodies.push_back ( body.get() );
  }

  this->activeBody ( body.get() );

  // Set the log.
  this->_setLog();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLegend( bool b )
{
  Guard guard ( this );
  if( b != _showLegend )
  {
    _showLegend = b;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowLegend() const
{
  Guard guard ( this );
  return _showLegend;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend width percentage.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendWidth ( float p )
{
  Guard guard ( this );
  _legendWidth = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend width percentage.
//
///////////////////////////////////////////////////////////////////////////////

float MinervaDocument::legendWidth() const
{
  Guard guard ( this );
  return _legendWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend padding.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendPadding( const osg::Vec2& padding )
{
  Guard guard ( this );
  _legendPadding = padding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend padding.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec2& MinervaDocument::legendPadding () const
{
  Guard guard ( this );
  return _legendPadding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Height of each item showing in the legend.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendHeightPerItem( unsigned int height )
{
  Guard guard ( this );
  _legendHeightPerItem = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Height of each item showing in the legend.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int MinervaDocument::legendHeightPerItem() const
{
  Guard guard ( this );
  return _legendHeightPerItem;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::legendPosition ( LegendPosition position )
{
  Guard guard ( this );
  _legendPosition = position;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::LegendPosition MinervaDocument::legendPosition () const
{
  Guard guard ( this );
  return _legendPosition;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the legend.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildLegend ( Usul::Interfaces::IUnknown *caller )
{
  // Always clear.
  _legend->clear();

  if( this->isShowLegend() )
  {
    // Set the legend size.
    const unsigned int legendWidth  ( static_cast < unsigned int > ( _width * _legendWidth ) );
    //const unsigned int legendHeight ( static_cast < unsigned int > ( _height - ( _legendPadding.y() * 2 ) ) );

    // Add items to the legend.
    Minerva::Core::Visitors::BuildLegend::RefPtr visitor ( new Minerva::Core::Visitors::BuildLegend ( _legend.get() ) );
    this->accept( *visitor );
    
    const unsigned int height ( 35 * _legend->numRows() );
    _legend->size ( legendWidth, height );
    _legend->maximiumSize ( legendWidth, height );

    // Must be called after rows are added to the legend.
    this->_setLegendPosition( legendWidth );
  }

  // Build the legend.
  _camera->addChild( _legend->buildScene() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend position.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_setLegendPosition ( unsigned int legendWidth )
{
  // Translate legend to correct location.
  unsigned int x ( 0 );
  unsigned int y ( 0 );

  unsigned legendHeight ( _legend->maximiumSize()[1] );

  switch ( _legendPosition )
  {
    case LEGEND_TOP_LEFT:
    {
      x = static_cast < unsigned int > ( _legendPadding.x() );
      y = static_cast < unsigned int > ( _height - legendHeight -_legendPadding.y() );
    }
      break;
    case LEGEND_TOP_RIGHT:
    {
      x = static_cast < unsigned int > ( _width - ( legendWidth + _legendPadding.x() ) );
      y = static_cast < unsigned int > ( _height - legendHeight - _legendPadding.y() );
    }
      break;
    case LEGEND_BOTTOM_RIGHT:
    {
      x = static_cast < unsigned int > ( _width - ( legendWidth + _legendPadding.x() ) );
      y = static_cast < unsigned int > ( _legendPadding.y() );
    }
      break;
    case LEGEND_BOTTOM_LEFT:
    {
      x = static_cast < unsigned int > ( _legendPadding.x() );
      y = static_cast < unsigned int > ( _legendPadding.y() );
    }
      break;
  }

  _legend->position( x, y );
  _legend->growDirection( OsgTools::Widgets::Legend::GROW_DIRECTION_UP );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the scene dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::dirty() const
{
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::dirty( bool b )
{
  Guard guard ( this );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a layer menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildLayerSubMenu ( MenuKit::Menu& menu, Usul::Interfaces::ITreeNode* node )
{
  // Return now if layer is null.
  if ( 0x0 == node )
    return;

  {
    MenuKit::Menu::RefPtr layerMenu ( new MenuKit::Menu ( node->getTreeNodeName() ) );

    namespace UA = Usul::Adaptors;
    namespace UC = Usul::Commands;

    const unsigned int number ( node->getNumChildNodes() );
    for ( unsigned int i = 0; i < number; ++i  )
    {
      Usul::Interfaces::ITreeNode::QueryPtr child ( node->getChildNode ( i ).get() );
      if ( child.valid() )
        this->_buildLayerSubMenu ( *layerMenu, child.get() );

      Usul::Interfaces::IMenuAdd::QueryPtr ma ( node->getChildNode ( i ).get() );
      if ( ma.valid() )
        ma->menuAdd ( *layerMenu );
    }

    if ( number > 0 )
      layerMenu->addSeparator();

    layerMenu->append ( new MenuKit::ToggleButton ( new Minerva::Core::Commands::ToggleShown ( node, "This Layer" ) ) );
    layerMenu->append ( new MenuKit::Button ( UC::genericCommand ( "Goto This Layer", UA::bind1<void> ( Usul::Interfaces::IUnknown::QueryPtr ( node ).get(), UA::memberFunction<void> ( this, &MinervaDocument::lookAtLayer ) ), UC::TrueFunctor() ) ) );

    Minerva::Interfaces::IRefreshData::QueryPtr rd ( node );
    if ( rd.valid() )
    {
      layerMenu->append ( new MenuKit::Button ( UC::genericCommand ( "Refresh This Layer", UA::memberFunction<void> ( rd, &Minerva::Interfaces::IRefreshData::refreshData ), UC::TrueFunctor() ) ) );
    }

    menu.append ( layerMenu.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the layer menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_buildLayerMenu()
{
  Guard guard ( this );
  _layersMenu->clear();

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      this->_buildLayerSubMenu ( *_layersMenu, Usul::Interfaces::ITreeNode::QueryPtr ( body ) );
  }

  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;

  _layersMenu->addSeparator();
  _layersMenu->append ( new MenuKit::Button ( UC::genericCommand ( "Refresh Sub-Menu", UA::memberFunction<void> ( this, &MinervaDocument::_buildLayerMenu ), UC::TrueFunctor() ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize all points.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_resizePoints ( double factor )
{
  Minerva::Core::Visitors::ResizePoints::RefPtr visitor ( new Minerva::Core::Visitors::ResizePoints ( factor ) );
  this->accept ( *visitor );
  //this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the observer of the intersection (IIntersectListener).
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller )
{
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef std::vector<NodePtr> Nodes;
  typedef Minerva::Interfaces::IIntersectNotify IIntersectNotify;
  typedef IIntersectNotify::Closest Closest;
  typedef IIntersectNotify::Path Path;
  typedef IIntersectNotify::Point Point;
  typedef IIntersectNotify::PointAndDistance PointAndDistance;

  Body::RefPtr body ( this->activeBody() );
  if ( false == body.valid() )
    return;

  // Call this every time.
  body->intersectionGraphicClear();
  _hud.metaString ( "" );

  // Get the point in lon-lat-elev.
  osg::Vec3 world ( hit.getWorldIntersectPoint() );
  Usul::Math::Vec3d point ( world[0], world[1], world[2] );
  Usul::Math::Vec3d lonLatPoint;
  body->convertFromPlanet ( point, lonLatPoint );

  // Set the heads-up display.
  {
    Guard guard ( this );
    _hud.position ( lonLatPoint[1], lonLatPoint[0], lonLatPoint[2] );
  }

  // If we're in pick mode...
  typedef Usul::Interfaces::IViewMode IViewMode;
  IViewMode::QueryPtr mode ( caller );
  if ( false == mode.valid() )
    return;
  if ( false == mode->isViewModeCurrent ( IViewMode::PICK ) )
    return;

  // Initialize up here.
  Closest closest ( Path(), PointAndDistance ( Point(), std::numeric_limits<double>::max() ) );

  // Find the deepest tile.
  Nodes path ( hit.nodePath.rbegin(), hit.nodePath.rend() );
  IUnknown::RefPtr unknown ( caller );
  for ( Nodes::iterator i = path.begin(); i != path.end(); ++i )
  {
    typedef Minerva::Core::TileEngine::Tile Tile;
    Tile::RefPtr tile ( dynamic_cast < Tile * > ( i->get() ) );
    if ( true == tile.valid() )
    {
      IIntersectNotify::QueryPtr notify ( tile ); 
      if ( true == notify.valid() )
      {
        // Notify the tile about the intersection. It can optionally 
        // return the closest point on any geometry.
        notify->intersectNotify ( point[0], point[1], point[2], 
                                  lonLatPoint[0], lonLatPoint[1], lonLatPoint[2], 
                                  Usul::Interfaces::IUnknown::QueryPtr ( this ),
                                  Usul::Interfaces::IUnknown::QueryPtr ( body ),
                                  unknown, closest );
        break;
      }
    }
  }

  // Get the path of unknown pointers.
  Path unknowns ( closest.first );

  // Do we have a closest point?
  const bool haveClosestPoint ( ( false == unknowns.empty() ) && ( 3 == closest.second.first.size() ) );
  if ( false == haveClosestPoint )
    return;

  // Draw a line from under the mouse to the intersected point.
  body->intersectionGraphicSet ( lonLatPoint[0], lonLatPoint[1], lonLatPoint[2], closest.second.first.at(0), closest.second.first.at(1), closest.second.first.at(2) );

  // Show the description string?
  if ( false == _hud.showMetaString() )
    return;

  // Accumulate the description strings for the data objects.
  std::ostringstream ds;
  for ( Path::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    Minerva::Interfaces::IDataObject::QueryPtr ptr ( *i );
    Minerva::Core::Data::DataObject::RefPtr dataObject ( ( true == ptr.valid() ) ? ptr->dataObject() : 0x0 );
    if ( true == dataObject.valid() )
    {
      const std::string s ( dataObject->description() );
      if ( false == s.empty() )
      {
        ds << s << '\n';
      }
    }
  }

  std::string description ( ds.str() );
  if ( description.size() > 1 )
  {
    // Lose last newline.
    description.erase ( description.size() - 1 );

    // Display the info.
    _hud.metaString ( description );
    std::cout << Usul::Strings::format ( "\n\n", description, '\n' ) << std::flush;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show compass state.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showCompass( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _hud.showCompass ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show compass state.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowCompass() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showCompass();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use skirts state.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowSkirts() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  return ( body.valid() ? body->useSkirts() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the skirts on and off.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showSkirts ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  if ( body.valid() )
    body->useSkirts ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use borders state.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowBorders() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  return ( body.valid() ? body->useBorders() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the borders on and off.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showBorders ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Body::RefPtr body ( this->activeBody() );
  if ( body.valid() )
    body->useBorders ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the active body.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::activeBody ( Body* body )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _activeBody = body;

  if ( _activeBody.valid () )
  {
    _callback = new Callback;
    _callback->_body = _activeBody;
    _activeBody->scene()->setCullCallback ( _callback.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active body.
//
///////////////////////////////////////////////////////////////////////////////

MinervaDocument::Body* MinervaDocument::activeBody() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _activeBody.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the job manager.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager * MinervaDocument::_getJobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Only make it once.
  if ( 0x0 == _manager )
  {
    typedef Usul::Registry::Database Reg;
    namespace Sections = Usul::Registry::Sections;

    const std::string type ( Reg::instance().convertToTag ( this->typeName() ) );
    Usul::Registry::Node &node ( Reg::instance()[Sections::DOCUMENT_SETTINGS][type]["job_manager_thread_pool_size"] );
    const unsigned int poolSize ( node.get<unsigned int> ( 5, true ) );

    const std::string name ( Usul::Strings::format ( "Minerva ", this ) );
    std::cout << Usul::Strings::format ( name, " thread pool size = ", poolSize, '\n' ) << std::flush;

    _manager = new Usul::Jobs::Manager ( name, poolSize, true );
    _manager->logSet ( Usul::Jobs::Manager::instance().logGet() );
    _manager->addJobFinishedListener ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }

  return _manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get data from cull visitor.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::Callback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
  if ( osg::NodeVisitor::CULL_VISITOR ==  nv->getVisitorType() )
  {
    osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>( nv );
    if( cullVisitor )
    {
      // Set the eye position.
      _eye = cullVisitor->getEyePoint();

      if ( 0x0 != _body )
      {
        // Convert the eye to lat,lon, height.
        Usul::Math::Vec3d point ( _eye[0], _eye[1], _eye[2] );
        _body->convertFromPlanet ( point, _eyePosition );

        // Get the model view matrix from the cull visitor.
        osg::ref_ptr<osg::RefMatrix> m ( cullVisitor->getModelViewMatrix() );

        // Get the inverse of the view matrix.
        osg::Matrixd viewMatrix ( 0x0 != m.get() ? osg::Matrixd::inverse ( *m ) : osg::Matrixd() );

        // Get the matrix to point north at the eye position.
        osg::Matrixd localLsr ( _body->planetRotationMatrix( _eyePosition[1], _eyePosition[0], _eyePosition[2], 0.0 ) );

        osg::Matrixd invert;
        if ( invert.invert ( localLsr ) )
        {
          osg::Matrixd matrix ( viewMatrix * invert );
          Detail::matrixToHpr( _hpr, matrix );
        }

        // Set the eye altitude to above sea level.
        const double elevationAtEye ( _body->elevationAtLatLong ( _eyePosition[1], _eyePosition[0] ) );
        _eyePosition[2] -= elevationAtEye;
      }
    }
  }

  this->traverse( node, nv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int MinervaDocument::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return _bodies.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode::RefPtr MinervaDocument::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Interfaces::ITreeNode::RefPtr ( Usul::Interfaces::ITreeNode::QueryPtr ( _bodies.at ( which ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::setTreeNodeName ( const std::string & )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string MinervaDocument::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->fileName();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the freeze split flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isAllowSplit() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _allowSplit;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the freeze split flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::allowSplit( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _allowSplit = b;

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->allowSplit ( _allowSplit );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the keep detail flag.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isKeepDetail() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _keepDetail;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the keep detail flag.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::keepDetail( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _keepDetail = b;

  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
      body->keepDetail ( _keepDetail );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::jobFinished ( Usul::Jobs::Job *job )
{
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when mouse event occurs.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::mouseEventNotify ( osgGA::GUIEventAdapter& ea, Usul::Interfaces::IUnknown * caller )
{
  if ( osgGA::GUIEventAdapter::PUSH == ea.getEventType() )
  {
    typedef Usul::Interfaces::IViewMode IViewMode;
    IViewMode::QueryPtr vm ( caller );
    IViewMode::ViewMode mode ( vm.valid() ? vm->getViewMode() : IViewMode::NAVIGATION );

    // Stop any animation if we aren't in seek mode. Look for animation interface.
    if ( IViewMode::SEEK != mode )
    {
      Usul::Interfaces::IAnimateMatrices::QueryPtr animator ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IAnimateMatrices::IID ) );
      if ( true == animator.valid() )
      {
        // Send an empty vector of matrices to stop the animation.
        animator->animateMatrices ( Usul::Interfaces::IAnimateMatrices::Matrices(), 0, false, Usul::Interfaces::IUnknown::RefPtr ( 0x0 ) );
      }
    }
  
    // See if it's the left button.
    const bool left ( Usul::Bits::has ( ea.getButton(), osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) );
    if ( left )
    {
      if ( this->_intersectBalloon ( ea, caller ) )
        return;
    
      this->_intersectScene ( ea, caller );
    }
  }
  else if ( osgGA::GUIEventAdapter::MOVE == ea.getEventType() )
  {
    if ( true == this->isShowLatLonText() )
    {
      this->requestRedraw();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect with osg::Node*.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool intersectWithScene ( osg::Node* node, osgUtil::LineSegmentIntersector* intersector, osgUtil::LineSegmentIntersector::Intersection& hit )
  {
    if ( 0x0 == node )
      return false;
    
    // Make the visitor.
    osg::ref_ptr<osgUtil::IntersectionVisitor> visitor ( new osgUtil::IntersectionVisitor ( intersector ) );
    node->accept ( *visitor );
    
    osgUtil::LineSegmentIntersector::Intersections& intersections ( intersector->getIntersections() );
    if ( false == intersections.empty() )
    {
      hit = *intersections.begin();
      return true;
    }
    
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect with balloon.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::_intersectBalloon ( osgGA::GUIEventAdapter& ea, Usul::Interfaces::IUnknown * )
{
  Guard guard ( this->mutex() );
  
  if ( false == _balloon.valid() || false == _camera.valid() )
    return false;
  
  // Make a line segment intersector.
  typedef osgUtil::LineSegmentIntersector Intersector;
  osg::ref_ptr<Intersector> intersector ( new Intersector ( Intersector::WINDOW, ea.getX(), ea.getY() ) );
  
  // Intersect.
  Intersector::Intersection hit;
  if ( Helper::intersectWithScene ( _camera.get(), intersector.get(), hit ) )
  { 
    // See if the object is a Widget.
    if ( OsgTools::Widgets::Item* item = dynamic_cast<OsgTools::Widgets::Item*> ( hit.nodePath.back() ) )
    {
      item->onClick ( ea.getX(), ea.getY() );
      return true;
    }
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the data object from the node path.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  MinervaDocument::ObjectID findObjectID ( const osg::NodePath& path )
  {
    osg::ref_ptr < Minerva::Core::Data::UserData > userdata ( 0x0 );

    // See if there is user data.
    for( osg::NodePath::const_reverse_iterator iter = path.rbegin(); iter != path.rend(); ++iter )
    {
      osg::ref_ptr<osg::Node> node ( *iter );
      if ( true == node.valid() )
      {
        osg::ref_ptr<osg::Referenced> data ( node->getUserData() );
        if ( true == data.valid() )
        {
          osg::ref_ptr<Minerva::Core::Data::UserData> ud ( dynamic_cast < Minerva::Core::Data::UserData * > ( data.get() ) );
          if ( true == ud.valid() )
          {
            userdata = ud;
            break;
          }
        }
      }
    }
    
    if ( userdata.valid() )
    {
      return userdata->objectID();
    }

    return "";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find object.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr MinervaDocument::_findObject ( const ObjectID& objectID )
{
  Minerva::Core::Visitors::FindObject::RefPtr visitor ( new Minerva::Core::Visitors::FindObject ( objectID ) );
  this->accept ( *visitor );
  return visitor->object();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect with scene.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::_intersectScene ( osgGA::GUIEventAdapter& ea, Usul::Interfaces::IUnknown * caller )
{
  // Query for the interface.
  Usul::Interfaces::ISceneIntersect::QueryPtr si ( caller );
  if ( false == si.valid() )
  {
    this->_clearBalloon();
    return false;
  }

#if 0
  typedef osgUtil::PolytopeIntersector::Intersections Intersections;
  const unsigned int dimensions ( osgUtil::PolytopeIntersector::DimOne | osgUtil::PolytopeIntersector::DimZero );
  const double size ( 0.5 );
  Intersections intersections;
  if ( true == si->intersect ( ea.getX(), ea.getY(), intersections, size, size, dimensions ) )
#else
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;
  Intersections intersections;
  if ( true == si->intersect ( ea.getX(), ea.getY(), intersections ) )
#endif
  {
    // List of objects that were intersected.
    std::vector<Minerva::Core::Data::DataObject::RefPtr> objects;
    
    for ( Intersections::const_iterator iter = intersections.begin(); iter != intersections.end(); ++iter )
    {
      // Find the id for the object we intersected.
      ObjectID objectID ( Helper::findObjectID ( iter->nodePath ) );
      
      if ( false == objectID.empty() )
      {
        // Find the unknown
        Usul::Interfaces::IUnknown::RefPtr unknown ( this->_findObject ( objectID ) );
        
        // See if it's a data object.
        Minerva::Interfaces::IDataObject::QueryPtr iDataObject ( unknown );
        if ( iDataObject.valid() )
        {
          // This should be true, but check to make sure the data object pointer is valid.
          Minerva::Core::Data::DataObject::RefPtr dataObject ( iDataObject->dataObject() );
          if( dataObject.valid() )
            objects.push_back ( dataObject );
        }
      }
    }

    // Get the first data object and display it.
    if( false == objects.empty() && true == objects.front().valid() )
    {
      Minerva::Core::Data::DataObject::RefPtr dataObject ( objects.front() );
      return this->_displayInformationBalloon ( *dataObject );
    }
    else
    {
      this->_clearBalloon();
    }
  }
  
  // No intersection, remove the balloon.
  else
  {
    this->_clearBalloon();
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the balloon
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::_displayInformationBalloon ( Minerva::Core::Data::DataObject& dataObject )
{
  // Remove what we have.
  this->_clearBalloon();

  osg::ref_ptr<osg::Camera> camera ( Usul::Threads::Safe::get ( this->mutex(), _camera, true ) );
  OsgTools::Widgets::Item::RefPtr item ( dataObject.clicked() );

  if ( camera.valid() && item.valid() )
  {
    osg::ref_ptr<osg::Node> balloon ( item->buildScene() );
    if ( true == balloon.valid() )
    {
      camera->addChild ( balloon.get() );
    }

    Usul::Threads::Safe::set ( this->mutex(), balloon, _balloon );

    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the balloon
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_clearBalloon()
{
  Guard guard ( this->mutex() );
  if ( _camera.valid() && _balloon.valid() )
    _camera->removeChild ( _balloon.get() );
  
  _balloon = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the lat lon text shown?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowLatLonText() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showPointerPosition();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle showing of lat lon text.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showLatLonText ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showPointerPosition( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the job feedback text shown?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowJobFeedback() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showJobFeedback();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle showing of job feedback text.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showJobFeedback ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _hud.showJobFeedback( b );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Is the date feedback text shown?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowDateFeedback() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _hud.showDateFeedback();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle showing of date feedback text.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showDateFeedback ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _hud.showDateFeedback( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset all the logs.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::_setLog()
{
  // Get the log.
  LogPtr logFile ( Usul::Threads::Safe::get ( this->mutex(), _log ) );

  // Loop through the bodies.
  for ( Bodies::iterator iter = _bodies.begin(); iter != _bodies.end(); ++iter )
  {
    Body::RefPtr body ( *iter );
    if ( body.valid() )
    {
      // Set the log.
      body->logSet ( logFile );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the job to close this document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Job *MinervaDocument::closeJob()
{
  USUL_TRACE_SCOPE;
  return new MinervaDocument::CloseJob ( MinervaDocument::RefPtr ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get showing of eye altitude feedback state.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowEyeAltitude() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _hud.showEyeAltitude();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set showing of eye altitude feedback state.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showEyeAltitude ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _hud.showEyeAltitude ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get showing of meta-string feedback state.
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::isShowMetaString() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _hud.showMetaString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set showing of meta-string feedback state.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::showMetaString ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _hud.showMetaString ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we busy?
//
///////////////////////////////////////////////////////////////////////////////

bool MinervaDocument::busyStateGet() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  MinervaDocument *me ( const_cast < MinervaDocument * > ( this ) );
  if ( 0x0 == me )
    return false;

  const unsigned int queued    ( me->_getJobManager()->numJobsQueued() );
  const unsigned int executing ( me->_getJobManager()->numJobsExecuting() );
  const bool idle ( ( 0 == queued ) && ( 0 == executing ) );

  return ( false == idle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the context menu.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::contextMenuAdd ( MenuKit::Menu& menu, const Usul::Math::Vec2ui& position, Usul::Interfaces::IUnknown* caller )
{
  if ( Usul::System::ClipBoard::isClipBoardSupported() )
  {
    // Query for the interface.
    Usul::Interfaces::ISceneIntersect::QueryPtr si ( caller );
    
    typedef osgUtil::LineSegmentIntersector::Intersections Intersections;
    Intersections intersections;
    if ( si.valid() && si->intersect ( position[0], position[1], intersections ) )
    {
      Body::RefPtr body ( this->activeBody() );
      
      // Set the intersection point.
      if ( body.valid() )
      {
        osgUtil::LineSegmentIntersector::Intersection hit ( *intersections.begin() );
        osg::Vec3 world ( hit.getWorldIntersectPoint() );
        
        Usul::Math::Vec3d point ( world[0], world[1], world[2] );
        Usul::Math::Vec3d lonLatPoint;
        body->convertFromPlanet( point, lonLatPoint );
        
        // Namespace aliases to help shorten lines.
        namespace UA = Usul::Adaptors;
        namespace UC = Usul::Commands;

        const std::string lon ( Usul::Convert::Type<double,std::string>::convert ( lonLatPoint[0] ) );
        const std::string lat ( Usul::Convert::Type<double,std::string>::convert ( lonLatPoint[1] ) );
        const std::string text ( Usul::Strings::format ( lat, ", ", lon ) );
        menu.append ( new MenuKit::Button ( UC::genericCommand ( "Copy lat/lon to clipboard", UA::bind1<void> ( text, Usul::System::ClipBoard::addToClipboard ), UC::TrueFunctor() ) ) );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the animation stopped.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::animationStopped()
{
  Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );
  if ( false == vm.valid() )
    return;

  Usul::Interfaces::ITrackball::QueryPtr tb ( vm );
  if ( false == tb.valid() )
    return;

  Body::RefPtr body ( Usul::Threads::Safe::get ( this->mutex(), _activeBody, true ) );
  if ( false == body.valid() )
    return;

  // The translate portion of the matrix is where the eye position will be.
  const osg::Matrixd mat ( osg::Matrixd::inverse ( vm->getViewMatrix() ) );
  Usul::Math::Vec3d point ( mat ( 3, 0 ), mat ( 3, 1 ), mat ( 3, 2 ) );

  // Find the intersection point from the eye to the center of the body.
  // This will become the new center of the trackball.
  Usul::Math::Vec3d center;
  if ( false == body->intersectWithTiles ( point, Usul::Math::Vec3d ( 0.0, 0.0, 0.0 ), center ) )
    return;

  // Get the distance between the eye and the center.
  const double d ( center.distance ( point ) );

  // Get the rotation and set the trackball.
  osg::Quat rot; mat.get ( rot );
  const osg::Vec3d c ( Usul::Convert::Type<Usul::Math::Vec3d,osg::Vec3d>::convert ( center ) );
  tb->setTrackball ( c, d, rot, true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the projection matrix for the active view.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd MinervaDocument::getProjectionMatrix() const
{
  Usul::Interfaces::IProjectionMatrix::QueryPtr view ( Usul::Documents::Manager::instance().activeView() );
  return view.valid() ? view->getProjectionMatrix() : osg::Matrixd();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek to intersection point.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::handleSeek ( const Usul::Math::Vec3d& intersectionPoint )
{
  Body::RefPtr body ( this->activeBody() );

  if ( body.valid() )
  {
    Usul::Math::Vec3d lonLatPoint;
    body->convertFromPlanet ( intersectionPoint, lonLatPoint );

    Minerva::Core::Data::Camera::RefPtr start ( _navigator->copyCameraState() );

    Minerva::Core::Data::Camera::RefPtr destination ( new Minerva::Core::Data::Camera );
    destination->longitude ( lonLatPoint[0] );
    destination->latitude ( lonLatPoint[1] );
    destination->altitude ( Usul::Math::maximum ( start->altitude() * 0.50, 1000.0 ) );
    destination->heading ( 0.0 );
    destination->tilt ( 0.0 );
    destination->roll ( 0.0 );

    typedef Usul::Interfaces::IAnimateMatrices::Matrices Matrices;
    Matrices matrices;
    Minerva::Core::Data::Camera::generateAnimatePath ( start, destination, 0.50, 50, body->landModel(), matrices );
    
    // Look for animation interface.
    Usul::Interfaces::IAnimateMatrices::QueryPtr animator ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IAnimateMatrices::IID ) );
    if ( true == animator.valid() )
    {
      const unsigned int milliSeconds ( Usul::Registry::Database::instance()[Usul::Registry::Sections::PATH_ANIMATION]["curve"]["milliseconds"].get<unsigned int> ( 15, true ) );
      animator->animateMatrices ( matrices, milliSeconds, false, 0x0 );
    }

    // Otherwise, just slam in the last one.
    else
    {
      // Set the new matrix.
      Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );
      if ( vm.valid() )
      {
        const Matrices::value_type mat ( matrices.back() );
        vm->setViewMatrix ( Usul::Convert::Type<Usul::Math::Matrix44d,osg::Matrixd>::convert ( mat ) );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::ref()
{
  USUL_TRACE_SCOPE;
  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaDocument::unref ( bool allowDeletion )
{
  USUL_TRACE_SCOPE;
  BaseClass::unref ( allowDeletion );
}
