
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h>
#endif

#include "Minerva/Core/Layers/RasterLayerOssim.h"

#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "ossim/imaging/ossimImageRenderer.h"
#include "ossim/imaging/ossimImageHandler.h"
#include "ossim/imaging/ossimImageHandlerRegistry.h"
#include "ossim/imaging/ossimOverviewBuilderFactoryRegistry.h"
#include "ossim/imaging/ossimOverviewBuilderFactory.h"
#include "ossim/imaging/ossimOverviewBuilderBase.h"
#include "ossim/projection/ossimEquDistCylProjection.h"
#include "ossim/projection/ossimImageViewTransform.h"
#include "ossim/projection/ossimImageViewProjectionTransform.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"

using namespace Minerva::Core::Layers;

///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < RasterLayerOssim > > _creator0 ( "JPEG (*.jpg)", "*.jpg" );
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < RasterLayerOssim > > _creator1 ( "TIFF (*.tiff *.tif)", "*.tiff,*.tif" );
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < RasterLayerOssim > > _creator2 ( "PNG (*.png)", "*.png" );
}

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerOssim );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( RasterLayerOssim, RasterLayerOssim::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerOssim::RasterLayerOssim() : 
  BaseClass(),
  _filename (),
  _handler ( 0x0 ),
  _renderer ( 0x0 ),
  _viewInterface ( 0x0 ),
  _projection ( 0x0 )
{
  USUL_TRACE_SCOPE;

  this->_registerMembers();

  // So we can automatically build pyramids.
  ossimOverviewBuilderFactoryRegistry::instance()->registerFactory ( ossimOverviewBuilderFactory::instance(), true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerOssim::RasterLayerOssim ( const RasterLayerOssim& rhs ) : BaseClass ( rhs ),
_filename (),
_handler ( 0x0 ),
_renderer ( 0x0 ),
_viewInterface ( 0x0 ),
_projection ( 0x0 )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
  
  this->_open ( rhs._filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerOssim::~RasterLayerOssim()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &RasterLayerOssim::_destroy ), "3809846167" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_registerMembers()
{
  USUL_TRACE_SCOPE;
  this->_addMember( "filename", _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayerOssim::clone() const
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerOssim ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_destroy()
{
  USUL_TRACE_SCOPE;

  Usul::Pointers::unreference ( _handler ); _handler = 0x0;
  Usul::Pointers::unreference ( _projection ); _projection = 0x0;
  Usul::Pointers::unreference ( _renderer ); _renderer = 0x0;
  _viewInterface = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface id..
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayerOssim::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IRead::IID:
      return static_cast< Usul::Interfaces::IRead* > ( this );
    default:
      return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::read (  const std::string& filename, Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown* progress )
{
  this->_open ( filename );
  
  if ( true == this->name().empty() )
  {
    this->name ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the image.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_open ( const std::string& filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Set the filename
  _filename = filename;

  _handler = ossimImageHandlerRegistry::instance()->open ( ossimFilename ( filename.c_str() ) );
  Usul::Pointers::reference ( _handler );

  _projection = new ossimEquDistCylProjection;
  Usul::Pointers::reference ( _projection );

  _renderer = new ossimImageRenderer;
  Usul::Pointers::reference ( _renderer );

  _viewInterface = _renderer;

  _renderer->setView ( _projection, false );
  _renderer->getResampler()->setFilterType ( "nearest neighbor" );

  _renderer->connectMyInputTo ( 0, _handler );

  // I'm not really sure what this is for.
  // I think some projections might be sensitive to elevation.
  ossimImageViewTransform* ivt = _renderer->getImageViewTransform();
  if ( ivt )
  {
    ossimImageViewProjectionTransform* projectionTransform = PTR_CAST ( ossimImageViewProjectionTransform, ivt );
    if ( projectionTransform )
    {
       ossimMapProjection* mapProj = PTR_CAST ( ossimMapProjection, projectionTransform->getImageProjection() );
       if ( mapProj )
       {
          mapProj->setElevationLookupFlag ( false );
          _projection->setElevationLookupFlag ( false );
       }
    }
  }

  // Update our extents.
  this->_updateExtents();

  // Make the pyramids.
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &RasterLayerOssim::_buildImagePyramids ), this->filename(), "7649047000" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an osg::Image.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Image* makeImage ( unsigned int width, unsigned int height, ossimScalarType type )
  {   
    osg::ref_ptr<osg::Image> result ( new osg::Image );
    
    GLenum dataType ( GL_UNSIGNED_BYTE );
    
    switch ( type )
    {
      case OSSIM_UINT8:
        dataType = GL_UNSIGNED_BYTE;
        break;
      case OSSIM_SINT8:
        dataType = GL_BYTE;
        break;
      case OSSIM_UINT16:
        dataType = GL_UNSIGNED_SHORT;
        break;
      case OSSIM_SINT16:
        dataType = GL_SHORT;
        break;
      case OSSIM_UINT32:
        dataType = GL_UNSIGNED_INT;
        break;
      case OSSIM_SINT32:
        dataType = GL_INT;
        break;
      case OSSIM_FLOAT32:
        dataType = GL_FLOAT;
        break;
      case OSSIM_FLOAT64:
        dataType = GL_DOUBLE;
        break;
      default:
        throw std::invalid_argument ( "Error 3452396252: invalid image passed to makeImage" ); // We don't handle this data type.
    }
    
    result->allocateImage ( width, height, 1, GL_RGBA, dataType );
    
    return result.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerOssim::ImagePtr RasterLayerOssim::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Let the base class go first.
  {
    ImagePtr answer ( BaseClass::texture ( extents, width, height, level, job, caller ) );
    if ( true == answer.valid() )
      return answer;
  }

  // Now guard.
  Guard guard ( this );

  // Create the answer.
  ImagePtr result ( 0x0 );

  // Check state.
  if ( ( 0x0 == _projection ) || ( 0x0 == _viewInterface ) || ( 0x0 == _renderer ) )
    return result;

  ossimIrect requestRect ( 0, 0, width - 1, height - 1 );

  const double deltaX ( extents.maximum()[0] - extents.minimum()[0] );
  const double deltaY ( extents.maximum()[1] - extents.minimum()[1] );
   
  const double deltaLat ( deltaY / height );
  const double deltaLon ( deltaX / width );

  _projection->setDecimalDegreesPerPixel ( ossimDpt ( deltaLon, deltaLat ) );

  _projection->setUlGpt ( ossimGpt ( extents.maximum()[1], extents.minimum()[0] ) ); // Max lat and min lon.
  _viewInterface->setView ( _projection, false );
  ossimRefPtr<ossimImageData> data ( _renderer->getTile ( requestRect ) );

  // Check state and create image.
  if ( ( true == data.valid() ) && 
       ( 0x0 != data->getBuf() ) && 
       ( OSSIM_EMPTY != data->getDataObjectStatus() ) )
  {
    //result = this->_createBlankImage ( width, height );
    result = Detail::makeImage ( width, height, data->getScalarType() );
    this->_convert ( *data, *result );

    // Save the image to the cache.
    BaseClass::_writeImageToCache ( extents, width, height, level, result );
  }

  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template<class PixelType> void convert ( const ossimImageData& data, osg::Image& image )
  {
    USUL_TRACE_SCOPE_STATIC;
    
    const unsigned int width ( data.getWidth() );
    const unsigned int height ( data.getHeight() );
    const unsigned int size ( width * height );
    
    // Check sizes.
    if ( ( static_cast<int> ( width ) != image.s() ) || ( static_cast<int> ( height ) != image.t() ) )
      throw std::invalid_argument ( "Error 3651784054: image sizes are not equal" );
    
    PixelType *buffer ( reinterpret_cast < PixelType * > ( image.data() ) );
    
    bool greyscale ( data.getNumberOfBands() < 3 );
    
    const PixelType* b1 ( static_cast < const PixelType* > ( data.getBuf( 0 ) ) );
    const PixelType* b2 ( static_cast < const PixelType* > ( data.getBuf( ( greyscale ? 0 : 1 ) ) ) );
    const PixelType* b3 ( static_cast < const PixelType* > ( data.getBuf( ( greyscale ? 0 : 2 ) ) ) ); 
    PixelType np1 ( static_cast < PixelType > ( data.getNullPix( 0 ) ) );
    PixelType np2 ( static_cast < PixelType > ( data.getNullPix( ( greyscale ? 0 : 1 ) ) ) );
    PixelType np3 ( static_cast < PixelType > ( data.getNullPix( ( greyscale ? 0 : 2 ) ) ) );
    
    // Copy the pixels into the osg image.
    for ( unsigned int i = 0; i < size; ++i )
    {
      // If the pixel is null, make transparent.
      if ( ( *b1 == np1 ) && ( *b2 == np2 ) && ( *b3 == np3 ) )
      {
        buffer[3] = 0;
      }
      else
      {
				buffer[3] = std::numeric_limits<PixelType>::max();
      }
      buffer[0] = *b1;
      buffer[1] = *b2;
      buffer[2] = *b3;
      buffer +=4;
      ++b1;
      ++b2;
      ++b3;
    }
    
    image.flipVertical();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_convert ( const ossimImageData& data, osg::Image& image )
{
  USUL_TRACE_SCOPE;

  switch ( data.getScalarType() )
  {
  case OSSIM_UINT8:
    Detail::convert<unsigned char> ( data, image );
    break;
  case OSSIM_SINT8:
    Detail::convert<char> ( data, image );
    break;
  case OSSIM_UINT16:
    Detail::convert<short> ( data, image );
    break;
  case OSSIM_SINT16:
    Detail::convert<unsigned short> ( data, image );
    break;
  case OSSIM_UINT32:
    Detail::convert<unsigned int> ( data, image );
    break;
  case OSSIM_SINT32:
    Detail::convert<int> ( data, image );
    break;
  case OSSIM_FLOAT32:
    Detail::convert<float> ( data, image );
    break;
  case OSSIM_FLOAT64:
    Detail::convert<double> ( data, image );
    break;
  default:
    throw std::invalid_argument ( "Error 3651784054: invalid image passed to convert" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_updateExtents()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _handler )
  {
    ossimKeywordlist kwl;
    _handler->getImageGeometry( kwl );
    
    ossimRefPtr<ossimProjection> projection ( ossimProjectionFactoryRegistry::instance()->createProjection( kwl ) );

    ossimIrect rect ( _handler->getBoundingRect() );

    if ( projection.valid () )
    {
      ossimGpt ulGpt;
      ossimGpt urGpt;
      ossimGpt lrGpt;
      ossimGpt llGpt;

      projection->lineSampleToWorld( rect.ul(), ulGpt );
      projection->lineSampleToWorld( rect.ur(), urGpt );
      projection->lineSampleToWorld( rect.lr(), lrGpt );
      projection->lineSampleToWorld( rect.ll(), llGpt );

      Extents extents ( Extents::Vertex ( llGpt.lon, llGpt.lat ), Extents::Vertex ( urGpt.lon, urGpt.lat ) );
      this->extents ( extents );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  _dataMemberMap.deserialize ( node );

  // Open ourselfs.
  this->_open ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerOssim::filename() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerOssim::_cacheDirectory() const
{
  USUL_TRACE_SCOPE;

  const std::size_t hashValue ( BaseClass::_hashString ( Usul::File::fullPath ( this->filename() ) ) );
  const std::string dir ( BaseClass::_buildCacheDir ( this->cacheDirectory(), "file_system", hashValue ) );
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension for the cached files.
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterLayerOssim::_cacheFileExtension() const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::File::extension ( this->filename() ) );
  return ext;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the image pyramids if they do not exist.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerOssim::_buildImagePyramids ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;

  // Yes, guard this whole function, including the check for existing files.
  // We don't want two threads writing the pyramids.
  Guard guard ( this );

  // The source file has to exist.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return;

  // Only build pyramids once.
  const std::string base ( Usul::Strings::format ( Usul::File::directory ( file, true ), Usul::File::base ( file ) ) );
  const std::string ovr ( base + ".ovr" );
  const std::string omd ( base + ".omd" );
  if ( true == Usul::Predicates::FileExists::test ( ovr ) )
    return;

  // Need an input handler.
  if ( 0x0 == _handler )
    return;

  // Remove these files if we fail.
  Usul::Scope::RemoveFile remove1 ( ovr, false );
  Usul::Scope::RemoveFile remove2 ( omd, false );

  // Always remove these files.
  Usul::Scope::RemoveFile remove3 ( ovr + ".tmp", false );
  Usul::Scope::RemoveFile remove4 ( omd + ".tmp", false );

  // Make the builder.
  ossimString overviewType ( "ossim_tiff_box" );
  ossimRefPtr<ossimOverviewBuilderBase> builder ( ossimOverviewBuilderFactoryRegistry::instance()->createBuilder ( overviewType ) );
  if ( false == builder.valid() )
    return;

  // Set properties.
  builder->setOutputFile ( ossimFilename ( ovr.c_str() ) );
  builder->setInputSource ( _handler, false );

  std::cout << Usul::Strings::format ( "Building overviews for: ", file ) << std::endl;

  // Make the pyramids.
  if ( false == builder->execute() )
  {
    std::cout << Usul::Strings::format ( "Warning 2517484520: Failed to build overviews for: ", file, ", performance will be slow" ) << std::endl;
    return;
  }

  std::cout << Usul::Strings::format ( "Done building overviews for: ", file ) << std::endl;

  // It worked so do not remove these files.
  remove1.remove ( false );
  remove2.remove ( false );
}
