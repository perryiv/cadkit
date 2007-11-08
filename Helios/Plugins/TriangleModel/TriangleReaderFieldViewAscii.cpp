#include "TriangleReaderFieldViewAscii.h"
#include "Usul/Interfaces/IColorsPerVertex.h"
#include "Usul/Interfaces/IVertices.h"


#include "Usul/Math/MinMax.h"
#include "Usul/Policies/Update.h"
#include "Usul/File/Path.h"

#include <fstream>
#include <iostream>

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangleReaderFieldViewAscii, TriangleReaderFieldViewAscii::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderFieldViewAscii::TriangleReaderFieldViewAscii( const std::string &file, IUnknown *caller, TriangleDocument *doc ):
_root         ( new osg::Group ),
_colorTable   ( 0 ),
_values       ( 0 ),
_document     ( doc ),
_colorRange   ( 0, 0 ),
_filename     ( file ),
_caller       ( caller ),
_progress     ( 0, 1 )
{
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderFieldViewAscii::~TriangleReaderFieldViewAscii()
{
	// Need to do this because of circular references. Otherwise there are leaks.
  this->clear();
}

/////////////////
//  PUBLIC
/////////////////



///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderFieldViewAscii::operator()()
{
  // Set initial progress and range.
  _document->setProgressBar ( true, 0, 100, _caller );

  // Clear accumulated state.
  this->clear();

  // Load the color table
  this->_readColorTable( _filename );

  // Read the file.
  this->_read( _filename );

  // Create the triangle set with colors
  this->_createTriangeSet();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Note: we do not clear the reference-counted objects because 
//  we hand those to the document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderFieldViewAscii::clear()
{
}


/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderFieldViewAscii::_read( const std::string & filename )
{
  std::ifstream in;
  in.open( filename.c_str() );
  std::string temp;
  const unsigned int bufSize ( 2048 ); // should be big enough
  char buffer[bufSize];

  //trim header information

  //skip the first line
  in.getline ( buffer, bufSize );
  unsigned int numsets = 0;
  in >> numsets;
  in.getline ( buffer, bufSize );
  //skip the object poly counts
  for( unsigned int i = 0; i < numsets; ++i )
  {
    in.getline ( buffer, bufSize );
  }

  //get the number of points
  unsigned int numPoints = 0;
  in >> numPoints;
  in.getline ( buffer, bufSize );
  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numPoints;

  Usul::Policies::TimeBased update ( 1000 ); // Every second.
  
  // skip the column header
  in.getline ( buffer, bufSize );
  
  //assuming number of columns is 4
  for( unsigned int i = 0; i <  numPoints ; ++i )
  {
    double x = 0, y = 0, z = 0, s = 0;

    // 1st point for the triangle
    in >> x >> y >> z >> s;
    in.getline ( buffer, bufSize );

    Usul::Math::Vec3d p( x, y, z );
    _values.push_back( s );
    _vertices.push_back( p );

    _colorRange[0] = Usul::Math::minimum( _colorRange[0], s );
    _colorRange[1] = Usul::Math::maximum( _colorRange[1], s );
 
    // Feedback.
    this->_incrementProgress ( update() );

  }
 
  in.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderFieldViewAscii::_createTriangeSet()
{
  // Get color array
  Usul::Interfaces::IColorsPerVertex::QueryPtr colorsV ( _document );
  osg::ref_ptr< osg::Vec4Array > colors = colorsV->getColorsV ( true );
 
  //feedback
  std::cout << "Setting color information..." << std::endl;
  _progress.first = 0;
  _progress.second = _values.size() / 3;
  Usul::Policies::TimeBased update ( 1000 ); // Every second.
  
  for( unsigned int i = 0; i < _vertices.size(); i += 3 )
  {
   
    // vertex 1
    osg::Vec3f v2 ( static_cast< float > ( _vertices.at( i )[0] ), 
                    static_cast< float > ( _vertices.at( i )[1] ),
                    static_cast< float > ( _vertices.at( i )[2] ) );
    
    SharedVertex *sv2 ( _document->addSharedVertex ( v2, true ) );
    
    // Color for vertex 1
    Color4 c0 = this->_getInterpolatedColorValue( _values.at( i ) / _colorRange[1] );
    colors->at( sv2->index() ) = osg::Vec4d( c0[0] / 255, c0[1] / 255, c0[2] / 255, c0[3] );

    // vertex 2
    osg::Vec3f v1 ( static_cast< float > ( _vertices.at( i + 1 )[0] ), 
                    static_cast< float > ( _vertices.at( i + 1 )[1] ),
                    static_cast< float > ( _vertices.at( i + 1 )[2] ) );
    SharedVertex *sv1 ( _document->addSharedVertex ( v1, true ) );

    // Color for vertex 2
    Color4 c1 = this->_getInterpolatedColorValue( _values.at( i + 1 ) / _colorRange[1] );
    colors->at ( sv1->index() ) = osg::Vec4d( c1[0] / 255, c1[1] / 255, c1[2] / 255, c1[3] );

    // vertex 3
    osg::Vec3f v0 ( static_cast< float > ( _vertices.at( i + 2 )[0] ), 
                    static_cast< float > ( _vertices.at( i + 2 )[1] ),
                    static_cast< float > ( _vertices.at( i + 2 )[2] ) );
    SharedVertex *sv0 ( _document->addSharedVertex ( v0, true ) );

    // Color for vertex 3
    Color4 c2 = this->_getInterpolatedColorValue( _values.at( i + 2 ) / _colorRange[1] );
    colors->at ( sv0->index() ) = osg::Vec4d( c2[0] / 255 , c2[1] / 255 , c2[2] / 255, c2[3] );

    // ( p2 - p1 ) ^ ( p0 - p1 )
    osg::Vec3d n ( ( v2 - v1 ) ^ ( v0 - v1 ) );

    _document->addTriangle( sv0, sv1, sv2, n, false, true );

    // Feedback.
    this->_incrementProgress ( update() );

  }
  colorsV->dirtyColorsV ( false );
   

}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderFieldViewAscii::Color4 TriangleReaderFieldViewAscii::_getInterpolatedColorValue( double value )
{
  Color4 c( 0, 0, 0, 1 );

  double cvalue = value * 255;

  unsigned int minValue = static_cast< unsigned int > ( floor( cvalue ) );
  unsigned int maxValue = minValue + 1;

  minValue = Usul::Math::minimum( minValue, static_cast< unsigned int > ( 255 ) );
  maxValue = Usul::Math::minimum( maxValue, static_cast< unsigned int > ( 255 ) );

  
  Color4 cmin = _colorTable.at( minValue );
  Color4 cmax = _colorTable.at( maxValue );

  c = cmin + ( ( cmax - cmin ) * 0.5 );
 
  return c;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderFieldViewAscii::_readColorTable( const std::string & filename )
{

  std::string directory = Usul::File::directory( filename, true );
  std::string basename = Usul::File::base( filename );

  std::string file = directory + basename + ".color";

  std::ifstream in;
  in.open( file.c_str() );
  _colorTable.resize( 256 );
  for( unsigned int i = 0; i < 256; i ++ )
  {
    double temp;
    char comma;

    in >> temp >> comma
       >> _colorTable.at( i )[0] >> comma 
       >> _colorTable.at( i )[1] >> comma
       >> _colorTable.at( i )[2] >> comma
       >> _colorTable.at( i )[3];
  }
  in.close();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderFieldViewAscii::_incrementProgress ( bool state )
{
  
  
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  // Debugging in Cadviewer
  //std::cout << "\rProgress: " << numerator << "/" << denominator << " ( " << static_cast< double > ( numerator / denominator ) << " % )" << std::endl;
  _document->setProgressBar ( state, numerator, denominator, _caller );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}


/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


