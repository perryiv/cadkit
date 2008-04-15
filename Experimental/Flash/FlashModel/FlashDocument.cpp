
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/Flash/FlashModel/FlashDocument.h"
#include "Experimental/Flash/FlashModel/H5File.h"
#include "Experimental/Flash/FlashModel/Dataset.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/Color.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Case.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Volume/TransferFunction1D.h"

#include "osg/MatrixTransform"
#include "osg/Point"

#include "boost/multi_array.hpp"

#include "hdf5.h"

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
  _currentTimestep ( 0 ),
  _root ( new osg::Group ),
  _dirty ( true ),
  _currentTransferFunction ( 0 ),
  _transferFunctions(),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_addMember ( "filenames", _filenames );
  
  this->_buildDefaultTransferFunctions();
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
    _filenames.push_back ( filename );
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
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &FlashDocument::_buildScene ), "3960901428" );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::_buildScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Remove what we have.
  _root->removeChild ( 0, _root->getNumChildren() );
  
  // Make sure we are in range...
  if ( _currentTimestep < _filenames.size() )
  {
    // Open the file.
    H5File file ( _filenames.at ( _currentTimestep ) );
    
    // Make sure we got a good file handle.
    if ( false == file.isOpen())
      throw std::runtime_error ( "Error 1747059583: Could not open file: " + _filenames.at ( _currentTimestep ) );
    
    // Open the data set.
    Dataset::RefPtr boundingBox ( file.openDataset( "bounding box" ) );
    
    // Make sure we got a good data set handle.
    if ( false == boundingBox.valid() || false == boundingBox->isOpen() )
      throw std::runtime_error ( "Error 1220099573: Could not open data set." );
    
    // Get the rank.
    //const int dimensions ( boundingBox->dimensions() );
    
    // Get the number of elements.
    const hsize_t elements ( boundingBox->elements() );
    
    // Buffer for the data.
    std::vector<double> buffer ( elements );
    
    // Read the data.
    boundingBox->read ( H5T_NATIVE_DOUBLE, &buffer[0] );
    
    // Number of nodes.
    const unsigned int numNodes ( boundingBox->size ( 0 ) );
    
    // Open the data set.
    Dataset::RefPtr levelsData ( file.openDataset( "refine level" ) );
    
    // Make sure we got a good data set handle.
    if ( false == levelsData.valid() || false == levelsData->isOpen() )
      throw std::runtime_error ( "Error 3972712559: Could not open data set." );
    
    // Buffer for refine level.
    std::vector<int> levels ( levelsData->elements() );
    
    // Read the levels.
    levelsData->read ( H5T_NATIVE_INT, &levels[0] );
    
    // Open the data set.
    Dataset::RefPtr nodeData ( file.openDataset( "node type" ) );
    
    // Make sure we got a good data set handle.
    if ( false == nodeData.valid() || false == nodeData->isOpen() )
      throw std::runtime_error ( "Error 1356818230: Could not open data set." );
    
    // Buffer for node type.
    std::vector<int> nodeType ( nodeData->elements() );
    
    // Read the levels.
    nodeData->read ( H5T_NATIVE_INT, &nodeType[0] );
    
    // Colors.
    std::vector<osg::Vec4> colors ( 6 );
    colors[0] = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );
    colors[1] = osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 );
    colors[2] = osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 );
    colors[3] = osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 );
    colors[4] = osg::Vec4 ( 1.0, 0.0, 1.0, 1.0 );
    colors[5] = osg::Vec4 ( 0.0, 1.0, 1.0, 1.0 );
    
    // Read the density.
    Dataset::RefPtr densityData ( file.openDataset( "dens" ) );
    
    // Make sure it's valid.
    if ( false == densityData.valid() || false == densityData->isOpen() )
      throw std::runtime_error ( "Error 6348683680: Could not open data set." );
    
    // Buffer for density.
    typedef boost::multi_array<double, 4> Array;
    Array density ( boost::extents [densityData->size ( 0 )][densityData->size ( 1 )][densityData->size ( 2 )][densityData->size ( 3 )] );
    
    //std::vector<double> density ( densityData->elements() );
    
    // Read the density.
    //densityData->read ( H5T_NATIVE_DOUBLE, &density[0] );
    densityData->read ( H5T_NATIVE_DOUBLE, density.origin() );
    
    // Get min and max from the data set.
    const double minimum ( densityData->attribute ( "minimum" ) );
    const double maximum ( densityData->attribute ( "maximum" ) );
    
    // Get the dimensions in each direction.
    const unsigned int x ( densityData->size ( 1 ) );
    const unsigned int y ( densityData->size ( 2 ) );
    const unsigned int z ( densityData->size ( 3 ) );
    
    // Make bounding boxes.
    for ( unsigned int num = 0; num < numNodes; ++num )
    {
      const bool isLeaf ( 1 == nodeType.at ( num ) );
      const int level ( levels.at ( num ) );
      
      if ( isLeaf && 5 == level )
      {
        const unsigned int start ( num * 6 );

        osg::BoundingBox bb ( buffer[start    ], buffer[start + 2], buffer[start + 4], 
                              buffer[start + 1], buffer[start + 3], buffer[start + 5] );

        bb._min =  bb._min / float ( 1e+20f );
        bb._max =  bb._max / float ( 1e+20f );
        
#if 0
        OsgTools::ColorBox box ( bb );
        box.color_policy().color ( colors.at ( level - 1 ) );
        
        osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
        mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
        mt->addChild ( box() );
        
        // Add the bounding box.
        _root->addChild ( mt.get() );
        
        // Wire-frame.
        OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
        OsgTools::State::StateSet::setLighting ( mt.get(), false );
#endif
        //const unsigned int total ( x * y * z );
        
#if 1
        // Data pointer.
        //double* data ( reinterpret_cast<double*> ( &density[0] + ( num * total ) ) );
        
        const osg::Vec3 min ( bb._min );
        const osg::Vec3 max ( bb._max );
        
        const double lengthX ( bb._max.x() - bb._min.x() );
        const double lengthY ( bb._max.y() - bb._min.y() );
        const double lengthZ ( bb._max.z() - bb._min.z() );
        
        const double deltaX ( lengthX / ( x + 1 ) );
        const double deltaY ( lengthY / ( y + 1 ) );
        const double deltaZ ( lengthZ / ( z + 1 ) );
        
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
              const double value ( density[num][k][j][i] );
              
              float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
              
              double temp ( ( ( ::log10 ( value * 5 ) + 3.0 ) - tmin ) / ( tmax - tmin ) );
              
              Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( temp * 300.0f ), 1.0f, 1.0f );
              
              osg::Vec3 position ( ( i + 1 ) * deltaX, ( j + 1 ) * deltaY, ( k + 1 ) * deltaZ );
              vertices->push_back ( osg::Vec3 ( bb._min + position ) );
              myColors->push_back ( osg::Vec4 ( r, g, b, 1.0 ) );
            }
          }
        }
        
        osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
        geometry->setVertexArray ( vertices.get() );
        geometry->setNormalArray ( normals.get() );
        geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
        geometry->setColorArray ( myColors.get() );
        geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
        
        geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_POINTS, 0, vertices->size() ) );
        
        osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
        geode->addDrawable ( geometry.get() );
        
        osg::ref_ptr<osg::Point> ps ( new osg::Point );
        ps->setSize ( 5.0f );
        geode->getOrCreateStateSet()->setAttributeAndModes ( ps.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
        
        // Turn off lighting
        OsgTools::State::StateSet::setLighting ( geode.get(), false );
        
        _root->addChild ( geode.get() );
#else
        osg::ref_ptr<OsgTools::Volume::Texture3DVolume> volumeNode ( new OsgTools::Volume::Texture3DVolume );
        
        // Set the volume nodes bounding box.
        volumeNode->boundingBox ( bb );
        
        // Get the 3D image for the volume.
        osg::ref_ptr<osg::Image> image ( new osg::Image );
        //image->allocateImage ( x, y, z, GL_LUMINANCE, GL_UNSIGNED_BYTE );
        image->allocateImage ( x, y, z, GL_LUMINANCE, GL_FLOAT );
        
        // Data pointer.
        //double* data ( &density[0] + ( num * total ) );
        
        for ( unsigned int i = 0; i < x; ++i )
        {
          for ( unsigned int j = 0; j < y; ++j )
          {
            for ( unsigned int k = 0; k < z; ++k )
            {
              //double value ( density[num][i][j][k] );
              double value ( density[num][k][j][i] );
#if 0
              value = ( value - minimum ) / ( maximum - minimum );
              const unsigned char pixel ( static_cast < unsigned char > ( value * 255 ) );
              image->data()[v] = pixel;
#else
              float* pixel ( reinterpret_cast<float*> ( image->data( i, j, k ) ) );
              *pixel = static_cast < float > ( ( value - minimum ) / ( maximum - minimum ) );
#endif
            }
          }
        }
        
        volumeNode->numPlanes ( 64 );
        volumeNode->image ( image.get() );
        
        // Build the transfer function.
        volumeNode->transferFunction ( _transferFunctions.at ( 0 ).get() );
        
        volumeNode->getOrCreateStateSet()->setRenderBinDetails ( 1, "DepthSortedBin" );
        
        _root->addChild ( volumeNode.get() );
#endif
      }
    }
  }
  
  // No longer dirty.
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // Buid the scene if we need to.
  if ( this->dirty () )
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &FlashDocument::_buildScene ), "3279281359" );
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

