#include "TriangleReaderGrassRaster.h"

#include "Usul/File/Stats.h"
#include "Usul/File/Path.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Policies/Update.h"
#include "Usul/Endian/Endian.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Types/Types.h"
#include "Usul/Predicates/FileExists.h"

#include "osgDB/ReadFile"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

const int GRASS_RASTER_FORMAT_FLOAT ( -1 );
const int GRASS_RASTER_FORMAT_8Bit_INT ( 0 );
const int GRASS_RASTER_FORMAT_16Bit_INT ( 1 );

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangleReaderGrassRaster, TriangleReaderGrassRaster::BaseClass );



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderGrassRaster::TriangleReaderGrassRaster( const std::string &file, Usul::Interfaces::IUnknown *caller, TriangleDocument *doc ):
BaseClass(),
_dir            ( "" ),
_caller         ( caller ),
//_file           ( "ziqlab_watersheds_15m_dem_clipped", Usul::File::size ( file ) ),
_file           ( "", 0 ),
_document       ( doc ),
_xmlFilename    ( file )
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderGrassRaster::~TriangleReaderGrassRaster()
{
	USUL_TRACE_SCOPE;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for reading a stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to swap bytes in containers.
//
///////////////////////////////////////////////////////////////////////////////

struct SwapBytes
{
  
  void operator () ( float &v ) const
  {
    Usul::Endian::FromBigToSystem::convert ( v );
  }
  void operator () ( double &v ) const
  {
    Usul::Endian::FromBigToSystem::convert ( v );
  }
  void operator () ( int &v ) const
  {
    Usul::Endian::FromBigToSystem::convert ( v );
  }
  void operator () ( unsigned int &v ) const
  {
    Usul::Endian::FromBigToSystem::convert ( v );
  }
  void operator () ( Usul::Types::Int16 &v ) const
  {
    Usul::Endian::FromBigToSystem::convert ( v );
  }
  
};

  struct ReadStream
  {
    
    ReadStream ( const std::string &file, std::istream &in ) : _file ( file ), _in ( in )
    {
    }
    ReadStream ( const ReadStream &r ) : _file ( r._file ), _in ( r._in )
    {
    }
    template < class T > void operator () ( T &t )
    {
      if ( _in.eof() )
      {
        throw std::runtime_error ( "Error 3191797615: Unexpected end of file when reading: " + _file );
      }
      _in >> t;
    }
  private:
    ReadStream &operator = ( const ReadStream & );
    std::string _file;
    std::istream &_in;
  };
}



/////////////////
//  PUBLIC
/////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* TriangleReaderGrassRaster::buildScene()
{
  USUL_TRACE_SCOPE;
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  OsgTools::Triangles::TriangleSet::Options opt;
  opt[ "normals" ] = "per-vertex";
  opt[ "colors" ]  = "per-vertex";
  std::cout << "Building scene..." << std::endl;
//  group->addChild( _triangleSet->buildScene( opt, 0x0 ) );
  group->addChild( _document->buildScene( opt, _caller ) );
  return group.release();

}

///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::operator()()
{
  USUL_TRACE_SCOPE;

  // Clear accumulated state.
  this->clear();

  //Read Parameters from XML
  this->_readXML();

  // Read the file.
  this->_read();

  // Load the texture file
  this->_loadTexture ( _textureFilename, _caller );

  // Display stats of the triangle set
  this->_stats();

  // testing...
#if 1
  const std::string name ( Usul::File::fullPath ( "testwrite.tdf" ) );
  _document->write ( name );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::read()
{
  USUL_TRACE_SCOPE;
  // Clear accumulated state.
  this->clear();

  //Read Parameters from XML
  this->_readXML();

  // Read the file.
  this->_read();

  // Load the texture file
  this->_loadTexture( _textureFilename, _caller );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Note: we do not clear the reference-counted objects because 
//  we hand those to the document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::clear()
{
  USUL_TRACE_SCOPE;
}


/////////////////
//  PROTECTED
/////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the xml grass raster file information.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_readXML()
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );

  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( _xmlFilename );

  if ( "grs" == document->name() )
  {
    // if the parameter file is loaded correctly go ahead and load the 1st map
    this->_parseXML( *document, _caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the xml grass raster file information.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_parseXML( XmlTree::Node &node, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "directory" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _dir );
    }
    if ( "prefix" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _file.first );
      _file.second = Usul::File::size ( _file.first );
    }
    if ( "texture" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _textureFilename );
      
    }

  }

#if 0
  Children& children ( node.children() );
	
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "set" == node->name() )
    {
      
    }
    
  }
