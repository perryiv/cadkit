
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

#if 1
      newImage->allocateImage ( width, height, 1, GL_LUMINANCE_ALPHA, image->getDataType() );
#else
      newImage->allocateImage ( width, height, 1, GL_RGBA, image->getDataType() );
#endif

      for ( int t = 0; t < height; ++t )
      {
        unsigned char* dest = newImage->data ( 0, t, 0 );
        unsigned char* source =  image->data ( 0, t, 0 );

        for ( int i = 0; i < width; ++i )
        {
          //*dest++ = *source;
          //*dest++ = *source;
          *dest++ = *source++;
          *dest++ = 255;
        }
      }

      //image->setImage( newImage->s(), newImage->t(), newImage->r(), newImage->getInternalTextureFormat(), newImage->getPixelFormat(), newImage->getDataType(), newImage->data(0,0,0), newImage->getAllocationMode() ); 
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
  osg::ref_ptr< osg::Group > root ( new osg::Group );
  osg::ref_ptr <osg::TexGenNode> texgen ( new osg::TexGenNode );

  root->addChild( texgen.get() );

  texgen->setTextureUnit( 0 );
  texgen->getTexGen()->setMode( osg::TexGen::EYE_LINEAR );
  texgen->getTexGen()->setPlane( osg::TexGen::S, osg::Vec4( 1.0f, 0.0f, 0.0f, 0.5f ) );
  texgen->getTexGen()->setPlane( osg::TexGen::T, osg::Vec4( 0.0f, 1.0f, 0.0f, 0.5f ) );
  texgen->getTexGen()->setPlane( osg::TexGen::R, osg::Vec4( 0.0f, 0.0f, 1.0f, 0.5f ) );

  this->setStatusBar ( "Building scene..." );

  // Make the 3D texture
  osg::ref_ptr< osg::Texture3D > texture3D ( this->_make3DTexture() );

  osg::ref_ptr< osg::LOD > lod ( new osg::LOD );

  // Get the dimensions.
  const unsigned int width  ( _imageList.front()->s() );
  const unsigned int height ( _imageList.front()->t() );

  //double aspect ( ( double ) width / height );

  // Make the high LOD.
  lod->addChild( this->_makeCube( 512, width, height ) );

  // Make the low LOD.
  lod->addChild( this->_makeCube( 128, width, height ) );

  // Get the first child.
  osg::Node *child = lod->getChild ( 0 );

  // Get the bounding sphere for the first child.
  const osg::BoundingSphere &boundingSphere = child->getBound();
  if ( boundingSphere.radius() <= 0 )
    throw ( std::runtime_error ( "Error 1614663463, bounding sphere has zero radius" ) );

  // Get the center of the bounding sphere.
  const osg::Vec3 &center ( boundingSphere.center() );

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  lod->setRange ( 0, 0, 50 );
  lod->setRange ( 1, 50, std::numeric_limits< float >::max() );

#if 0
  osg::BoundingBox bb ( osg::Vec3 ( 0, 0, 0 ), osg::Vec3 ( width, height, _imageList.size() ) );

  osg::ref_ptr< OsgTools::Drawables::VoxelGeometry > vg ( new OsgTools::Drawables::VoxelGeometry() );
  vg->boundingBox( bb );
  vg->numPlanes( 512 );

  {
    osg::ref_ptr< osg::StateSet > ss ( vg->getOrCreateStateSet() );
    osg::Material *mat = new osg::Material;
    mat->setDiffuse ( osg::Material::FRONT, osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );

    //ss->setAttribute ( mat, osg::StateAttribute::ON );
  }

  osg::ref_ptr< osg::Geode > g ( new osg::Geode );
  g->addDrawable( vg.get() );

  texgen->addChild( g.get() );

  // Get or Create the state set
  osg::ref_ptr < osg::StateSet > stateset ( g->getOrCreateStateSet() );
#else
  texgen->addChild( lod.get() );

  // Get or Create the state set
  osg::ref_ptr < osg::StateSet > stateset ( lod->getOrCreateStateSet() );