bool FlashDocument::dirty () const
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

void FlashDocument::_buildDefaultTransferFunctions ()
{
  // Typedefs.
  typedef OsgTools::Volume::TransferFunction1D TransferFunction1D;
  typedef TransferFunction1D::Colors           Colors;
  
  const unsigned int size ( 256 );
  
  Colors hsv       ( size );
  
  // Don't draw voxels with a value of zero.
  hsv.at ( 0 ) [ 3 ] = 0;
  
  const double tmin ( ::log10 ( 1.0 / ( size - 1 ) ) + 3.0 );
  const double tmax ( ::log10 ( 5.0 ) + 3.0 );
  
  for ( unsigned int i = 1; i < size; ++i )
  {
    float value ( static_cast < float > ( i ) / ( size - 1 ) );
    const unsigned char alpha ( 35 );
    
    float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
    
    double temp ( ( ( ::log10 ( value * 5 ) + 3.0 ) - tmin ) / ( tmax - tmin ) );
    
    Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( temp * 300.0f ), 1.0f, 1.0f );
    //Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( value * 300.0f ), 1.0f, 1.0f );
    hsv.at ( i ) [ 0 ] = static_cast < unsigned char > ( r * 255 );
    hsv.at ( i ) [ 1 ] = static_cast < unsigned char > ( g * 255 );
    hsv.at ( i ) [ 2 ] = static_cast < unsigned char > ( b * 255 );
    hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( value * alpha );
    //hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( 127 );
  }
  
  _transferFunctions.push_back ( new TransferFunction1D ( hsv ) );
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
