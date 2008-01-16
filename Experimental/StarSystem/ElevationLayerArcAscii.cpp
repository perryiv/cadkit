
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/ElevationLayerArcAscii.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Math/Interpolate.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "ossim/imaging/ossimImageData.h"
#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimProjection.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimUtmProjection.h"
#include "ossim/support_data/ossimDemGrid.h"

#include <fstream>

using namespace StarSystem;

USUL_FACTORY_REGISTER_CREATOR ( ElevationLayerArcAscii );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerArcAscii::ElevationLayerArcAscii() : 
  BaseClass(),
  _loaded ( false ),
  _grid( 0x0 ),
  _projection ( 0x0 ),
  _gridResolution ( 0, 0 ),
  _lowerLeft ( 0, 0 ),
  _cellSize ( 0 ),
  _noDataDouble ( 0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerArcAscii::~ElevationLayerArcAscii()
{
  USUL_TRACE_SCOPE;

  //if ( 0x0 != _grid )
  //  delete _grid;
  //_grid = 0x0;
  
  if ( 0x0 != _projection )
    delete _projection;
  _projection = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ElevationLayerArcAscii::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new ElevationLayerArcAscii ( *this ) );
  return clone.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for reading a stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
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

///////////////////////////////////////////////////////////////////////////////
//
//  Open the grid.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerArcAscii::open( const std::string& filename )
{

  {
    
    Guard guard ( this );
    //_grid = new ossimDemGrid;
    //_grid->read(in, false);
    
    this->_read( filename );
    
  }
  
#if 0
  // See if there is a .geom file.
  const std::string directory ( Usul::File::directory ( filename, false ) );
  const std::string basename ( Usul::File::base ( filename ) );
  
  ossimKeywordlist kwl;
  kwl.add( ossimKeywordNames::GEOM_FILE_KW, /*filename.c_str()*/ std::string ( directory + "/" + basename + ".geom" ).c_str() );
  
  Guard guard ( this );
  _projection = ossimProjectionFactoryRegistry::instance()->createProjection ( kwl );
#else
  ossimUtmProjection *projection = new ossimUtmProjection ( *(ossimDatumFactory::instance()->
                                                              create(ossimString("NAS-C"))->
                                                              ellipsoid()) );
  projection->setDatum(ossimDatumFactory::instance()->
                 create(ossimString("NAS-C")));
  projection->setZone(11);
  projection->setHemisphere('N');
  _projection = projection;
#endif
  if ( 0x0 == _projection )
    return;
  
  //ossimDemHeader header ( _grid->getHeader() );
  //ossimDemPointVector corners ( header.getDEMCorners() );


  Usul::Math::Vec2d upperRight ( _lowerLeft[0] + ( _gridResolution[0] * _cellSize ),
                                 _lowerLeft[1] + ( _gridResolution[1] * _cellSize ) );

  //provide double precision boundary points
  ossimGpt ll ( _projection->inverse ( ossimDpt ( _lowerLeft[0], _lowerLeft[1] ) ) );
  ossimGpt ur ( _projection->inverse ( ossimDpt ( upperRight[0], upperRight[1] ) ) );
  
  Extents extents ( ll.lon, ll.lat, ur.lon, ur.lat );
  this->extents ( extents );
  
  _loaded = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the ascii file and create a grid structure.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerArcAscii::_read( const std::string& filename )
{  
  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  in.open ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1099205557: Failed to open file: " + filename );

  // Initialize local vaiables.
    Usul::Math::Vec2ui gridResolution ( 0, 0 );
    Usul::Math::Vec2d lowerLeft ( 0, 0 );
    double cellSize ( 0 );
    double noDataDouble ( 0 );
  
    //Read the header of the arc ascii grid
    {
      std::string label;
      in >> label >> gridResolution[0];  // Number of columns
      in >> label >> gridResolution[1];  // Number of rows
      in >> label >> lowerLeft[0]; // Lower left x coordinate
      in >> label >> lowerLeft[1]; // Lower left y coordinate
      in >> label >> cellSize;
      in >> label >> noDataDouble;

      _gridResolution = gridResolution;
      _lowerLeft = lowerLeft;
      _cellSize = cellSize;
      _noDataDouble = noDataDouble;
    }

  // Make sure nodata value is an integer.
  const int noDataInt ( static_cast < int > ( noDataDouble ) );
  USUL_ASSERT ( noDataDouble == static_cast < double > ( noDataInt ) );

  // Useful typedefs.
  typedef std::vector<double> Values;
  
  // More local variables.
  const unsigned int numPoints ( gridResolution[0] * gridResolution[1] );

  // Allocate a row.
  Values row0 ( gridResolution[1] );

  //Usul::Policies::TimeBased update ( 1000 ); // Every second.
  //// Set the progress numbers.
  //_progress.first = 0;
  //_progress.second = numPoints;

  // Feedback.
  std::cout << Usul::Strings::format ( "Grid Size X: ", gridResolution[1], ", Y: ", gridResolution[0], ", Total: ", numPoints, '\n',
                                       "Lower Left: ", lowerLeft[0], ", ", lowerLeft[1], '\n', 
                                       "Cell Size: ", cellSize, '\n',
                                       "No Data: ", noDataInt ) << std::endl;

  std::cout << Usul::Strings::format ( "Reading rows from: ", filename, " ..." ) << std::endl;

  
  // Loop until we've read all the points.
  const unsigned int stopR ( gridResolution[0] - 1 );
  for ( unsigned int i = 0; i < stopR; ++i )
  {
    // Grab the next row.
    std::for_each ( row0.begin(), row0.end(), Helper::ReadStream ( filename, in ) );
      
    _grid.push_back( row0 );
    
    
  }
 
  // Close the input file.
  in.close();
    
  

}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerArcAscii::ImagePtr ElevationLayerArcAscii::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // Make the result.
  osg::ref_ptr < osg::Image > result ( this->_createBlankImage( width, height ) );
  
  Guard guard ( this );

  if ( 0x0 != _projection && true == _loaded )
  {
    // The upper left corner.
    ossimGpt ul ( extents.maximum()[1], extents.minimum()[0] );
    ossimDpt point ( _projection->forward( ul ) );

#if 0
    const double deltaX ( extents.maximum()[0] - extents.minimum()[0] );
    const double deltaY ( extents.maximum()[1] - extents.minimum()[1] );

    for ( unsigned int i = 0; i < height; ++i )
    {
      const double v ( static_cast<double> ( i ) / ( height - 1 ) );
      for ( unsigned int j = 0; j < width; ++j )
      {
        const double u ( static_cast<double> ( j ) / ( width - 1 ) );
        
        // Calculate current lat, lon.
        const double longitude ( extents.minimum()[0] + ( u * deltaX ) );
        const double latitude ( extents.maximum()[1] - ( v * deltaY ) );
#else
        // Shortcuts.
        const Extents::Vertex &mn ( extents.minimum() );
        const Extents::Vertex &mx ( extents.maximum() );

        for ( int i = height - 1; i >= 0; --i )
        {
          const double u ( 1.0 - static_cast<double> ( i ) / ( height - 1 ) );
          for ( unsigned int j = 0; j < width; ++j )
          {
            const double v ( static_cast<double> ( j ) / ( width - 1 ) );
            const double longitude ( mn[0] + u * ( mx[0] - mn[0] ) );
            const double latitude ( mn[1] + v * ( mx[1] - mn[1] ) );
#endif
        // Get the elevation data.
        float elevation ( static_cast < float > ( this->value ( longitude, latitude ) ) );
        //
        if ( static_cast< float > ( _noDataDouble ) == elevation )
          elevation = 0.0;
        
        *reinterpret_cast < float* > ( result->data ( i, j ) ) = elevation;
      }
    }
  }
  
  // Return our result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a blank image.
//
///////////////////////////////////////////////////////////////////////////////
/*
osg::Image* ElevationLayerArcAscii::_createBlankImage( unsigned int width, unsigned int height ) const
{
  osg::ref_ptr < osg::Image > result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_LUMINANCE, GL_FLOAT );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );

  return result.release();
}

*/
// Get the value at the lat, lon location.  May return null pixel value.
double ElevationLayerArcAscii::value ( double lon, double lat ) const
{
  USUL_TRACE_SCOPE;
  
  ossimGpt ul ( lat, lon );
  ossimDpt point ( _projection->forward( ul ) );
  
  // return no data value if the query region is outside the bounds of the arc ascii file
  if( point.x < _lowerLeft[0] ||
      point.y < _lowerLeft[1] ||
      point.x > ( _lowerLeft[0] + ( _gridResolution[1] * _cellSize ) ) ||
      point.y > ( _lowerLeft[1] + ( _gridResolution[0] * _cellSize ) ) )
  {
    return _noDataDouble;
  }
      
  // ossimDemHeader header ( _grid->getHeader() );
  
  const double nullValue ( _noDataDouble );
  
  double northWestX ( _lowerLeft[0] ), northWestY ( _lowerLeft[1] + ( _gridResolution[0] * _cellSize ) );
  
  // Y is the row number, x is the column number.
  const double x ( ( point.x - northWestX ) / _cellSize );
  const double y ( ( northWestY - point.y ) / _cellSize );
  
  const long i ( static_cast <long> ( y ) );
  const long j ( static_cast <long> ( x ) );
  
  //double a ( _grid->getElevation( i,     j     ) );
  //double b ( _grid->getElevation( i + 1, j     ) );
  //double c ( _grid->getElevation( i,     j + 1 ) );
  //double d ( _grid->getElevation( i + 1, j + 1 ) );

  double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
  try
  {
    // 
    if( i < static_cast< long > ( _gridResolution[0] - 1 ) &&
        j < static_cast< long > ( _gridResolution[1] - 1 ) )
    {
      a = ( _grid.at( i ).at( j ) );
      b = ( _grid.at( i + 1 ).at( j ) );
      c = ( _grid.at( i ).at( j + 1 ) );
      d = ( _grid.at( i + 1 ).at( j  + 1) );
    }
   
  
  }
  catch( ... )
  {
    std::cout << "Error caught when accessing the grid" << std::cout;
  }
  Usul::Predicates::Tolerance<double,double> tolerance ( 5 );
  
#if 0
  if ( tolerance ( nullValue, a ) )
    a = 0.0;
  if ( tolerance ( nullValue, b ) )
    b = 0.0;
  if ( tolerance ( nullValue, c ) )
    c = 0.0;
  if ( tolerance ( nullValue, d ) )
    d = 0.0;
#else
  if( static_cast< int > ( a ) == static_cast< int > ( _noDataDouble ) )
    a = 0.0;
  if( static_cast< int > ( b ) == static_cast< int > ( _noDataDouble ) )
    b = 0.0;
  if( static_cast< int > ( c ) == static_cast< int > ( _noDataDouble ) )
    c = 0.0;
  if( static_cast< int > ( d ) == static_cast< int > ( _noDataDouble ) )
    d = 0.0;
  
#endif

  const double u ( x - static_cast < double > ( j ) );
  const double v ( y - static_cast < double > ( i ) );
  
  return Usul::Math::Interpolate<double>::bilinear ( u, v, a, b, c, d );
}