#endif  


}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the headerfile.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_readHeader()
{
  USUL_TRACE_SCOPE;
  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  std::string filename = _dir + "cellhd/" + _file.first;
  in.open ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1099205557: Failed to open file: " + filename );
  
  //Read the header file
  {
    std::string label;
    in >> label >> _header.proj;      // projection
    in >> label >> _header.zone;      // zone
    in >> label >> _header.north;     // north
    in >> label >> _header.south;     // south
    in >> label >> _header.east;      // east
    in >> label >> _header.west;      // west
    in >> label >> _header.cols;      // number of columns
    in >> label >> _header.rows;      // number of rows
    in >> label >> label >>_header.ew_resol;  // East/West Resolution
    in >> label >> label >>_header.ns_resol;  // North/South Resolution
    in >> label >> _header.format;
    in >> label >> _header.compressed;
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_read()
{
  USUL_TRACE_SCOPE;
  // Read header file.
  std::cout << "Reading Header Information..." << std::endl;
  this->_readHeader();

  // More local variables.
  const unsigned int numPoints ( ( _header.rows + 1 ) * ( _header.cols +1 ) );

  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  std::string filename = "";
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  if( _header.format == ::GRASS_RASTER_FORMAT_FLOAT )
    filename = _dir + "fcell/" + _file.first;
  if( _header.format == ::GRASS_RASTER_FORMAT_16Bit_INT ||
      _header.format == ::GRASS_RASTER_FORMAT_8Bit_INT  )
    filename = _dir + "cell/" + _file.first;

  in.open ( filename.c_str(), std::ios::in | std::ios::binary );
  //std::ifstream in ( filename.c_str(), std::ios::in | std::ios::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1099205557: Failed to open file: " + filename );

  
  

  
  // Floating point raster files
  if( _header.format == ::GRASS_RASTER_FORMAT_FLOAT )
  {
    FloatVec vertices ( numPoints, 0 );

    // Read the data
    in.read ( reinterpret_cast<char *> ( &vertices[0] ), vertices.size() * sizeof ( FloatVec::value_type ) );
   
    // Swap the bytes.
    std::for_each ( vertices.begin(), vertices.end(), Helper::SwapBytes() );

    // Make the Triangle Set
    this->_makeTriangleDocument( vertices );
  }

  // 1 Byte Integer raster files
  if( _header.format == ::GRASS_RASTER_FORMAT_8Bit_INT )
  {
    Int8Vec vertices ( numPoints, 0 );

    // Read the data
    in.read ( reinterpret_cast<char *> ( &vertices[0] ), vertices.size() * sizeof ( Int8Vec::value_type ) );
   
    // Make the Triangle Set
    this->_makeTriangleDocument( vertices );
  }
  
  // 2 Byte Integer raster files
  if( _header.format == ::GRASS_RASTER_FORMAT_16Bit_INT )
  {
    Int16Vec vertices ( numPoints, 0 );

    // Read the data
    in.read ( reinterpret_cast<char *> ( &vertices[0] ), vertices.size() * sizeof ( Int16Vec::value_type ) );
   
    // Swap the bytes.
    std::for_each ( vertices.begin(), vertices.end(), Helper::SwapBytes() );

    // Make the Triangle Set
    this->_makeTriangleDocument( vertices );
  }
  //delete old triangles
  //_triangleSet->clear();

  // Reserve triangles.
  // NOTE: commented out this line to debug a bad_alloc error happening
  //       in reserve, called by reserveTriangles( ... )
  //_triangleSet->reserveTriangles ( gridSize[0] * gridSize[1] );

  // Grab the first row.
  //std::for_each ( row0.begin(), row0.end(), vertices );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_incrementProgress ( bool state, float i, float num )
{
  USUL_TRACE_SCOPE;
 // std::cout << "Percent Complete: " << i << "/" << num << "( " <<  ( i / num ) * 100 << " )\r" << std::flush;
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  _document->setProgressBar ( state, numerator, denominator, _caller );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );

#if 0
  // Check to see if we've been cancelled.
  Usul::Threads:Thread::RefPtr thread ( Usul::Threads::Manager::instance().currentThread() );
  if ( true == thread.valid() )
  {
    if ( Usul::Threads::Thread::CANCELLED == thread->result() )
    {
      // Throws the exception to stop executing this thread.
      thread->cancel();
    }
  }
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//  Functor create the triangle document
//
///////////////////////////////////////////////////////////////////////////////
template < class VectorType > void TriangleReaderGrassRaster::_makeTriangleDocument( VectorType vertices )
{
   // Useful typedefs.
  typedef std::vector<float> Values;
  typedef osg::Vec3f Vec3;
  typedef Usul::Predicates::CloseFloat<double> Close;

  // Initialize local vaiables.
  Usul::Math::Vec2ui gridSize ( _header.rows + 1 , _header.cols +1 );
  Usul::Math::Vec2d lowerLeft ( _header.west, _header.south );
  float cellSize ( _header.ns_resol );
  float noDataValue ( 0 );

  // More local variables.
  const unsigned int numPoints ( gridSize[0] * gridSize[1] );
  Vec3 a0 ( 0, 0, 0 ), a1 ( 0, 0, 0 ), b0 ( 0, 0, 0 ), b1 ( 0, 0, 0 );
  Close close ( 0.0005 ); // Reasonable?
  Usul::Policies::TimeBased update ( 1000 ); // Every second.

  std::cout << "Reading Vertices..." << std::endl;
  // Loop until we've read all the points.
  const unsigned int stopR ( gridSize[0] - 1 );
  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = stopR;
  
  for ( unsigned int i = 0; i < stopR; ++i )
  {
    // Grab the next row.
    //std::for_each ( row1.begin(), row1.end(), vertices );

    // Shortcuts.
    const Values::value_type xa ( lowerLeft[1] + ( ( i     ) * cellSize ) );
    const Values::value_type xb ( lowerLeft[1] + ( ( i + 1 ) * cellSize ) );

    // Loop over the columns.
    const unsigned int stopC ( gridSize[1] - 1 );
    for ( unsigned int j = 0; j < stopC; ++j)
    {
      // Shortcuts.
      const Values::value_type y0 ( lowerLeft[0] + ( ( j     ) * cellSize ) );
      const Values::value_type y1 ( lowerLeft[0] + ( ( j + 1 ) * cellSize ) );

      // Calculate vertices of current triangles.
      
      const Vec3 a ( xa, y0, vertices.at ( ( i * stopC ) + j ) );
      const Vec3 b ( xa, y1, vertices.at ( ( i * stopC ) + ( j + 1 ) ) );
      const Vec3 c ( xb, y0, vertices.at ( ( ( i + 1 ) * stopC ) + j     ) );
      const Vec3 d ( xb, y1, vertices.at ( ( ( i + 1 ) * stopC ) + ( j + 1 ) ) );
      

      // See if points are close to "no data".
      if ( false == close ( d[2], noDataValue ) &&
           false == close ( a[2], noDataValue ) )
      {
        // Add first triangle.
        if ( false == close ( c[2], noDataValue ) )
        {
          Vec3 n ( ( d - c ) ^ ( a - c ) );
          n.normalize();
          OsgTools::Triangles::Triangle::RefPtr t ( _document->addTriangle ( a, c, d, n, false ) );
          t->original ( true );
          //_triangleSet->addTriangle ( a, c, d, n, false );
        }

        // Add second triangle.
        if ( false == close ( b[2], noDataValue ) )
        {
          Vec3 n ( ( b - d ) ^ ( a - d ) );
          n.normalize();
          OsgTools::Triangles::Triangle::RefPtr t ( _document->addTriangle ( a, d, b, n, false ) );
          t->original ( true );
          //_triangleSet->addTriangle ( a, d, b, n, false );
        }
      }

    }
    //std::cout << "\rPercent Complete: " << i << "/" << static_cast<float>( numPoints / gridSize[0] ) <<  std::flush;
    // Feedback.
      this->_incrementProgress ( update(), static_cast<float>( i ), static_cast<float>( numPoints / stopC ) );
 
    // Swap rows.
    //row0.swap ( row1 );
  }

}



///////////////////////////////////////////////////////////////////////////////
//
//  Load an image file to use as a texture file to the model.
//  An actual texture object is not created.  Instead, we use
//  the color information contained in the image to set the 
//  corresponding color at a given point in the terrain file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_loadTexture ( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  // Get interface to triangle set for loading a color file
  Usul::Interfaces::ILoadColorFile::QueryPtr colorFile ( _document );

  // Create header vector from header information
  std::vector< float > header ( 0 );

  header.push_back( _header.rows );
  header.push_back( _header.cols );
  header.push_back( _header.west );
  header.push_back( _header.south );
  header.push_back( _header.ns_resol );
  
  colorFile->loadColorFile( filename, header );

     
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderGrassRaster::_stats()
{
  std::cout << "Number of triangles in the model: " << _document->numTriangles() << std::endl;
}



/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


