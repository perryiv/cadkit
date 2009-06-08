
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __linux
#define __USE_LARGEFILE64
#endif

#include "PointDocument.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Find.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Policies/Update.h"
#include "Usul/Strings/Format.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Interfaces/IColorEditor.h"
#include "Usul/Documents/Manager.h"

#include "OsgTools/State/StateSet.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <string>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PointDocument, PointDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( PointDocument );


#ifdef _MSC_VER
#define FILE_TELL_64 _ftelli64
#elif __APPLE__
#define FILE_TELL_64 ftello
#else
#define FILE_TELL_64 ftello64
#endif

    
///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::PointDocument() : BaseClass ( "Point Document" ),
_pointSet( new PointSet() ),
_numPoints( 0 ),
_material( new osg::Material() ),
_color( 1.0f, 0.0f, 0.0f, 1.0f ),
_workingDir()
{
  // Set the default material ambient and diffuse
  _material->setAmbient( osg::Material::FRONT_AND_BACK, _color );
  _material->setDiffuse( osg::Material::FRONT_AND_BACK, _color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::~PointDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PointDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IBuildScene::IID:
      return static_cast < Usul::Interfaces::IBuildScene* > ( this );
    case Usul::Interfaces::IMenuAdd::IID:
      return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
    default:
      return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::canInsert ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "point3d" || ext == "p3dbf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "psxml" || ext == "point3d" || ext == "p3dbf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "p3dbf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the xml file and parse elements.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_parseXML( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "usetools" == iter->first )
    {
      
    }
  }

  Children& children ( node.children() );
	unsigned int set = 0;
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "set" == node->name() )
    {
     
    }

  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{

  // XML Parsing
  /*
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  _workingDir = Usul::File::directory( name, true );
  
  document->load ( name );
  if ( "mpd" == document->name() )
  {
    
    this->_readParameterFile( *document, caller, progress );
    this->updateGlobalEndtime();
  }
  */

  this->_read( name, caller, progress );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////


void PointDocument::_read ( const std::string &name, Unknown *caller, Unknown *progress )
{

  // Binary restart filename
  std::string restartFilename ( Usul::Strings::format ( Usul::File::directory( name, true ), Usul::File::base( name ), ".p3dbf" ) );

  // TODO:
  // Check to see if the binary restart file exists.  
  // If it does and the date is greater than the ASCII data
  // read that file instead.

  // Get the extension of the file to load
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "point3d" == ext )
  {
    // Temporary file name.
    const std::string binaryFilename ( Usul::File::directory( name, true ) + Usul::File::base( name ) + ".bp3d" );

    // Make sure it gets deleted if there is an exception.
    Usul::Scope::RemoveFile remove ( binaryFilename );

    // Read the header of the file to determine which path to take in parsing
    bool fastPath = this->_parseHeader( name, caller, progress );
   
    if( true == fastPath )
    {
      // Read ascii file and build binary "temp" file.
      this->_fastReadAndSetBounds( name, binaryFilename, caller, progress );
    }
    else
    {
      // Read ascii file and build binary "temp" file.
      this->_readAndSetBounds( name, binaryFilename, caller, progress  );
    }
 
    // Read binary file.
    this->_readPoint3DFile( binaryFilename, caller, progress );

    // Create the octree
    this->_split( caller, progress );

    // Build the vectors from the linked lists
    this->_buildVectors( caller, progress );

    // debug info
    std::cout << Usul::Strings::format( "Writing binary restart file: ", restartFilename ) << std::endl;

    // Write restart file
    this->write( restartFilename, caller, progress );

    // Everything worked so keep the binary file
    // remove.remove( false );
 
  }
  else if( "p3dbf" == ext )
  {
    // read the binary restart file
    this->_readBinaryRestartFile( name, caller, progress );
  }
  else
  {
    throw std::runtime_error("Error: 4144590656 Invalid file extension for Point Document:" + name);
  }

}
///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to the given file name.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  std::ofstream* ofs ( new std::ofstream );
  ofs->open( name.c_str(), std::ofstream::out | std::ifstream::binary ); 

  // write the binary restart file
  _pointSet->write( ofs, _numPoints, 0x0, caller, progress );

  // close the stream and clean up memory
  ofs->close();
  delete ofs;
  ofs = 0x0;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::clear ( Usul::Interfaces::IUnknown *caller )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::Filters PointDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "Ascii 3D Point Files (*.point3d )", "*.point3d" ) );
  filters.push_back ( Filter ( "Point Set Binary Restart Files (*.p3dbf )", "*.p3dbf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::Filters PointDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Ascii 3D Point Files (*.point3d )", "*.point3d" ) );
  filters.push_back ( Filter ( "Point Set Binary Restart Files (*.p3dbf )", "*.p3dbf" ) );
  filters.push_back ( Filter ( "Point Set Document XML (*.psxml )", "*.psxml" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::Filters PointDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Point Set Binary Restart Files (*.p3dbf )", "*.p3dbf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::Filters PointDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *PointDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  // Redirect to point set
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  group->addChild( _pointSet->buildScene ( caller ) );
  OsgTools::State::StateSet::setMaterial( group.get(), _material.get() );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_readPoint3DFile( const std::string &filename, Unknown *caller, Unknown *progress )
{
 
  this->setStatusBar ( "Step 1/2: Reading file...", progress );

  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char fileBuffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( fileBuffer, bufSize );
  in.open ( filename.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 4073898430: Failed to open file: " + filename );

  
  // Read binary header
  const std::string id ( "f2775490-ecf0-4362-8c81-7c1693e0736c" );
  std::vector<char> header ( id.size() );
  in.read ( &header[0], header.size() );
  const bool same ( id == std::string ( header.begin(), header.end() ) );
  if( false == same )
  {
    throw std::runtime_error ( "Error 2348749452: Invalid binary file format: " + filename );
   
  }

  // Get the min/max bounds and the number of points
  osg::Vec3f minCorner( 0.0, 0.0, 0.0 );
  in.read ( reinterpret_cast<char *> ( &minCorner ), sizeof ( osg::Vec3f ) );

  osg::Vec3f maxCorner( 0.0, 0.0, 0.0 );
  in.read ( reinterpret_cast<char *> ( &maxCorner ), sizeof ( osg::Vec3f ) );

  // Get the number of points
  in.read ( reinterpret_cast<char *> ( &_numPoints ), sizeof ( unsigned int ) );

  // Set the bounds of the pointset octree and the point capacity
  _pointSet->bounds( minCorner, maxCorner );
  unsigned int capacity = Usul::Math::minimum( static_cast< unsigned int > ( _numPoints / 400 ), static_cast< unsigned int > ( std::numeric_limits< short >::max() ) );
  _pointSet->capacity( capacity );
  
  // Setup progress bars
  Usul::Policies::TimeBased update ( 1000 ); // Every second.

  // Needed for progress.
  unsigned int count ( 0 );

  try
  {
    // Read the rest of the file.
    // If the x, y, or z header wasn't found then set
    // their corresponding values to 0  
    while( false == in.eof() )
    {
      osg::Vec3f value;

      in.read ( reinterpret_cast<char *> ( &value ), sizeof ( osg::Vec3f ) );
    
      if( false == _pointSet->addPoint( value ) )
      {
        std::cout << "Failed to insert point in PointDocument! " << std::endl;
      }

      ++count;

      // Feedback.
      this->setProgressBar( update(), _numPoints + count, _numPoints * 2, progress );
      
    }
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1302438923" );
  --count;
  // Give proper feedback.
  if ( count != _numPoints )
  {
    std::cout << Usul::Strings::format ( "Only loaded ", count, " of ", _numPoints, " points" ) << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::_parseHeader( const std::string &filename, Unknown *caller, Unknown *progress )
{
  this->setStatusBar ( "Step 1/2: Reading File...", progress );

  // Open a stream with a large buffer for ascii reading.
  const unsigned long int bufSize ( 4095 );
  std::ifstream in;
  in.open ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 3582771800: Failed to open file: " + filename );

  char buffer[bufSize+1];
  in.getline ( buffer, bufSize );
  
  std::string temp ( buffer );
  temp = temp.substr( 0, 5 );
  
  return ( temp == "x,y,z" );
  //return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_fastReadAndSetBounds( const std::string &filename, const std::string &binaryFilename, Unknown *caller, Unknown *progress )
{
  osg::BoundingBox bounds;

  this->setStatusBar ( "Step 1/2: Reading File...", progress );

  // Update progress bar every second.
  Usul::Policies::TimeBased update ( 1000 );

  // Get size of file.
  const Usul::Types::Uint64 fileSize ( Usul::File::size ( filename ) );

  // Open a stream with a large buffer for ascii reading.
  const unsigned long int bufSize ( 4095 );
  FILE* infile = fopen( filename.c_str(), "r" );
  
  if ( false == infile )
    throw std::runtime_error ( "Error 3521397700: Failed to open file: " + filename );

  // Open a stream with a large buffer for binary writing.
  std::ofstream out;
  out.open ( binaryFilename.c_str(), std::ofstream::out | std::ifstream::binary );
  if ( !out.is_open() )
    throw std::runtime_error ( "Error 1592984234: Failed to open file: " + binaryFilename );

  {
    // Write binary header
    const std::string id ( "f2775490-ecf0-4362-8c81-7c1693e0736c" );
    std::vector<char> header ( id.size() );
    out.write ( &header[0], header.size() );

    // reserve space for the min/max bounds and the number of points
    std::vector<char> space ( sizeof ( osg::Vec3f ) * 2 + sizeof ( unsigned int ) );
    out.write ( &space[0], space.size() );
  }

  //read and discard the first line in the file
  std::vector< char > rest( 1024 );
  fscanf( infile, "%s\n", &rest[0] );
  
  // Read the the file.
  std::cout << filename << " size is: " << fileSize << std::endl;
    
  bool keepGoing ( true );
  while ( keepGoing )
  {
    #ifdef _DEBUG
    rest.assign ( rest.size(), '\0' );
    #endif

    double x = 0.0f, y = 0.0f, z = 0.0f;
    fscanf( infile, "%lf,%lf,%lf,%s\n", &x, &y, &z, &rest[0] );

    osg::Vec3f p( x, y, z );
      
    // update the boundary information
    bounds.expandBy( p );

    out.write ( reinterpret_cast<char *> ( &p ), sizeof ( osg::Vec3f ) ); 

    ++_numPoints;

    // This is how far we've gone so far.
    const Usul::Types::Uint64 bytesRead ( static_cast < Usul::Types::Uint64 > ( FILE_TELL_64 ( infile ) ) );
    const double fraction ( static_cast<double> ( bytesRead ) / static_cast<double> ( fileSize * 2 ) );

    // Feedback.
    this->setProgressBar ( update(), static_cast<float> ( fraction ), progress );

    // Set loop variable.
    keepGoing = ( static_cast < Usul::Types::Uint64 > ( FILE_TELL_64 ( infile ) + 2 ) < fileSize );
    if ( false == keepGoing )
    {
      std::cout << "Done" << std::endl;
      std::string doubleValues ( Usul::Convert::Type< double, std::string >::convert ( x ) + " " +
                                 Usul::Convert::Type< double, std::string >::convert ( y ) + " " +
                                 Usul::Convert::Type< double, std::string >::convert ( z ) );

      std::string floatValues (  Usul::Convert::Type< float, std::string >::convert ( p.x() ) + " " +
                                 Usul::Convert::Type< float, std::string >::convert ( p.y() ) + " " +
                                 Usul::Convert::Type< float, std::string >::convert ( p.z() ) );
    }
  }
  
  // Close the input file
  fclose( infile );

  // Min and max bounds of the data set
  osg::Vec3f minCorner ( bounds.xMin(), bounds.yMin(), bounds.zMin() );
  osg::Vec3f maxCorner ( bounds.xMax(), bounds.yMax(), bounds.zMax() );
  
  // Go back to the beginning file and fill in the min/max and num points information
  out.seekp ( 0, std::ios_base::beg );

  {
    // Write binary header
    const std::string id ( "f2775490-ecf0-4362-8c81-7c1693e0736c" );
    out.write ( &id.c_str()[0], id.size() );

    // Write the min/max bounds and the number of points
    out.write ( reinterpret_cast<char *> ( &minCorner ), sizeof ( osg::Vec3f ) );  
    out.write ( reinterpret_cast<char *> ( &maxCorner ), sizeof ( osg::Vec3f ) );
    out.write ( reinterpret_cast<char *> ( &_numPoints ), sizeof ( unsigned int ) );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_readAndSetBounds( const std::string &filename, const std::string &binaryFilename, Unknown *caller, Unknown *progress )
{
  osg::BoundingBox bounds;

  this->setStatusBar ( "Step 1/2: Reading File...", progress );

  // Update progress bar every second.
  Usul::Policies::TimeBased update ( 1000 );

  // Get size of file.
  const unsigned int fileSize ( Usul::File::size ( filename ) );

  // Open a stream with a large buffer for ascii reading.
  const unsigned long int bufSize ( 4095 );
  std::ifstream in;
  in.open ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 3582771800: Failed to open file: " + filename );

  // Open a stream with a large buffer for binary writing.
  std::ofstream out;
  out.open ( binaryFilename.c_str(), std::ofstream::out | std::ifstream::binary );
  if ( !out.is_open() )
    throw std::runtime_error ( "Error 3697190559: Failed to open file: " + binaryFilename );

  {
    // Write binary header
    const std::string id ( "f2775490-ecf0-4362-8c81-7c1693e0736c" );
    std::vector<char> header ( id.size() );
    out.write ( &header[0], header.size() );
    const bool same ( id == std::string ( header.begin(), header.end() ) );

    // reserve space for the min/max bounds and the number of points
    std::vector<char> space ( sizeof ( osg::Vec3f ) * 2 + sizeof ( unsigned int ) );
    out.write ( &space[0], space.size() );
  }

  // Get the first line.  This should be the header of the file.
  // Check to make sure the first line is the header.
  // Check to see which column is X, Y, and Z
  unsigned int x_index = 0, y_index = 0, z_index = 0;
  
  bool foundX = false, foundY = false, foundZ = false;
  {
    char buffer[bufSize+1];
    in.getline ( buffer, bufSize );
    std::string temp ( buffer );
    temp = Usul::Strings::lowerCase( temp );

    std::replace( temp.begin(), temp.end(), ' ', ',' );
    
    typedef std::vector< std::string > StringVec;
    StringVec sv;
    Usul::Strings::split( temp, ",", false, sv );
    
    for( unsigned int i = 0; i < sv.size(); ++i )
    {
      if( "x" == sv.at( i ) )
      {
        x_index = i;
        foundX = true;
      }
      if( "y" == sv.at( i ) )
      {
        y_index = i;
        foundY = true;
      }
      if( "z" == sv.at( i ) )
      {
        z_index = i;
        foundZ = true;
      }
    }
  }
  
  // If we didn't find x, y, and z, punt.
  if ( ( false == foundX ) || ( false == foundY ) || ( false == foundZ ) )
  {
    throw std::runtime_error ( "Error 4396772640: Failed to find x, y, and z columns in file: " + filename );
  }

  // Read the rest of the file.
  // If the x, y, or z header wasn't found then set
  // their corresponding values to 0  
  while( EOF != in.peek() )
  {
    char buffer[bufSize+1];
    in.getline ( buffer, bufSize );
    std::string temp ( buffer );
    temp = Usul::Strings::lowerCase( temp );

    std::replace( temp.begin(), temp.end(), ' ', ',' );
    
    typedef std::vector< std::string > StringVec;
    StringVec sv;
    Usul::Strings::split( temp, ",", false, sv );
    if( sv.size() > 0 )
    {
      osg::Vec3f p ( 0.0, 0.0, 0.0 );
      if( true == foundX )
      {
        p.x() = Usul::Convert::Type< std::string, float >::convert( sv.at( x_index ) );
      }
      if( true == foundY )
      {
        p.y() = Usul::Convert::Type< std::string, float >::convert( sv.at( y_index ) );
      }
      if( true == foundZ )
      {
        p.z() = Usul::Convert::Type< std::string, float >::convert( sv.at( z_index ) );
      }
      
      // update the boundary information
      bounds.expandBy( p );

      out.write ( reinterpret_cast<char *> ( &p ), sizeof ( osg::Vec3f ) ); 
      
    }
    ++_numPoints;

    // Feedback.
    this->setProgressBar ( update(), in, fileSize * 2, progress );
  }


#if 0
  osg::Vec3f minCorner ( bounds.xMin(), bounds.yMin(), bounds.zMin() );
  osg::Vec3f maxCorner ( bounds.xMax(), bounds.yMax(), bounds.zMax() );
  _pointSet->bounds( minCorner, maxCorner );
  unsigned int capacity = Usul::Math::minimum( static_cast< unsigned int > ( _numPoints / 400 ), static_cast< unsigned int > ( std::numeric_limits< short >::max() ) );
  _pointSet->capacity( capacity );
#endif

  osg::Vec3f minCorner ( bounds.xMin(), bounds.yMin(), bounds.zMin() );
  osg::Vec3f maxCorner ( bounds.xMax(), bounds.yMax(), bounds.zMax() );
  
  out.seekp ( 0, std::ios_base::beg );

  {
    // Write binary header
    const std::string id ( "f2775490-ecf0-4362-8c81-7c1693e0736c" );
    out.write ( &id.c_str()[0], id.size() );

    // Write the min/max bounds and the number of points
    out.write ( reinterpret_cast<char *> ( &minCorner ), sizeof ( osg::Vec3f ) );  
    out.write ( reinterpret_cast<char *> ( &maxCorner ), sizeof ( osg::Vec3f ) );
    out.write ( reinterpret_cast<char *> ( &_numPoints ), sizeof ( unsigned int ) );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vectors from the linked lists
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_buildVectors( Unknown *caller, Unknown *progress )
{
  this->setStatusBar ( "Step 2/2: Building Spatial Parameters...", progress );
  _pointSet->buildVectors();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the octree
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_split( Unknown *caller, Unknown *progress )
{
  _pointSet->split( this, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the binary restart file format
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_readBinaryRestartFile( const std::string& filename, Unknown *caller, Unknown *progress )
{
  std::ifstream* ifs ( new std::ifstream );
  ifs->open( filename.c_str(), std::ifstream::out | std::ifstream::binary );

  if( false == ifs->is_open() )
    throw std::runtime_error ( "Error 3339602607: Failed to open file: " + filename );

  Usul::Types::Uint64 numPoints ( 0 );

  _pointSet->read( ifs, numPoints, this, caller, progress );

  _numPoints = numPoints;

  ifs->close();
  delete ifs;
  ifs = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the point color.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_editPointColor()
{
  Usul::Interfaces::IColorEditor::QueryPtr colorEditor ( this->delegate() );
  if( false == colorEditor.valid() )
    return;
  Usul::Math::Vec4f color ( 0.0f, 1.0f, 0.0f, 1.0f );
  if( true == colorEditor->editColor( color ) )
  {
    // Set the material color in PointSet
    osg::Vec4f c ( color[0], color[1], color[2], color[3] );
    _material->setAmbient( osg::Material::FRONT_AND_BACK, c );
    _material->setDiffuse( osg::Material::FRONT_AND_BACK, c );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::Button       Button;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  MenuKit::Menu::RefPtr pointMenu ( menu.find ( "&Points", true ) );

  pointMenu->append( new Button ( Usul::Commands::genericCommand ( "Edit Color...", Usul::Adaptors::memberFunction<void> ( this, &PointDocument::_editPointColor ), Usul::Commands::TrueFunctor() ) ) );
}
