
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/Flash/FlashModel/Timestep.h"
#include "Experimental/Flash/FlashModel/H5File.h"
#include "Experimental/Flash/FlashModel/Dataset.h"

#include "Usul/Functions/Color.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Volume/Texture3DVolume.h"
#include "OsgTools/Volume/TransferFunction1D.h"

#include "osg/BlendFunc"
#include "osg/MatrixTransform"
#include "osg/Point"

#include "hdf5.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Timestep::Timestep( const std::string& filename ) : BaseClass(),
  _filename ( filename ),
  _scale ( 1.0 / 1e+20f ),
  _boundingBoxes(),
  _leafFlags(),
  _levels(),
  _data (),
  _minimum ( 0.0 ),
  _maximum ( 0.0 ),
  _hierarchy()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Timestep::~Timestep()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::init()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Open the file.
  H5File file ( _filename );
  
  // Make sure we got a good file handle.
  if ( false == file.isOpen() )
    throw std::runtime_error ( "Error 1747059583: Could not open file: " + _filename );
  
  // Initialize the bounding boxes.
  this->_initBoudingBoxes ( file );
  
  // Initialize the level numbers.
  this->_initLevels ( file );
  
  // Initialize flags for leaves.
  this->_initLeafFlags ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the bounding boxes.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initBoudingBoxes ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr boundingBox ( file.openDataset( "bounding box" ) );
  
  // Make sure we got a good data set handle.
  if ( false == boundingBox.valid() || false == boundingBox->isOpen() )
    throw std::runtime_error ( "Error 1220099573: Could not open data set." );
  
  // Get the number of elements.
  const hsize_t elements ( boundingBox->elements() );
  
  // Buffer for the data.
  std::vector<double> buffer ( elements );
  
  // Read the data.
  boundingBox->read ( H5T_NATIVE_DOUBLE, &buffer[0] );
  
  // Number of nodes.
  const unsigned int numNodes ( boundingBox->size ( 0 ) );
  
  // Clear what we have.
  _boundingBoxes.clear();
  
  // Reserve enough room.
  _boundingBoxes.reserve ( numNodes );
  
  // Make bounding boxes.
  for ( unsigned int num = 0; num < numNodes; ++num )
  {
    const unsigned int start ( num * 6 );
      
    osg::BoundingBox bb ( buffer[start    ], buffer[start + 2], buffer[start + 4], 
                          buffer[start + 1], buffer[start + 3], buffer[start + 5] );
    
    _boundingBoxes.push_back ( bb );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the leaf flags.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initLeafFlags ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr nodeData ( file.openDataset( "node type" ) );
  
  // Make sure we got a good data set handle.
  if ( false == nodeData.valid() || false == nodeData->isOpen() )
    throw std::runtime_error ( "Error 1356818230: Could not open data set." );
  
  // Buffer for node type.
  std::vector<int> nodeType ( nodeData->elements() );
  
  // Read the levels.
  nodeData->read ( H5T_NATIVE_INT, &nodeType[0] );
  
  // Clear what we have.
  _leafFlags.clear();
  
  // Make enough room.
  _leafFlags.reserve ( nodeType.size() );
  
  for ( unsigned int i = 0; i < nodeType.size(); ++i )
    _leafFlags.push_back ( 1 == nodeType.at ( i ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the levels.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initLevels ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr levelsData ( file.openDataset( "refine level" ) );
  
  // Make sure we got a good data set handle.
  if ( false == levelsData.valid() || false == levelsData->isOpen() )
    throw std::runtime_error ( "Error 3972712559: Could not open data set." );
  
  // Clear what we have.
  _levels.clear();
  
  // Make vector the correct size.
  _levels.resize ( levelsData->elements() );
  
  // Read the levels.
  levelsData->read ( H5T_NATIVE_INT, &_levels[0] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the hierarchy.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::_initHierarchy ( H5File& file )
{
  // Open the data set.
  Dataset::RefPtr data ( file.openDataset( "gid" ) );
  
  // Make sure we got a good data set handle.
  if ( false == data.valid() || false == data->isOpen() )
    throw std::runtime_error ( "Error 2495376396: Could not open data set." );
  
  // Make vector the correct size.
  _hierarchy.resize ( boost::extents [data->size ( 0 )][data->size ( 1 )] );
  
  // Read the levels.
  data->read ( H5T_NATIVE_INT, _hierarchy.origin() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Timestep::buildScene( bool drawBBox, bool drawPoints, bool drawVolume )
{
  Guard guard ( this->mutex() );
   
  osg::ref_ptr<osg::Group> root ( new osg::Group );
  
  // Colors.
  std::vector<osg::Vec4> colors ( 6 );
  colors[0] = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );
  colors[1] = osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 );
  colors[2] = osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 );
  colors[3] = osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 );
  colors[4] = osg::Vec4 ( 1.0, 0.0, 1.0, 1.0 );
  colors[5] = osg::Vec4 ( 0.0, 1.0, 1.0, 1.0 );

  // Get the dimensions in each direction.
  const unsigned int x ( _data.shape()[1] );
  const unsigned int y ( _data.shape()[2] );
  const unsigned int z ( _data.shape()[3] );
  
  // Get min and max from the data set.
  const double minimum ( _minimum );
  const double maximum ( _maximum );

  // Typedefs.
  typedef OsgTools::Volume::TransferFunction1D TransferFunction1D;
  typedef TransferFunction1D::Colors           Colors;
  
  const unsigned int size ( 256 );
  Colors hsv       ( size );
  
  {
    const double tmin ( ::log10 ( minimum * 5 ) + 3.0 );
    const double tmax ( ::log10 ( maximum * 5 ) + 3.0 );
    
    const double range ( maximum - minimum );
    
    for ( unsigned int i = 0; i < size; ++i )
    {
      const float u ( static_cast < float > ( i ) / ( size - 1 ) );
      const unsigned char alpha ( 35 );
      
      float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
      
      const double value ( minimum + ( u * range ) );
      double temp ( ( ( ::log10 ( value * 5 ) + 3.0 ) - tmin ) / ( tmax - tmin ) );
      
      Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( temp * 300.0f ), 1.0f, 1.0f );
      hsv.at ( i ) [ 0 ] = static_cast < unsigned char > ( r * 255 );
      hsv.at ( i ) [ 1 ] = static_cast < unsigned char > ( g * 255 );
      hsv.at ( i ) [ 2 ] = static_cast < unsigned char > ( b * 255 );
      hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( u * alpha );
      //hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( 127 );
    }
  }
  
  TransferFunction1D::RefPtr tf ( new TransferFunction1D ( hsv ) );
  
  const unsigned int numNodes ( _boundingBoxes.size() );
  
  // Make bounding boxes.
  for ( unsigned int num = 0; num < numNodes; ++num )
  {
    osg::BoundingBox bb ( _boundingBoxes.at ( num ) );
    const bool isLeaf ( _leafFlags.at ( num ) );
    const int level ( _levels.at ( num ) );
    
    if ( isLeaf && 5 == level )
    //if ( isLeaf )
    {      
      bb._min =  bb._min * _scale;
      bb._max =  bb._max * _scale;

      // Add the bounding box.
      if ( drawBBox )
        root->addChild ( this->_buildBoundingBox ( bb, colors.at ( level - 1 ) ) );
      
      if ( drawPoints )
      {
        const osg::Vec3 min ( bb._min );
        const osg::Vec3 max ( bb._max );
        
        const double lengthX ( bb._max.x() - bb._min.x() );
        const double lengthY ( bb._max.y() - bb._min.y() );
        const double lengthZ ( bb._max.z() - bb._min.z() );
        
        const double deltaX ( lengthX / x );
        const double deltaY ( lengthY / y );
        const double deltaZ ( lengthZ / z );
        
        osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
        osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( 1 ) );
        osg::ref_ptr<osg::Vec4Array> myColors ( new osg::Vec4Array );
        
        // One normal.
        normals->at ( 0 ) = osg::Vec3 ( 0.0, 0.0, 1.0 );
        
        const double tmin ( ::log10 ( minimum * 5 ) + 3.0 );
        const double tmax ( ::log10 ( maximum * 5 ) + 3.0 );
        
        for ( unsigned int i = 0; i < x; ++i )
        {
          for ( unsigned int j = 0; j < y; ++j )
          {
            for ( unsigned int k = 0; k < z; ++k )
            {
              const double value ( _data[num][k][j][i] );
              
              float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
              
              double temp ( ( ( ::log10 ( value * 5 ) + 3.0 ) - tmin ) / ( tmax - tmin ) );
              
              Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( temp * 300.0f ), 1.0f, 1.0f );
              
              osg::Vec3 position ( ( static_cast<double> ( i ) + 0.5 ) * deltaX, 
                                   ( static_cast<double> ( j ) + 0.5 ) * deltaY, 
                                   ( static_cast<double> ( k ) + 0.5 ) * deltaZ );
              vertices->push_back ( osg::Vec3 ( bb._min + position ) );
              myColors->push_back ( osg::Vec4 ( r, g, b, temp * 0.5f ) );
            }
          }
        }
        
        osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
        geometry->setVertexArray ( vertices.get() );
        geometry->setNormalArray ( normals.get() );
        geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
        geometry->setColorArray ( myColors.get() );
        geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
        geometry->setUseDisplayList ( false );
        
        geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_POINTS, 0, vertices->size() ) );
        
        osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
        geode->addDrawable ( geometry.get() );
        
        osg::ref_ptr<osg::Point> ps ( new osg::Point );
        ps->setSize ( 5.0f );
        geode->getOrCreateStateSet()->setAttributeAndModes ( ps.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
        
        // Turn off lighting
        OsgTools::State::StateSet::setLighting ( geode.get(), false );
        
        geode->getOrCreateStateSet()->setRenderBinDetails ( 1, "DepthSortedBin" );
        
        root->addChild ( geode.get() );
      }

      if ( drawVolume )
      {
        osg::ref_ptr<OsgTools::Volume::Texture3DVolume> volumeNode ( new OsgTools::Volume::Texture3DVolume );
        
        // Set the volume nodes bounding box.
        volumeNode->boundingBox ( bb );
        
        // Get the 3D image for the volume.
        osg::ref_ptr<osg::Image> image ( new osg::Image );
        
        image->allocateImage ( x, y, z, GL_LUMINANCE, GL_UNSIGNED_BYTE );
        //image->allocateImage ( x, y, z, GL_LUMINANCE, GL_FLOAT );
        
        for ( unsigned int i = 0; i < x; ++i )
        {
          for ( unsigned int j = 0; j < y; ++j )
          {
            for ( unsigned int k = 0; k < z; ++k )
            {
              double value ( _data[num][k][j][i] );
  #if 1
              value = ( value - minimum ) / ( maximum - minimum );
              const unsigned char pixel ( static_cast < unsigned char > ( value * 255 ) );
              *image->data( i, j, k ) = pixel;
  #else
              float* pixel ( reinterpret_cast<float*> ( image->data( i, j, k ) ) );
              *pixel = static_cast < float > ( ( value - minimum ) / ( maximum - minimum ) );
  #endif
            }
          }
        }
        
        // Set the transfer function.
        volumeNode->transferFunction ( tf.get() );
        
        volumeNode->numPlanes ( 64 );
        volumeNode->image ( image.get() );
        
        volumeNode->getOrCreateStateSet()->setRenderBinDetails ( 1, "DepthSortedBin" );
        
        root->addChild ( volumeNode.get() );
      }
      //if ( 16 == root->getNumChildren() )
      //  break;
    }
  }

  // Create a blend function.
  osg::ref_ptr< osg::BlendFunc > blendFunc ( new osg::BlendFunc );
  blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  root->getOrCreateStateSet()->setAttributeAndModes( blendFunc.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the data.
//
///////////////////////////////////////////////////////////////////////////////

void Timestep::loadData ( const std::string& name )
{
  Guard guard ( this->mutex() );
  
  // Open the file.
  H5File file ( _filename );
  
  // Make sure we got a good file handle.
  if ( false == file.isOpen() )
    throw std::runtime_error ( "Error 1747059583: Could not open file: " + _filename );
  
  // Read the density.
  Dataset::RefPtr data ( file.openDataset( name ) );
  
  // Make sure it's valid.
  if ( false == data.valid() || false == data->isOpen() )
    throw std::runtime_error ( "Error 6348683680: Could not open data set." );
  
  // Get the dimensions in each direction.
  const unsigned int x ( data->size ( 1 ) );
  const unsigned int y ( data->size ( 2 ) );
  const unsigned int z ( data->size ( 3 ) );
  
  // Buffer for density.
  _data.resize ( boost::extents [data->size ( 0 )][x][y][z] );
  
  // Fill the buffer.
  data->read ( H5T_NATIVE_DOUBLE, _data.origin() );
  
  // Get min and max from the data set.
  _minimum = data->attribute ( "minimum" );
  _maximum = data->attribute ( "maximum" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Timestep::_buildBoundingBox ( const osg::BoundingBox& bb, const osg::Vec4f& color ) const
{
  OsgTools::ColorBox box ( bb );
  box.color_policy().color ( color );
  
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
  mt->addChild ( box() );
   
  // Wire-frame.
  OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
  OsgTools::State::StateSet::setLighting ( mt.get(), false );
  
  return mt.release();
}
