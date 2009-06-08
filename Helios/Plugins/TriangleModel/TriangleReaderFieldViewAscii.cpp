#include "TriangleReaderFieldViewAscii.h"
#include "Usul/Interfaces/IColorsPerVertex.h"
#include "Usul/Interfaces/IVertices.h"


#include "Usul/Math/MinMax.h"
#include "Usul/Policies/Update.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Adaptors/MemberFunction.h"

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
#if 1
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
#else
  //skip the first line
  in.getline ( buffer, bufSize );
#endif
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

  //Feedback
  std::cout << "Reading vertices..." << std::endl;  

  //read in the 1st value so we can set the initial color range values
    double x = 0, y = 0, z = 0, s = 0;
#if 1
    in >> x >> y >> z >> s; 
#else
    double temp;
    //in >> x >> y >> z >> temp >> s;
    in >> temp >> temp >> temp >> s >> x >> y >> z;
#endif
    in.getline ( buffer, bufSize );

    Usul::Math::Vec3d p( x, y, z );
    Usul::Math::Vec3d m( x, y, z * -1 );
    _values.push_back( s );
    _vertices.push_back( p );
    _mirrorVertices.push_back( m );
    
    //_colorRange[0] = s;
    _colorRange[0] = 11.323;
    _colorRange[1] = s;

  //assuming number of columns is 4
  for( unsigned int i = 1; i <  numPoints ; ++i )
  {

    // 1st point for the triangle
#if 1
    in >> x >> y >> z >> s; 
#else
    double temp;
    //in >> x >> y >> z >> temp >> s;
    in >> temp >> temp >> temp >> s >> x >> y >> z;
#endif
    in.getline ( buffer, bufSize );

    Usul::Math::Vec3d p( x, y, z );
    Usul::Math::Vec3d m( x, y, z * -1 );

    _values.push_back( s );
    _vertices.push_back( p );
    _mirrorVertices.push_back( m );

    _colorRange[0] = Usul::Math::minimum( _colorRange[0], s );
    _colorRange[1] = Usul::Math::maximum( _colorRange[1], s );
 
    // Feedback.
    this->_incrementProgress ( update() );

  }
  std::cout << "Color range is from " << _colorRange[0] << " to " <<_colorRange[1] << std::endl;
  //Skip the word GEOMETRY
  in.getline ( buffer, bufSize );

  //Get the number of faces
  unsigned int numFaces = 0;
  in >> numFaces;

  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numFaces;
  
  //Feedback
  std::cout << "Reading Faces..." << std::endl;

  //Get all the faces and store them.
  for( unsigned int i = 0; i < numFaces; ++i )
  {
    unsigned int numVertices = 0;

    in >> numVertices;
    Face face;
    face.push_back( numVertices );
    for( unsigned int j = 0; j < numVertices; ++j )
    {
      unsigned int p = 0;
      in >> p;
      face.push_back( p );
    }
    _faces.push_back( face );

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

void TriangleReaderFieldViewAscii::_createMirroredTriangle( unsigned int i, osg::Vec4Array * colors )
{
  #if 1
  double maxRange = _colorRange[1] - _colorRange[0];
  typedef TriangleDocument::Triangle* (TriangleDocument::*Function) ( SharedVertex *, SharedVertex *, SharedVertex *, const osg::Vec3f &, bool, bool );
  typedef Usul::Adaptors::MemberFunction < void, TriangleDocument*, Function > MemFun;

 
   
    // vertex 1
    osg::Vec3f v0 ( static_cast< float > ( _mirrorVertices.at( i )[0] ), 
                    static_cast< float > ( _mirrorVertices.at( i )[1] ),
                    static_cast< float > ( _mirrorVertices.at( i )[2] ) );
    
    SharedVertex *sv0 ( _document->addSharedVertex ( v0, true ) );
    
    // Color for vertex 1
    double value0 = _values.at( i ) - _colorRange[0];
    //Color4 c0 = this->_getInterpolatedColorValue( _values.at( i ) / _colorRange[1] );
    Color4 c0 = this->_getInterpolatedColorValue( value0 / maxRange );
    
    colors->at( sv0->index() ) = osg::Vec4d( c0[0] / 255, c0[1] / 255, c0[2] / 255, c0[3] );

    // vertex 2
    osg::Vec3f v1 ( static_cast< float > ( _mirrorVertices.at( i + 1 )[0] ), 
                    static_cast< float > ( _mirrorVertices.at( i + 1 )[1] ),
                    static_cast< float > ( _mirrorVertices.at( i + 1 )[2] ) );
    SharedVertex *sv1 ( _document->addSharedVertex ( v1, true ) );

    // Color for vertex 2
    double value1 = _values.at( i + 1 ) - _colorRange[0];
    //Color4 c1 = this->_getInterpolatedColorValue( _values.at( i + 1 ) / _colorRange[1] );
    Color4 c1 = this->_getInterpolatedColorValue( value1 / maxRange );
    
    colors->at ( sv1->index() ) = osg::Vec4d( c1[0] / 255, c1[1] / 255, c1[2] / 255, c1[3] );

    // vertex 3
    osg::Vec3f v2 ( static_cast< float > ( _mirrorVertices.at( i + 2 )[0] ), 
                    static_cast< float > ( _mirrorVertices.at( i + 2 )[1] ),
                    static_cast< float > ( _mirrorVertices.at( i + 2 )[2] ) );
    SharedVertex *sv2 ( _document->addSharedVertex ( v2, true ) );

    // Color for vertex 3
    double value2 = _values.at( i + 2 ) - _colorRange[0];
    //Color4 c2 = this->_getInterpolatedColorValue( _values.at( i + 2 ) / _colorRange[1] );
    Color4 c2 = this->_getInterpolatedColorValue( value2 / maxRange );
    
    colors->at ( sv2->index() ) = osg::Vec4d( c2[0] / 255 , c2[1] / 255 , c2[2] / 255, c2[3] );

    // ( p2 - p1 ) ^ ( p0 - p1 )
    osg::Vec3d n ( ( v2 - v1 ) ^ ( v0 - v1 ) );

    if( sv0 != sv1 && sv0 != sv2 && sv1 != sv2 )
    {
      //_document->addTriangle( sv0, sv1, sv2, n, false, false );
      Usul::Functions::safeCallV1V2V3V4V5V6( MemFun ( _document.get(), &TriangleDocument::addTriangle ), sv0, sv1, sv2, n, false, false, "" );

    }

#endif
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
 
  typedef TriangleDocument::Triangle* (TriangleDocument::*Function) ( SharedVertex *, SharedVertex *, SharedVertex *, const osg::Vec3f &, bool, bool );
  typedef Usul::Adaptors::MemberFunction < void, TriangleDocument*, Function > MemFun;

  //feedback
  std::cout << "Setting color information..." << std::endl;
  _progress.first = 0;
  _progress.second = _values.size() / 3;
  Usul::Policies::TimeBased update ( 1000 ); // Every second.
#if 1
  double maxRange = _colorRange[1] - _colorRange[0];
  for( unsigned int i = 0; i < _vertices.size(); i += 3 )
  {
   
    // vertex 1
    osg::Vec3f v2 ( static_cast< float > ( _vertices.at( i )[0] ), 
                    static_cast< float > ( _vertices.at( i )[1] ),
                    static_cast< float > ( _vertices.at( i )[2] ) );
    
    SharedVertex *sv2 ( _document->addSharedVertex ( v2, true ) );
    
    // Color for vertex 1
    double value0 = _values.at( i ) - _colorRange[0];
    //Color4 c0 = this->_getInterpolatedColorValue( _values.at( i ) / _colorRange[1] );
    Color4 c0 = this->_getInterpolatedColorValue( value0 / maxRange );
    colors->at( sv2->index() ) = osg::Vec4d( c0[0] / 255, c0[1] / 255, c0[2] / 255, c0[3] );

    // vertex 2
    osg::Vec3f v1 ( static_cast< float > ( _vertices.at( i + 1 )[0] ), 
                    static_cast< float > ( _vertices.at( i + 1 )[1] ),
                    static_cast< float > ( _vertices.at( i + 1 )[2] ) );
    SharedVertex *sv1 ( _document->addSharedVertex ( v1, true ) );

    // Color for vertex 2
    double value1 = _values.at( i + 1 ) - _colorRange[0];
    //Color4 c1 = this->_getInterpolatedColorValue( _values.at( i + 1 ) / _colorRange[1] );
    Color4 c1 = this->_getInterpolatedColorValue( value1 / maxRange );
    colors->at ( sv1->index() ) = osg::Vec4d( c1[0] / 255, c1[1] / 255, c1[2] / 255, c1[3] );

    // vertex 3
    osg::Vec3f v0 ( static_cast< float > ( _vertices.at( i + 2 )[0] ), 
                    static_cast< float > ( _vertices.at( i + 2 )[1] ),
                    static_cast< float > ( _vertices.at( i + 2 )[2] ) );
    SharedVertex *sv0 ( _document->addSharedVertex ( v0, true ) );

    // Color for vertex 3
    double value2 = _values.at( i + 2 ) - _colorRange[0];
    //Color4 c2 = this->_getInterpolatedColorValue( _values.at( i + 2 ) / _colorRange[1] );
    Color4 c2 = this->_getInterpolatedColorValue( value2 / maxRange );
    colors->at ( sv0->index() ) = osg::Vec4d( c2[0] / 255 , c2[1] / 255 , c2[2] / 255, c2[3] );

    // ( p2 - p1 ) ^ ( p0 - p1 )
    osg::Vec3d n ( ( v2 - v1 ) ^ ( v0 - v1 ) );

    if( sv0 != sv1 && sv0 != sv2 && sv1 != sv2 )
    {
      //_document->addTriangle( sv0, sv1, sv2, n, false, false );
      Usul::Functions::safeCallV1V2V3V4V5V6( MemFun ( _document.get(), &TriangleDocument::addTriangle ), sv0, sv1, sv2, n, false, false, "" );

    }
    this->_createMirroredTriangle( i, colors.get() );

    

    // Feedback.
    this->_incrementProgress ( update() );

  }
#else
  try{
  //Loop through the faces
  for( unsigned int i = 0; i < _faces.size(); ++i )
  {
        
    switch( _faces.at( i )[0] )
    {
      case 3:
      {
        // vertex 1
        osg::Vec3f v2 ( static_cast< float > ( _vertices.at( _faces.at ( i )[1] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[1] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[1] )[2] ) );
        
        SharedVertex *sv2 ( _document->addSharedVertex ( v2, true ) );

        // Color for vertex 1
        Color4 c0 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[1] ) / _colorRange[1] );
        colors->at( sv2->index() ) = osg::Vec4d( c0[0] / 255, c0[1] / 255, c0[2] / 255, c0[3] );

        // vertex 2
        osg::Vec3f v1 ( static_cast< float > ( _vertices.at( _faces.at ( i )[2] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[2] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[2] )[2] ) );
        SharedVertex *sv1 ( _document->addSharedVertex ( v1, true ) );

        // Color for vertex 2
        Color4 c1 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[2] ) / _colorRange[1] );
        colors->at ( sv1->index() ) = osg::Vec4d( c1[0] / 255, c1[1] / 255, c1[2] / 255, c1[3] );

        // vertex 3
        osg::Vec3f v0 ( static_cast< float > ( _vertices.at( _faces.at ( i )[3] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[3] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[3] )[2] ) );
        SharedVertex *sv0 ( _document->addSharedVertex ( v0, true ) );

        // Color for vertex 3
        Color4 c2 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[3] ) / _colorRange[1] );
        colors->at ( sv0->index() ) = osg::Vec4d( c2[0] / 255 , c2[1] / 255 , c2[2] / 255, c2[3] );

        // ( p2 - p1 ) ^ ( p0 - p1 )
        osg::Vec3d n ( ( v2 - v1 ) ^ ( v0 - v1 ) );
        
        if( sv0 != sv1 && sv0 != sv2 && sv1 != sv2 )
        {
          //_document->addTriangle( sv0, sv1, sv2, n, false, false );
          Usul::Functions::safeCallV1V2V3V4V5V6( MemFun ( _document.get(), &TriangleDocument::addTriangle ), sv0, sv1, sv2, n, false, false, "" );

        }

        // Feedback.
        this->_incrementProgress ( update() );

    
      }
      break;
      // 4 vertices case.  Seems to break the read.
      // Setting to 0 to remove all polys with 4 vertices.
      case 0:
      {
        
        // vertex 4
        osg::Vec3f v3 ( static_cast< float > ( _vertices.at( _faces.at ( i )[1] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[1] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[1] )[2] ) );
        
        SharedVertex *sv3 ( _document->addSharedVertex ( v3, true ) );
        
        // Color for vertex 4
        Color4 c3 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[1] ) / _colorRange[1] );
        colors->at( sv3->index() ) = osg::Vec4d( c3[0] / 255, c3[1] / 255, c3[2] / 255, c3[3] );

        // vertex 3
        osg::Vec3f v2 ( static_cast< float > ( _vertices.at( _faces.at ( i )[2] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[2] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[2] )[2] ) );
        
        SharedVertex *sv2 ( _document->addSharedVertex ( v2, true ) );

        // Color for vertex 3
        Color4 c0 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[2] ) / _colorRange[1] );
        colors->at( sv2->index() ) = osg::Vec4d( c0[0] / 255, c0[1] / 255, c0[2] / 255, c0[3] );

        // vertex 2
        osg::Vec3f v1 ( static_cast< float > ( _vertices.at( _faces.at ( i )[3] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[3] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[3] )[2] ) );
        SharedVertex *sv1 ( _document->addSharedVertex ( v1, true ) );

        // Color for vertex 2
        Color4 c1 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[3] ) / _colorRange[1] );
        colors->at ( sv1->index() ) = osg::Vec4d( c1[0] / 255, c1[1] / 255, c1[2] / 255, c1[3] );

        // vertex 1
        osg::Vec3f v0 ( static_cast< float > ( _vertices.at( _faces.at ( i )[4] )[0] ), 
                        static_cast< float > ( _vertices.at( _faces.at ( i )[4] )[1] ),
                        static_cast< float > ( _vertices.at( _faces.at ( i )[4] )[2] ) );
        SharedVertex *sv0 ( _document->addSharedVertex ( v0, true ) );

        // Color for vertex 3
        Color4 c2 = this->_getInterpolatedColorValue( _values.at( _faces.at ( i )[4] ) / _colorRange[1] );
        colors->at ( sv0->index() ) = osg::Vec4d( c2[0] / 255 , c2[1] / 255 , c2[2] / 255, c2[3] );

        // T 0 1 2 ( p2 - p1 ) ^ ( p0 - p1 )
        osg::Vec3d n0 ( ( v2 - v1 ) ^ ( v0 - v1 ) );
        if( sv0 != sv1 && sv0 != sv2 && sv1 != sv2 )
        {          
          Usul::Functions::safeCallV1V2V3V4V5V6( MemFun ( _document.get(), &TriangleDocument::addTriangle ), sv0, sv1, sv2, n0, false, false, "" );
        }

        // T 0 2 3 ( p2 - p1 ) ^ ( p0 - p1 )
        osg::Vec3d n1 ( ( v3 - v2 ) ^ ( v0 - v2 ) );
        if( sv0 != sv2 && sv0 != sv3 && sv2 != sv3 )
        {}
        else
        {
          //_document->addTriangle( sv0, sv2, sv3, n1, false, false );
          Usul::Functions::safeCallV1V2V3V4V5V6( MemFun ( _document.get(), &TriangleDocument::addTriangle ), sv0, sv2, sv3, n0, false, false, "" );

        }


        // Feedback.
        this->_incrementProgress ( update() );

      }
      break;

      default:
      break;

    }
    
  }
  }
    catch( ... )
    {}
#endif
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


