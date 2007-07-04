
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Volume models.
//
///////////////////////////////////////////////////////////////////////////////

#include "VolumeDocument.h"
#include "VolumeCallback.h"

#include "OsgTools/Draggers/Translate1.h"

#include "OsgTools/Images/Image3d.h"
#include "OsgTools/Drawables/VoxelGeometry.h"
#include "OsgTools/Volume/Texture3DVolume.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Policies/Update.h"
#include "Usul/Functions/Color.h"
#include "Usul/Math/Vector3.h"

#include "Usul/Interfaces/IGetOptions.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Texture2D"
#include "osg/Texture3D"
#include "osg/TexEnv"
#include "osg/Billboard"
#include "osg/TexGenNode"
#include "osg/TexEnvCombine"
#include "osg/AlphaFunc"
#include "osg/Material"
#include "osg/ClipNode"
#include "osg/ShapeDrawable"
#include "osg/BlendFunc"
#include "osg/LOD"
#include "osg/TexMat"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include <limits>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VolumeDocument, VolumeDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::VolumeDocument() : BaseClass ( "Volume Document" ),
  _imageList(),
  _filenames()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::~VolumeDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VolumeDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IImageList::IID:
    return static_cast < Usul::Interfaces::IImageList* > ( this );
  case Usul::Interfaces::IInitNewDocument::IID:
    return static_cast < Usul::Interfaces::IInitNewDocument* > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canInsert ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || ext == "jpg" || ext == "tiff" || ext == "tif" || ext == "gif" || ext == "png" || ext == "bmp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::read ( const std::string &name, Unknown *caller )
{
  // Read the image.
  ImagePtr image ( osgDB::readImageFile ( name ) );
  if ( !image.valid() )
    throw std::runtime_error ( "Error 1350090608: Could not load image file: " + name );

  // Get the dimensions.
  const int width  ( image->s() );
  const int height ( image->t() );

#if 1
  // See if we need to add alpha...
  switch ( image->getPixelFormat() )
  {
    case GL_LUMINANCE:
    {
      ImagePtr newImage ( new osg::Image );

      newImage->allocateImage ( width, height, 1, GL_LUMINANCE_ALPHA, image->getDataType() );

      for ( int t = 0; t < height; ++t )
      {
        unsigned char* dest = newImage->data ( 0, t, 0 );
        unsigned char* source =  image->data ( 0, t, 0 );

        for ( int i = 0; i < width; ++i )
        {
          *dest++ = *source++;
          *dest++ = 255;
        }
      }

      image = new osg::Image ( *newImage, osg::CopyOp::DEEP_COPY_ALL );
    }
    break;
  
    case GL_RGB:
    {
      ImagePtr newImage ( new osg::Image );
      newImage->allocateImage ( width, height, 1, GL_RGBA, image->getDataType() );

      for ( int t = 0; t < height; ++t )
      {
        unsigned char* dest = newImage->data ( 0, t, 0 );
        unsigned char* source =  image->data ( 0, t, 0 );

        for ( int i = 0; i < width; ++i )
        {
            *dest++ = *source++;
            *dest++ = *source++;
            *dest++ = *source++;
            *dest++ = 255;
        }
      }

      image = new osg::Image ( *newImage, osg::CopyOp::DEEP_COPY_ALL );
    }
    break;
  }
#endif
  // Set the image name and push it into our list.
  image->setFileName ( name );
  _imageList.push_back ( image.get() );

  _filenames.push_back ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::write ( const std::string &filename, Unknown *caller ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "All Images (*.jpeg *.jpg *.tiff *.tif *.png *.gif *.bmp)", "*.jpeg;*.jpg;*.tiff;*.tif;*.png;*.gif;*.bmp" ) );
  filters.push_back ( Filter ( "JPEG (*.jpeg *.jpg)", "*.jpeg;*.jpg" ) );
  filters.push_back ( Filter ( "TIFF (*.tiff *.tif)", "*.tiff;*.tif" ) );
  filters.push_back ( Filter ( "PNG  (*.png)",        "*.png"        ) );
  filters.push_back ( Filter ( "GIF  (*.gif)",        "*.gif"        ) );
  filters.push_back ( Filter ( "BMP  (*.bmp)",        "*.bmp"        ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersOpen() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersSave() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *VolumeDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  if ( _imageList.empty() )
    return 0x0;

  // Make a copy
  BaseClass::Options copy ( opt );

  bool draw2d     ( copy["TextureType"] == "2DTexture" );
  bool drawVoxels ( copy["TextureType"] == "Voxels"    );
  
  osg::ref_ptr< osg::Node > root ( 0x0 );

  if ( draw2d )
    root = this->_buildScene2DTexture ( opt, caller );
  else if ( drawVoxels )
    root = this->_buildSceneVoxels ( opt, caller );
  else
    root = this->_buildScene3DTexture ( opt, caller );

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene using 2D textures.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VolumeDocument::_buildScene2DTexture ( const BaseClass::Options& options, Unknown* caller )
{
  osg::ref_ptr<osg::Group> root ( new osg::Group );
 
  // How far apart will the planes be
  const double zInc ( 1.0 / (double) _imageList.size() );

  double z ( -0.5 );
  Usul::Policies::TimeBased elapsed ( 1000 );
  VolumeCallback::Ptr callback ( new VolumeCallback );

  this->setStatusBar ( "Building scene..." );

  // Loop through our images
  for( ImageList::iterator i = _imageList.begin(); i != _imageList.end(); ++i )
  {
	  // Get the dimensions.
	  const unsigned int width  ( (*i)->s() );
	  const unsigned int height ( (*i)->t() );

	  double aspect ( ( double ) width / height );

	  // Intial points for the plane
	  osg::Vec3 top_left     ( -0.5 * aspect, 0.5, 0.0 );
	  osg::Vec3 bottom_left  ( -0.5 * aspect, -0.5, 0.0 );
	  osg::Vec3 bottom_right ( 0.5 * aspect, -0.5, 0.0 );
	  osg::Vec3 top_right    ( 0.5 * aspect, 0.5, 0.0 ); 

    // Make the geometry
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
    
    // Make the vertices
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array(4) );
    
    // Set the values
    vertices->at( 0 ) = top_left;
    vertices->at( 1 ) = bottom_left;
    vertices->at( 2 ) = bottom_right;
    vertices->at( 3 ) = top_right;
    
    // Make the texture coordinates
    osg::ref_ptr < osg::Vec2Array > texcoords ( new osg::Vec2Array(4) );

    // Set the values
    texcoords->at( 0 ).set( 0.0f, 1.0f );
    texcoords->at( 1 ).set( 0.0f, 0.0f );
    texcoords->at( 2 ).set( 1.0f, 0.0f );
    texcoords->at( 3 ).set( 1.0f, 1.0f );

    // Make the normals
    osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array(1) );
    
    // Set the value
    normals->at( 0 ).set( 0.0f, 0.0f, 1.0f );
    
    // Make the colors
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array(1) );
    
    // Set the value
    colors->at ( 0 ).set( 1.0f, 1.0f, 1.0f, 1.0f );
    
    // Set the arrays
    geometry->setVertexArray   ( vertices.get() );
    geometry->setTexCoordArray ( 0, texcoords.get() );
    geometry->setNormalArray   ( normals.get() );
    geometry->setColorArray    ( colors.get() );

    // Set the bindings
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
    geometry->setColorBinding  ( osg::Geometry::BIND_OVERALL );

    // Add the primitive set
    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, 4 ) );
    
    // Make the geode
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    
    // Add the geometry
    geode->addDrawable( geometry.get() );

    // Make a matrix transform to place in right spot
    osg::Matrix m;
    m.makeTranslate( 0.0, 0.0, z );

    osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform ( m ) );

    mt->addChild( geode.get() );

    callback->addMT( mt.get() );

    // Add the geode to the root
    root->addChild( mt.get() );
    
    // Make the texture
    osg::ref_ptr < osg::Texture2D > texture ( new osg::Texture2D );
    texture->setDataVariance(osg::Object::DYNAMIC);
    texture->setImage( i->get() );
    
    // Get or Create the state set
    osg::ref_ptr < osg::StateSet > stateset ( geometry->getOrCreateStateSet() );

    stateset->setMode( GL_LIGHTING, osg::StateAttribute::INHERIT );

    // Add the texture
    stateset->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON);

    stateset->setTextureAttributeAndModes(0,new osg::TexEnv(osg::TexEnv::MODULATE),osg::StateAttribute::ON);

    // Set needed states from transparency
    stateset->setMode( GL_BLEND, osg::StateAttribute::ON);
    //stateset->setAttributeAndModes(new osg::BlendFunc( osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE ), osg::StateAttribute::ON);
    stateset->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    // Increment z value
    z += zInc;

    // Show progress.
    this->setProgressBar ( true, _imageList.size() - std::distance ( i, _imageList.end() ), _imageList.size() );
  }

  if( !_imageList.empty() )
  {
    //Get the last node
    osg::ref_ptr < osg::Node > last ( root->getChild( root->getNumChildren() - 1 ) );

    typedef OsgTools::Draggers::Translate1 Dragger;

    //Make a dragger
    osg::ref_ptr< Dragger > dragger ( new Dragger( Dragger::Z ) );
    dragger->geometry( Dragger::NORMAL, last.get() );
    dragger->geometry( Dragger::DRAGGING, last.get() );

    dragger->callback ( Dragger::MOVE, callback.get() );
    dragger->callback ( Dragger::FINISH, callback.get() );

    root->replaceChild( last.get(), dragger.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene using 3D Texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VolumeDocument::_buildScene3DTexture( const BaseClass::Options& options, Unknown* caller )
{
  this->setStatusBar ( "Building scene..." );

  // Create the 3d image
  ImagePtr image3d ( OsgTools::Images::image3d ( _imageList, true, 1000 ) );

  osg::ref_ptr < OsgTools::Volume::Texture3DVolume > volume ( new OsgTools::Volume::Texture3DVolume );
  volume->numPlanes ( 512 );
  volume->image ( image3d.get() );

  return volume.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the 3D Texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Texture3D* VolumeDocument::_make3DTexture()
{
  // Do we need to ensure the proper texture size?
  bool ensureProperTextureSize ( !osg::Texture::getExtensions( 0, true )->isNonPowerOfTwoTextureSupported ( GL_LINEAR ) );
  
  // Create the 3d image
  ImagePtr image3d ( OsgTools::Images::image3d ( _imageList, /*ensureProperTextureSize*/ true, 1000 ) );

  // Make the texture
  osg::ref_ptr< osg::Texture3D > texture3D ( new osg::Texture3D );

  // Set the attributes
  texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
  texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP_TO_BORDER );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP_TO_BORDER );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP_TO_BORDER );

  if ( image3d->getPixelFormat() == GL_ALPHA || image3d->getPixelFormat() == GL_LUMINANCE )
  {
      texture3D->setInternalFormatMode( osg::Texture3D::USE_USER_DEFINED_FORMAT );
      texture3D->setInternalFormat( GL_INTENSITY );
  }
  else
  {
      texture3D->setInternalFormatMode ( osg::Texture::USE_IMAGE_DATA_FORMAT );
  }

  // Set the 3d image as the texture's image
  texture3D->setImage ( image3d.get() );

  return texture3D.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this document
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::initNewDocument ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IGetOptions::QueryPtr getOptions ( this->delegate() );

  if( getOptions.valid() )
    getOptions->getOptions ( this->options() );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene by drawing the voxels
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VolumeDocument::_buildSceneVoxels ( const BaseClass::Options&, Unknown* caller )
{
  osg::ref_ptr < osg::Group > root ( new osg::Group );
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

  root->addChild ( geode.get() );
  geode->addDrawable ( geom.get() );

  unsigned int numVoxels ( 0 );

  // Reserve space. Assume all images are same size.
  const unsigned int space ( ( _imageList.empty() ) ? 0 : _imageList.front()->s() * _imageList.front()->t() );
  vertices->reserve ( space );
  colors->reserve ( space );

  // A single normal vector.
  normals->push_back ( osg::Vec3 ( 0, 0, 1 ) );

  this->setStatusBar ( "Building scene..." );

  for ( unsigned int i = 0; i < _imageList.size(); ++i )
  {
    osg::ref_ptr < osg::Image > image ( _imageList.at(i) );
    unsigned int width  ( static_cast < unsigned int > ( image->s() ) );
    unsigned int height ( static_cast < unsigned int > ( image->t() ) );
    const unsigned int numPixels ( width * height );
    numVoxels += numPixels;

    for ( unsigned int j = 0; j < width; ++j )
    {
      for ( unsigned int k = 0; k < height; ++k )
      {
        vertices->push_back ( osg::Vec3 ( j, k, i ) );
        const unsigned char *c = image->data ( j, k, 0 );
        switch ( image->getPixelFormat() )
        {
        case GL_LUMINANCE: // Greyscale
            colors->push_back ( osg::Vec4 ( c[0], c[0], c[0], 1.0f ) );
            break;

          case GL_LUMINANCE_ALPHA: // Greyscale + alpha
            colors->push_back ( osg::Vec4 ( c[0], c[0], c[0], c[1] ) );
            break;

          case GL_RGB: // Red, green, blue
            colors->push_back ( osg::Vec4 ( c[0], c[1], c[2], 1.0f ) );
            break;

          case GL_RGBA: // Red, green, blue, alpha
            colors->push_back ( osg::Vec4 ( c[0], c[1], c[2], c[3] ) );
            break;
        }
      }
    }

    // Show progress.
    this->setProgressBar ( true, i, _imageList.size() );
  }

  geom->setVertexArray ( vertices.get() );
  geom->setNormalArray ( normals.get() );
  geom->setColorArray ( colors.get() );
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::POINTS, 0, numVoxels ) );
  geom->setNormalBinding ( osg::Geometry::BIND_OVERALL );
  geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify this document of the message.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::notify ( unsigned short message )
{
  BaseClass::notify ( message );
}