#endif

  stateset->setMode( GL_LIGHTING, osg::StateAttribute::INHERIT );
  stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
  //stateset->setAttribute(new osg::AlphaFunc(osg::AlphaFunc::LEQUAL,0.2f));

  osg::ref_ptr< osg::Material > material ( new osg::Material );
  material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4( 1.0f,1.0f,1.0f,0.0f ) );
  material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 1.0f,1.0f,1.0f,0.0f ) );
  stateset->setAttributeAndModes( material.get() );
  
  // Add the texture
  stateset->setTextureAttributeAndModes( 0, texture3D.get(), osg::StateAttribute::ON );

  stateset->setTextureMode( 0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON );
  stateset->setTextureMode( 0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON );
  stateset->setTextureMode( 0, GL_TEXTURE_GEN_R, osg::StateAttribute::ON );

  stateset->setTextureAttributeAndModes( 0, new osg::TexEnv( osg::TexEnv::REPLACE ), osg::StateAttribute::ON );

  // Texture Matrix
  osg::ref_ptr < osg::TexMat > texmat ( new osg::TexMat );
  osg::Matrix matrix;
  matrix.makeScale( 1.0, 1.0, 1.0 );
  texmat->setMatrix( matrix );
  stateset->setTextureAttributeAndModes( 0, texmat.get(), osg::StateAttribute::ON );

  // Set needed states for transparency
  stateset->setMode ( GL_BLEND, osg::StateAttribute::ON );
  stateset->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

#if 0
  osg::ref_ptr< osg::Geode > box ( new osg::Geode );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

  osg::BoundingBox bb( -0.5f,-0.5f,-0.5f,0.5f,0.5f,0.0f );

  for( unsigned int i = 0; i < 8; ++i )
    vertices->push_back ( bb.corner( i ) );

  typedef osg::DrawElementsUInt DrawElements;

  osg::ref_ptr< DrawElements > bottom ( new DrawElements ( osg::PrimitiveSet::LINE_LOOP, 0 ) );

  bottom->push_back ( 0 );
  bottom->push_back ( 1 );
  bottom->push_back ( 3 );
  bottom->push_back ( 2 );

  osg::ref_ptr< DrawElements > top ( new DrawElements ( osg::PrimitiveSet::LINE_LOOP, 0 ) );

  top->push_back ( 4 );
  top->push_back ( 5 );
  top->push_back ( 7 );
  top->push_back ( 6 );

  osg::ref_ptr< DrawElements > lines  ( new DrawElements ( osg::PrimitiveSet::LINES, 0 ) );

  lines->push_back ( 0 );
  lines->push_back ( 4 );
  lines->push_back ( 1 );
  lines->push_back ( 5 );
  lines->push_back ( 2 );
  lines->push_back ( 6 );
  lines->push_back ( 3 );
  lines->push_back ( 7 );

  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setVertexArray  ( vertices.get() );
  geometry->addPrimitiveSet ( bottom.get() );
  geometry->addPrimitiveSet ( top.get() );
  geometry->addPrimitiveSet ( lines.get() );

  box->addDrawable ( geometry.get() );

  root->addChild ( box.get() );
#endif

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make cube for 3D texture
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VolumeDocument::_makeCube( unsigned int numSlices, int width, int height )
{
  // Make the geometry
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  
  const float size ( ::sqrt( 3.0 ) );

  // Make the vertices
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array( numSlices * 4 ) );

  const float halfSize ( size * 0.5f );
  float y ( halfSize );
  const float dy ( ( -1.0 * size ) / (float) ( numSlices - 1 ) * 1.0 );

  // Make the texture coordinates
  osg::ref_ptr < osg::Vec3Array > texcoords ( new osg::Vec3Array( 4 * numSlices ) );

  float t  ( 0.0 );
  float dt ( 1.0f / numSlices );

  for(unsigned int i = 0; i < numSlices; ++i, y += dy, t += dt )
  {
    // Set the values
    vertices->at( i * 4 + 0 ).set( -halfSize * width, y, halfSize * height );
    vertices->at( i * 4 + 1 ).set( -halfSize * width, y, -halfSize * height );
    vertices->at( i * 4 + 2 ).set( halfSize * width,  y, -halfSize * height );
    vertices->at( i * 4 + 3 ).set( halfSize * width,  y, halfSize * height );

    // Set the values
    texcoords->at( 0 * i ).set( 0.0f, 1.0f, t );
    texcoords->at( 1 * i ).set( 0.0f, 0.0f, t );
    texcoords->at( 2 * i ).set( 1.0f, 0.0f, t );
    texcoords->at( 3 * i ).set( 1.0f, 1.0f, t );
  }

  // Make the normals
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array( numSlices * 4 ) );
  
  // Set the value
  std::fill( normals->begin(), normals->end(), osg::Vec3( 0.0f, -1.0f, 0.0f ) );
  
  // Set the arrays
  geometry->setVertexArray   ( vertices.get() );
  geometry->setNormalArray   ( normals.get() );
  geometry->setTexCoordArray ( 0, texcoords.get() );

  // Set the bindings
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  // Add the primitive set
  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  osg::ref_ptr < osg::Billboard > billboard ( new osg::Billboard );
  billboard->setMode(osg::Billboard::POINT_ROT_WORLD );
  billboard->addDrawable( geometry.get() );
  billboard->setPosition( 0, osg::Vec3( 0.0f,0.0f,0.0f ) );

  return billboard.release();
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

  /// HACK for vf54 volume.
