
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/Flash/FlashModel/FlashDocument.h"
#include "Experimental/Flash/FlashModel/TypeWrapper.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/Color.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IRenderInfoOSG.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Clock.h"
#include "Usul/Threads/Safe.h"

#include "OsgTools/State/StateSet.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "MenuKit/Menu.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osg/BlendFunc"
#include "osg/Camera"
#include "osg/GLObjects"
#include "osg/LOD"
#include "osg/Texture1D"

#include "osgText/Text"

#include "osgUtil/GLObjectsVisitor"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FlashDocument, FlashDocument::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( FlashDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::FlashDocument() : 
  BaseClass ( "Flash Document" ),
  _filenames(),
  _scale ( 1.0 / 1e+20f ),
  _currentTimestep ( 0 ),
  _dataSet ( "temp" ),
  _minimum ( 0.0 ),
  _maximum ( 0.0 ),
  _root ( new osg::Group ),
  _dirty ( true ),
  _drawBBox ( false ),
  _drawPoints ( false ),
  _drawVolume ( true ),
  _currentTransferFunction ( 0 ),
  _transferFunctions(),
  _timesteps(),
  _program ( Volume::createProgram() ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_addMember ( "filenames", _filenames );
  this->_addMember ( "current_time_step", _currentTimestep );
  this->_addMember ( "data_set", _dataSet );
  this->_addMember ( "minimum", _minimum );
  this->_addMember ( "maximum", _maximum );
  this->_addMember ( "transfer_functions", _transferFunctions );
  
  this->_buildDefaultTransferFunctions();
  
  // Create a blend function.
  osg::ref_ptr< osg::BlendFunc > blendFunc ( new osg::BlendFunc );
  blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  _root->getOrCreateStateSet()->setAttributeAndModes( blendFunc.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
  _root->getOrCreateStateSet()->setRenderBinDetails ( 1, "DepthSortedBin" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::~FlashDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FlashDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast<Usul::Interfaces::IUpdateListener*> ( this );
  case Usul::Interfaces::ITimeVaryingData::IID:
    return static_cast<Usul::Interfaces::ITimeVaryingData*> ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast<Usul::Interfaces::IMenuAdd*> ( this );
  case Flash::IFlashDocument::IID:
    return static_cast<Flash::IFlashDocument*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "flash" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "flash" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::write ( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  
  if( "flash" == ext )
  {
    Serialize::XML::serialize ( *this, filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::read ( const std::string &filename, Unknown *caller, Unknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  
  if( "flash" == ext )
  {
    // Deserialize the xml tree.
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( filename );
    this->deserialize ( *document );
  }
  else
  {
    // Add the filename.
    _filenames.push_back ( filename );
    
    // Notify any observers.
    this->_notifyModifiedObservers();
    
    // We are modified.
    this->modified ( true );
    
    // We are dirty.
    this->dirty ( true );
    
    // Request a redraw.
    this->requestRedraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _filenames.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Flash xml (*.flash)", "*.flash" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Flash xml (*.flash)", "*.flash" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "Flash HDF5 file (*)", "*" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *FlashDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  //Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &FlashDocument::_buildScene ), caller, "3960901428" );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::_buildScene ( Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  
  const double minimum ( Usul::Threads::Safe::get ( this->mutex(), _minimum ) );
  const double maximum ( Usul::Threads::Safe::get ( this->mutex(), _maximum ) );
  const bool validMinMax ( 0.0 != minimum && 0.0 != maximum );
  
  
  Guard guard ( this->mutex() );
  
  Usul::Interfaces::IRenderInfoOSG::QueryPtr ri ( caller );
  osg::RenderInfo info ( ri.valid() ? ri->getRenderInfo() : osg::RenderInfo() );
  
  osg::ref_ptr<osgUtil::GLObjectsVisitor> visitor ( new osgUtil::GLObjectsVisitor ( osgUtil::GLObjectsVisitor::RELEASE_STATE_ATTRIBUTES | osgUtil::GLObjectsVisitor::RELEASE_DISPLAY_LISTS ) );
  visitor->setRenderInfo ( info );
  _root->accept ( *visitor );
  
  // Remove what we have.
  _root->removeChild ( 0, _root->getNumChildren() );
  
  osg::flushAllDeletedGLObjects ( info.getContextID() );
  
  double maxtime ( std::numeric_limits<double>::max() );
  osg::Texture::flushDeletedTextureObjects ( info.getContextID(), Usul::System::Clock::milliseconds(), maxtime );
  
  // Make sure we are in range...
  if ( _currentTimestep < _filenames.size() )
  {
    // Get the timestep.  Do not cache for now.
    Timestep::RefPtr timestep ( this->loadTimestep ( _currentTimestep, false ) );

    // Add the child to the scene.
    if ( timestep.valid() )
    {
      // Use this min and max.
      const double useMin ( validMinMax ? minimum : timestep->minimum() );
      const double useMax ( validMinMax ? maximum : timestep->maximum() );
      
      // Colors.
      std::vector<osg::Vec4> colors ( 6 );
      colors[0] = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );
      colors[1] = osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 );
      colors[2] = osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 );
      colors[3] = osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 );
      colors[4] = osg::Vec4 ( 1.0, 0.0, 1.0, 1.0 );
      colors[5] = osg::Vec4 ( 0.0, 1.0, 1.0, 1.0 );

      // Typedefs.
      TransferFunction1D::RefPtr tf ( new TransferFunction1D ( _transferFunctions.at ( _currentTransferFunction ) ) );

      const unsigned int numNodes ( timestep->numNodes() );
      
      // Make bounding boxes.
      for ( unsigned int num = 0; num < numNodes; ++num )
      {
        osg::BoundingBox bb ( timestep->boundingBox ( num ) );
        const bool isLeaf ( timestep->isLeaf ( num ) );
        const int level ( timestep->level ( num ) );
        
        if ( isLeaf /*&& 5 == level*/ )
        //if ( isLeaf && 5 == level )
        {      
          bb._min =  bb._min * _scale;
          bb._max =  bb._max * _scale;
          
          // Groups for the lod.
          osg::ref_ptr<osg::Group> low ( new osg::Group );
          osg::ref_ptr<osg::Group> high ( new osg::Group );

          // Add the bounding box.
          if ( _drawBBox )
          {
            osg::ref_ptr<osg::Node> node ( timestep->buildBoundingBox ( bb, colors.at ( level - 1 ) ) );
            low->addChild ( node.get() );
            high->addChild ( node.get() );
          }
          
          // Add the points.
          if ( _drawPoints )
          {
            osg::ref_ptr<osg::Node> node ( timestep->buildPoints ( bb, num ) );
            low->addChild ( node.get() );
            high->addChild ( node.get() );
          }
          
          if ( _drawVolume )
          {
            osg::ref_ptr<osg::Image> image ( timestep->buildVolume ( num, useMin, useMax ) );
            low->addChild  ( this->_buildVolume ( *timestep, image.get(), 8,  bb, tf.get() ) );
            high->addChild ( this->_buildVolume ( *timestep, image.get(), 64, bb, tf.get() ) );
          }
          
          // Make a lod.
          osg::ref_ptr<osg::LOD> lod ( new osg::LOD );
          lod->setCenter ( bb.center() );
          
          lod->addChild ( high.get(), 0, std::numeric_limits<float>::max() );
          lod->addChild ( low.get(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::min() );
          
          // Add the lod to the scene.
          _root->addChild ( lod.get() );
        }
      }
      
      _root->addChild ( this->_buildLegend ( useMin, useMax, tf.get(), caller ) );
    }
  }
  
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* FlashDocument::_buildVolume ( const Timestep& timestep, osg::Image* image, unsigned int numPlanes, const osg::BoundingBox& bb, TransferFunction::RefPtr tf )
{
  osg::ref_ptr<Volume> volumeNode ( new Volume ( Usul::Threads::Safe::get ( this->mutex(), _program.get() ) ) );
  
  // Set the volume nodes bounding box.
  volumeNode->boundingBox ( bb );
  
  // Set the transfer function.
  volumeNode->transferFunction ( tf.get() );
  
#if USE_RAY_CASTING
  volumeNode->samplingRate ( 1.0f / numPlanes ); 
#else
  volumeNode->numPlanes ( numPlanes );
#endif
  
  volumeNode->image ( image );
  
  volumeNode->getOrCreateStateSet()->setRenderBinDetails ( 1, "DepthSortedBin" );
  
  return volumeNode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a legend.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* FlashDocument::_buildLegend ( double minimum, double maximum, TransferFunction::RefPtr tf, Usul::Interfaces::IUnknown* caller ) const
{
  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  
  osg::ref_ptr<osg::Image> image ( tf.valid() ? tf->image() : 0x0 );
  
  if ( vp.valid() && image.valid() )
  {
    // Add transfer function key.
    osg::ref_ptr<osg::Camera> camera ( new osg::Camera );
    camera->setRenderOrder ( osg::Camera::POST_RENDER );
    camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
    camera->setClearMask( GL_DEPTH_BUFFER_BIT );
    camera->setViewMatrix( osg::Matrix::identity() );
    camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
    camera->setCullingMode ( osg::CullSettings::NO_CULLING );
    
    camera->setViewport ( 0, 0, vp->width(), vp->height() );
    camera->setProjectionMatrixAsOrtho ( 0, vp->width(), 0, vp->height(), -1000.0, 1000.0 );
    
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    
    const unsigned int xPadding ( 4 );
    const unsigned int yPadding ( 4 );
    const unsigned int width ( vp->width() - xPadding );
    const unsigned int height ( 20 );
    const unsigned int yStart ( vp->height() - yPadding - height );
    
    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
    vertices->push_back ( osg::Vec3 ( xPadding,   yStart,          -1.0 ) );
    vertices->push_back ( osg::Vec3 ( width,      yStart,          -1.0 ) );
    vertices->push_back ( osg::Vec3 ( width,      yStart + height, -1.0 ) );
    vertices->push_back ( osg::Vec3 ( xPadding,   yStart + height, -1.0 ) );
    
    osg::ref_ptr<osg::Vec2Array> tCoords ( new osg::Vec2Array );
    tCoords->push_back ( osg::Vec2 ( 0.0, 0.0 ) );
    tCoords->push_back ( osg::Vec2 ( 1.0, 0.0 ) );
    tCoords->push_back ( osg::Vec2 ( 1.0, 1.0 ) );
    tCoords->push_back ( osg::Vec2 ( 0.0, 1.0 ) );
    
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    geometry->setVertexArray ( vertices.get() );
    geometry->setTexCoordArray ( 0, tCoords.get() );
    
    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );
    
    osg::ref_ptr<osg::Image> copy ( static_cast<osg::Image*> ( image->clone ( osg::CopyOp::DEEP_COPY_ALL ) ) );
    for ( int i = 0; i < copy->s(); ++i )
    {
      copy->data ( i )[3] = 255;
    }
    
    osg::ref_ptr<osg::Texture1D> texture ( new osg::Texture1D );
    texture->setImage( copy.get () );
    
    texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
    texture->setWrap  ( osg::Texture::WRAP_S, osg::Texture::CLAMP );
    texture->setInternalFormatMode ( osg::Texture::USE_IMAGE_DATA_FORMAT );
    
    geometry->getOrCreateStateSet()->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    
    geode->addDrawable ( geometry.get() );
    
    {
      osg::ref_ptr<osgText::Text> text ( new osgText::Text );
      text->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
      text->setCharacterSize( 25.0 );
      text->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
      text->setPosition ( osg::Vec3 ( xPadding, yStart - yPadding - 15.0, 0.0 ) );
      text->setBackdropColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
      text->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
      text->setText ( Usul::Strings::format ( minimum ) );
      geode->addDrawable ( text.get() );
    }

    {
      osg::ref_ptr<osgText::Text> text ( new osgText::Text );
      text->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
      text->setCharacterSize( 25.0 );
      text->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
      text->setPosition ( osg::Vec3 ( xPadding + width - 90, yStart - yPadding - 15.0, 0.0 ) );
      text->setBackdropColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
      text->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
      text->setText ( Usul::Strings::format ( maximum ) );
      geode->addDrawable ( text.get() );
    }
    
    camera->addChild ( geode.get() );
    
    return camera.release();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  Usul::Interfaces::ITextMatrix::QueryPtr tm ( caller );
  if ( tm.valid() )
  {
    const unsigned int timestep  ( Usul::Threads::Safe::get ( this->mutex(), _currentTimestep ) );
    const unsigned int total ( Usul::Threads::Safe::get ( this->mutex(), _filenames.size() ) );
    tm->setText ( 15, 15, Usul::Strings::format ( "Timestep ", timestep + 1, " of ",  total ), osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  }
  
  // Buid the scene if we need to.
  if ( this->dirty () )
  {  
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &FlashDocument::_buildScene ), caller, "3279281359" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  _dataMemberMap.deserialize ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::dirty ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::dirty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the default transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::_buildDefaultTransferFunctions()
{
  const unsigned int size ( 256 );  
  Colors hsv       ( size );

  for ( unsigned int i = 0; i < size; ++i )
  {
    float u ( static_cast < float > ( i ) / ( size - 1 ) );
    const unsigned char alpha ( 50 );
    
    float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
    Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( u * 300.0f ), 1.0f, 1.0f );
    hsv.at ( i ) [ 0 ] = static_cast < unsigned char > ( r * 255 );
    hsv.at ( i ) [ 1 ] = static_cast < unsigned char > ( g * 255 );
    hsv.at ( i ) [ 2 ] = static_cast < unsigned char > ( b * 255 );
    hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( u < 0.5 ? 0 : u * alpha );
    
    if ( ( g > 0.75 || b > 0.5 ) && u >= 0.5)
      hsv.at( i )[3] = 1;
  }
  
  _transferFunctions.push_back ( hsv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time step.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::setCurrentTimeStep ( unsigned int current )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _currentTimestep = current;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current time step.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FlashDocument::getCurrentTimeStep () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentTimestep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of timesteps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FlashDocument::getNumberOfTimeSteps () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _filenames.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the i'th timestep.
//
///////////////////////////////////////////////////////////////////////////////

Timestep::RefPtr FlashDocument::loadTimestep ( unsigned int i, bool cache )
{
  USUL_TRACE_SCOPE;
  
  // Get the filename for the timestep.
  const std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filenames.at ( i ) ) );
  
  // Make the timestep.
  Timestep::RefPtr timestep ( new Timestep ( filename ) );
  timestep->init();
  timestep->loadData ( this->dataSet() );
  
  // Add the timestep.
  if ( true == cache )
  {
    Guard guard ( this->mutex() );
    _timesteps[i] = timestep;
  }
  
  return timestep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the i'th timestep loaded?
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::hasTimestep ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Timesteps::const_iterator iter ( _timesteps.find ( i ) );
  return ( iter != _timesteps.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw bounding box flag.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::drawBBox ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _drawBBox )
  {
    _drawBBox = b;
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw bounding box flag.
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::isDrawBBox() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _drawBBox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw points flag.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::drawPoints ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _drawPoints )
  {
    _drawPoints = b;
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw points flag.
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::isDrawPoints() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _drawPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw volume flag.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::drawVolume ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _drawVolume )
  {
    _drawVolume = b;
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw volume flag.
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::isDrawVolume() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _drawVolume;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu (IMenuAdd).
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::ToggleButton ToggleButton;
  
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;
  
  MenuKit::Menu::RefPtr view ( menu.find ( "&View", true ) );
  
  if ( view.valid() )
  {
    // Add a separator if the menu already has items.
    if ( view->items().size() > 0 )
      view->addSeparator();
    
    view->append ( new ToggleButton ( 
                                     UC::genericToggleCommand ( "Draw Bounding Boxes", 
                                                             UA::memberFunction<void> ( this, &FlashDocument::drawBBox ), 
                                                             UA::memberFunction<bool> ( this, &FlashDocument::isDrawBBox ) ) ) );
    
    view->append ( new ToggleButton ( 
                                     UC::genericToggleCommand ( "Draw Points", 
                                                                UA::memberFunction<void> ( this, &FlashDocument::drawPoints ), 
                                                                UA::memberFunction<bool> ( this, &FlashDocument::isDrawPoints ) ) ) );
    
    view->append ( new ToggleButton ( 
                                     UC::genericToggleCommand ( "Draw Volume", 
                                                                UA::memberFunction<void> ( this, &FlashDocument::drawVolume ), 
                                                                UA::memberFunction<bool> ( this, &FlashDocument::isDrawVolume ) ) ) );
    
    MenuKit::Menu::RefPtr tf ( new MenuKit::Menu ( "Transfer Functions" ) );
    view->append ( tf.get() );
    typedef TransferFunctions::const_iterator ConstIterator;
    Guard guard ( this );
    for ( ConstIterator iter = _transferFunctions.begin(); iter != _transferFunctions.end(); ++iter )
    {
      const long num ( std::distance<ConstIterator> ( _transferFunctions.begin(), iter ) );
      tf->append ( new MenuKit::RadioButton ( 
                                    Usul::Commands::genericCheckCommand ( Usul::Strings::format ( num ), 
                                                                         Usul::Adaptors::bind1<void> ( num, Usul::Adaptors::memberFunction<void> ( this, &FlashDocument::transferFunction ) ), 
                                                                         Usul::Adaptors::bind1<bool> ( num, Usul::Adaptors::memberFunction<bool> ( this, &FlashDocument::isTransferFunction ) ) ) ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dataset name.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::dataSet ( const std::string& s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( s != _dataSet )
  {
    _dataSet = s;
    this->clearCache();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dataset name.
//
///////////////////////////////////////////////////////////////////////////////

std::string FlashDocument::dataSet() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dataSet;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return whether or not to sort the files after the user has selected them 
//  with the file dialog, but before they are sent to the document's read().
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::_sortFilesBeforeInserting() const
{
  USUL_TRACE_SCOPE;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the cache.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::clearCache()
{
  USUL_TRACE_SCOPE;
  _timesteps.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::transferFunction ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _currentTransferFunction = i;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the current  transfer function?
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::isTransferFunction ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return i == _currentTransferFunction;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimum value to show.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::minimumSet ( double m )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( m != _minimum )
  {
    _minimum = m;
    this->dirty ( true );
    this->requestRedraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the minimum value to show.
//
///////////////////////////////////////////////////////////////////////////////

double FlashDocument::minimumGet() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _minimum;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum value to show.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::maximumSet ( double m )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( m != _maximum )
  {
    _maximum = m;
    this->dirty ( true );
    this->requestRedraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum value to show.
//
///////////////////////////////////////////////////////////////////////////////

double FlashDocument::maximumGet() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _maximum;
}