#if 0
  unsigned int width  ( static_cast < unsigned int > ( image3d->s() ) );
  unsigned int height ( static_cast < unsigned int > ( image3d->t() ) );
  unsigned int depth  ( static_cast < unsigned int > ( image3d->r() ) );

  
  for ( unsigned int i = 0; i < width; ++i )
  {
    for ( unsigned int j = 0; j < height; ++j )
    {
      for ( unsigned int k = 0; k < depth; ++k )
      {
        unsigned char *c2 = image3d->data ( i, j, k );

        if ( *c2 < 60 &&  *(c2+1) < 60 && *(c2+2) < 60 )
        {
          *c2 = 0.0;
          *(c2+1) = 0.0;
          *(c2+2) = 0.0;
          *(c2+3) = 0.0;
        }

      }
    }
  }
#endif
  // Apply a transfer function
  this->_applyTransferFunction( image3d.get() );

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


///////////////////////////////////////////////////////////////////////////////
//
//  Apply a transfer function to the image.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::_applyTransferFunction ( osg::Image *image )
{
#if 0
  unsigned int width  ( static_cast < unsigned int > ( image->s() ) );
  unsigned int height ( static_cast < unsigned int > ( image->t() ) );
  unsigned int depth  ( static_cast < unsigned int > ( image->r() ) );

  osg::ref_ptr< osg::Image > copy ( new osg::Image ( *image, osg::CopyOp::DEEP_COPY_ALL ) );

#if 0
  // Useful typedefs.
    typedef GN::Config::UsulConfig < double, double, unsigned int > DoubleConfig;
    typedef GN::Splines::Curve < DoubleConfig >  DoubleCurve;
    typedef DoubleCurve::IndependentSequence     IndependentSequence;
    typedef DoubleCurve::DependentContainer      DependentContainer;
    typedef DoubleCurve::DependentSequence       DependentSequence;
    typedef DoubleCurve::IndependentType         IndependentType;
    typedef DoubleCurve::ErrorCheckerType        ErrorCheckerType;
    typedef DoubleCurve::Power                   PowerFunctor;
    typedef DoubleCurve::SizeType                SizeType;
    typedef DoubleCurve::Vector                  Point;
    typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorCheckerType > KnotVectorBuilder;

    // Container for the data.
    DependentContainer points;

    // Container for independent variables.
    IndependentSequence params;

    {
      DependentSequence point;
      point.push_back ( 100 );
      point.push_back ( 50 );
      points.push_back ( point );
    }
    
    {
      DependentSequence point;
      point.push_back ( 175 );
      point.push_back ( 50 );
      points.push_back ( point );
    }

    // Make the parameters evenly spaced.
    GN::Algorithms::fill ( params, points.size(), 0, 1 );

    // Should be true.
    USUL_ASSERT ( params.size() == points.size() );

    // Transpose so that the first index of "points" is the dimension.
    Usul::Math::transpose ( points );

    // Make the knot vector. Size it for interpolation.
    IndependentSequence knots;
    const SizeType cubic ( 4 );
    const SizeType order ( std::min<SizeType> ( cubic, params.size() ) ); // TODO, the order should be a preference.
    knots.resize ( params.size() + order );
    KnotVectorBuilder::build ( params, order, knots );

    // The curve.
    DoubleCurve curve;

    // Interpolate the positions.
    GN::Interpolate::global ( order, params, knots, points, curve );

    // Container that the bisecter will fill with u values.
    IndependentSequence uValues;

    // Bisect the curve.  Need a better way to determine the chord height
    GN::Tessellate::bisect ( curve, 1, uValues );
    
    Point pos ( curve.dimension() );

    // Loop through the u values.
    for ( IndependentSequence::const_iterator i = uValues.begin(); i != uValues.end(); ++i )
    {
      // Evaluate the point at given u.
      GN::Evaluate::point ( curve, *i, pos );
    }
#endif

  for ( unsigned int k = 0; k < depth; ++k )
  {
    for ( unsigned int i = 0; i < width; ++i )
    {
      for ( unsigned int j = 0; j < height; ++j )
      {
        double gradient ( this->_calculateGradient( i, j, k, copy.get() ) );
        unsigned char *c = image->data ( i, j, k );
        switch ( image->getPixelFormat() )
        {
        case GL_LUMINANCE: // Greyscale
          break;

        case GL_LUMINANCE_ALPHA: // Greyscale + alpha
#if 0
          if ( gradient < 0.01 )
            c[1] = 0;
          else if ( gradient < 0.02 )
            c[1] = 20;
          else if ( gradient < 0.03 )
            c[1] = 0;
          else
            c[1] = 0;
#else
          
          if ( c[0] < 100 )
            c[1] = 0;
          else if ( c[0] < 175 )
            c[1] = 50;
          else
            c[1] = 0;
#endif
          break;

        case GL_RGB: // Red, green, blue
          break;

        case GL_RGBA: // Red, green, blue, alpha
#if 0
          if ( gradient < 0.02 )
          {
            c[0] = 255;
            c[1] = 255;
            c[2] = 255;
            c[3] = 0;
          }
          else if ( gradient < 0.025 )
          {
            c[0] = 255;
            c[1] = 0;
            c[2] = 255;
            c[3] = 0;
          }
          else if ( gradient < 0.03 )
          {
            c[0] = 0;
            c[1] = 0;
            c[2] = 255;
            c[3] = 20;
          }
          else if ( gradient < 0.04 )
          {
            c[0] = 0;
            c[1] = 255;
            c[2] = 255;
            c[3] = 20;
          }
          else if ( gradient < 0.05 )
          {
            c[0] = 255;
            c[1] = 255;
            c[2] = 0;
            c[3] = 20;
          }
          else if ( gradient < 0.06 )
          {
            c[0] = 100;
            c[1] = 100;
            c[2] = 255;
            c[3] = 20;
          }
          else
          {
            c[0] = 0;
            c[1] = 255;
            c[2] = 0;
            c[3] = 255;
          }
#elif 1
          if ( gradient < 0.5 )
          {
            c[0] = 255;
            c[1] = 255;
            c[2] = 255;
            c[3] = 5;
          }
          else
          {
            Usul::Math::Vec3f pixel;
            Usul::Functions::hsvToRgb < float > ( pixel[0], pixel[1], pixel[2], gradient * 360, 100.0, 100.0 );

            c[0] = pixel[0];
            c[1] = pixel[1];
            c[2] = pixel[2];
          }
#else
          if( gradient < 0.50 )
          {
            c[0] = 255;
            c[1] = 255;
            c[2] = 255;
            c[3] = 5;
          }
          else if( gradient < 0.8 )
          {
            c[0] = 0;
            c[1] = 0;
            c[2] = 255;
            c[3] = 20;
          }
          else if( gradient < 0.9 )
          {
            c[0] = 170;
            c[1] = 0;
            c[2] = 255;
            c[3] = 20;
          }
          else if( gradient < 0.95 )
          {
            c[0] = 0;
            c[1] = 255;
            c[2] = 0;
            c[3] = 20;
          }
          else
          {
            c[0] = 255;
            c[1] = 255;
            c[2] = 0;
            c[3] = 20;
          }
#endif
          break;
        }
      }
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate gradient at given location.
//
///////////////////////////////////////////////////////////////////////////////

double VolumeDocument::_calculateGradient ( unsigned int s, unsigned int t, unsigned int r, osg::Image *image )
{
  unsigned int width  ( static_cast < unsigned int > ( image->s() ) );
  unsigned int height ( static_cast < unsigned int > ( image->t() ) );
  unsigned int depth  ( static_cast < unsigned int > ( image->r() ) );

  const unsigned char *c = image->data ( s, t, r );

  // For now if value is on the edge, return
  if( s == 0.0 || t == 0.0 || r == 0.0 || s == ( width - 1 ) || t == ( height - 1 ) || r == ( depth - 1 ) )
    return 0.0;

  // Backup one.  Makes the loop a little easier to read.
  s--; t--; r--;

  double value ( 0.0 );

  for ( int i = 0; i < 3; ++i )
  {
    for ( int j = 0; j < 3; ++j )
    {
      for ( int k = 0; k < 3; ++k )
      {
        const unsigned char *c2 = image->data ( ( s + i ), ( t + j ), ( r + k ) );

        double temp ( c2[0] - c[0] );

        value += ( temp < 0.0 ? -temp : temp );
      }
    }
  }

  // Average
  value /= 26.0;

  // Normalize
  value /= 255.0;

  return value;
}

