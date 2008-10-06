
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

#include "OOCPointDocument.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Interfaces/IColorEditor.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IViewport.h"

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
#include "Usul/Documents/Manager.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Constants.h"

#include "OsgTools/State/StateSet.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <string>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OOCPointDocument, OOCPointDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( OOCPointDocument );


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

OOCPointDocument::OOCPointDocument() : BaseClass ( "Point Document" ),
  _pointSet ( 0x0 ),
  _numPoints ( 0 ),
  _material ( new osg::Material() ),
  _color ( 0.5f, 0.5f, 0.25f, 1.0f ),
  _workingDir(),
  _xpos ( 0 ),
  _ypos ( 0 ),
  _manager ( 0x0 )
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

OOCPointDocument::~OOCPointDocument()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &OOCPointDocument::_destroy ), "3494661626" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::_destroy()
{
  // Done with these.
  _pointSet = 0x0;
  _material = 0x0;
  _workingDir.clear();

  // Clean up job manager. Do this last.
  if ( 0x0 != _manager )
  {
    // Remove all queued jobs and cancel running jobs.
    _manager->cancel();
    
    // Wait for remaining jobs to finish.
    _manager->wait();
    
    // Delete the manager.
    delete _manager;
    _manager = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *OOCPointDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IBuildScene::IID:
      return static_cast < Usul::Interfaces::IBuildScene* > ( this );
    case Usul::Interfaces::IMenuAdd::IID:
      return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
    case Usul::Interfaces::IJobFinishedListener::IID:
      return static_cast < Usul::Interfaces::IJobFinishedListener * > ( this );
    case Usul::Interfaces::IUpdateListener::IID:
      return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
    default:
      return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool OOCPointDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool OOCPointDocument::canInsert ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ap3d" || ext == "oocp3d" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool OOCPointDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "psxml" || ext == "ap3d" || ext == "oocp3d" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool OOCPointDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "oocp3d" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the xml file and parse elements.
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::_parseXML( XmlTree::Node &node, Unknown *caller, Unknown *progress )
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

void OOCPointDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
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

  // Set the working directory in our point set.
  this->_getPointSet()->workingDir( Usul::File::directory( name, true ) );
  this->_getPointSet()->baseName( Usul::File::base( name ) );

  this->_read( name, caller, progress );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////


void OOCPointDocument::_read ( const std::string &name, Unknown *caller, Unknown *progress )
{

  // Binary restart filename
  std::string restartFilename ( Usul::Strings::format ( Usul::File::directory( name, true ), Usul::File::base( name ), ".oocp3d" ) );

  // TODO:
  // Check to see if the binary restart file exists.  
  // If it does and the date is greater than the ASCII data
  // read that file instead.

  // Get the extension of the file to load
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "ap3d" == ext )
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
  else if( "oocp3d" == ext )
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

void OOCPointDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  std::ofstream* ofs ( new std::ofstream );
  ofs->open( name.c_str(), std::ofstream::out | std::ifstream::binary ); 

  // write the binary restart file.
  OOCPointDocument *me ( const_cast < OOCPointDocument * > ( this ) );
  if ( 0x0 != me )
  {
    me->_getPointSet()->write( ofs, _numPoints, 0x0, caller, progress );
  }

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

void OOCPointDocument::clear ( Usul::Interfaces::IUnknown *caller )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

OOCPointDocument::Filters OOCPointDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "Ascii 3D Point Files (*.ap3d )", "*.ap3d" ) );
  filters.push_back ( Filter ( "Point Set Binary Restart Files (*.oocp3d )", "*.oocp3d" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

OOCPointDocument::Filters OOCPointDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Ascii 3D Point Files (*.ap3d )", "*.ap3d" ) );
  filters.push_back ( Filter ( "Point Set Binary Restart Files (*.oocp3d )", "*.oocp3d" ) );
  filters.push_back ( Filter ( "Point Set Document XML (*.psxml )", "*.psxml" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

OOCPointDocument::Filters OOCPointDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Point Set Binary Restart Files (*.oocp3d )", "*.oocp3d" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

OOCPointDocument::Filters OOCPointDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *OOCPointDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  // Redirect to point set
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  group->addChild( this->_getPointSet()->buildScene ( caller ) );
  OsgTools::State::StateSet::setMaterial( group.get(), _material.get() );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::_readPoint3DFile( const std::string &filename, Unknown *caller, Unknown *progress )
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
  this->_getPointSet()->bounds( minCorner, maxCorner );
  unsigned int capacity = Usul::Math::minimum( static_cast< unsigned int > ( _numPoints / 400 ), static_cast< unsigned int > ( std::numeric_limits< short >::max() ) );
  this->_getPointSet()->capacity( capacity );
  
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
    
      if( false == this->_getPointSet()->addPoint( value ) )
      {
        std::cout << "Failed to insert point in OOCPointDocument! " << std::endl;
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

bool OOCPointDocument::_parseHeader( const std::string &filename, Unknown *caller, Unknown *progress )
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

void OOCPointDocument::_fastReadAndSetBounds( const std::string &filename, const std::string &binaryFilename, Unknown *caller, Unknown *progress )
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

void OOCPointDocument::_readAndSetBounds( const std::string &filename, const std::string &binaryFilename, Unknown *caller, Unknown *progress )
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
  this->_getPointSet()->bounds( minCorner, maxCorner );
  unsigned int capacity = Usul::Math::minimum( static_cast< unsigned int > ( _numPoints / 400 ), static_cast< unsigned int > ( std::numeric_limits< short >::max() ) );
  this->_getPointSet()->capacity( capacity );
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

void OOCPointDocument::_buildVectors( Unknown *caller, Unknown *progress )
{
  this->setStatusBar ( "Step 2/2: Building Spatial Parameters...", progress );
  this->_getPointSet()->buildVectors();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the octree
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::_split( Unknown *caller, Unknown *progress )
{
  this->_getPointSet()->split( this, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the binary restart file format
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::_readBinaryRestartFile( const std::string& filename, Unknown *caller, Unknown *progress )
{
  std::ifstream* ifs ( new std::ifstream );
  ifs->open( filename.c_str(), std::ifstream::out | std::ifstream::binary );

  if( false == ifs->is_open() )
    throw std::runtime_error ( "Error 3339602607: Failed to open file: " + filename );

  Usul::Types::Uint64 numPoints ( 0 );

  this->_getPointSet()->read( ifs, numPoints, this, caller, progress );

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

void OOCPointDocument::_editPointColor()
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

void OOCPointDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::Button       Button;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  MenuKit::Menu::RefPtr pointMenu ( menu.find ( "&Points", true ) );

  pointMenu->append( new Button ( Usul::Commands::genericCommand ( "Edit Color...", Usul::Adaptors::memberFunction<void> ( this, &OOCPointDocument::_editPointColor ), Usul::Commands::TrueFunctor() ) ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  The Job is finished.  Implemented to request redraws when jobs finish
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::jobFinished ( Usul::Jobs::Job *job )
{
  //NOTE: This can still be further optimized

  // redraw the scene
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a view from this document.
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::removeView ( Usul::Interfaces::IView *view )
{
  // Call the base class' first.
  BaseClass::removeView ( view );

  // If there are no more views, remove the job finished listener.
  if ( 0 == this->numViews() )
  {
    this->_getJobManager()->removeJobFinishedListener ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update callback
//
///////////////////////////////////////////////////////////////////////////////


void OOCPointDocument::updateNotify( Unknown *caller )
{
  // Job update stuff
  unsigned int numQueued ( this->_getJobManager()->numJobsQueued() );
  unsigned int numRunning ( this->_getJobManager()->numJobsExecuting() );
#if 0
  std::string status( "" );
  if( 0 != numQueued && 0 != numRunning )
  {
    status = Usul::Strings::format( "Number of running Jobs: ", numRunning, "\nNumber of queued jobs: ", numQueued );
  }
  this->_setStatusText( status, _xpos, _ypos, 0.0, 0.95, caller );
#else
  std::string status( Usul::Strings::format( "Number of running Jobs: ", numRunning, "\nNumber of queued jobs: ", numQueued ) );
  this->_setStatusText( status, _xpos, _ypos, 0.0, 0.95, caller );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the status text
//
///////////////////////////////////////////////////////////////////////////////

void OOCPointDocument::_setStatusText( const std::string message, unsigned int &textXPos, unsigned int &textYPos, double xmult, double ymult, Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this ); 

  Usul::Interfaces::ITextMatrix::QueryPtr textMatrix ( caller );
  if( false == textMatrix.valid() )
    throw std::runtime_error ( "Error 3793514250: Failed to find a valid interface to Usul::Interfaces::ITextMatrix " );

  Usul::Interfaces::IViewport::QueryPtr viewPort( caller );
  if( false == viewPort.valid() )
    throw std::runtime_error ( "Error 2482359443: Failed to find a valid interface to Usul::Interfaces::IViewport " );

  textMatrix->removeText( static_cast< unsigned int > ( textXPos ),
                          static_cast< unsigned int > ( textYPos ) );
   
  const double xpos ( ::floor( viewPort->width()  * xmult ) );
  const double ypos ( ::floor( viewPort->height() * ymult ) );

#if 0
  osg::Vec4f fcolor (  0.841, 0.763, 0.371, 1 );
  osg::Vec4f bcolor (  0.841, 0.763, 0.371, 1 );
#else
  osg::Vec4f fcolor (  1.0, 1.0, 1.0, 1 );
  osg::Vec4f bcolor (  0.0, 0.0, 0.0, 1 );
#endif

  textXPos = static_cast< unsigned int > ( xpos );
  textYPos = static_cast< unsigned int > ( ypos );

  textMatrix->setText ( textXPos, textYPos, message, fcolor, bcolor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the job manager.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager *OOCPointDocument::_getJobManager()
{
  Guard guard ( this );
  
  // Only make it once.
  if ( 0x0 == _manager )
  {
    typedef Usul::Registry::Database Reg;
    namespace Sections = Usul::Registry::Sections;
    
    const std::string type ( Reg::instance().convertToTag ( this->typeName() ) );
    Usul::Registry::Node &node ( Reg::instance()[Sections::DOCUMENT_SETTINGS][type]["job_manager_thread_pool_size"] );
    const unsigned int poolSize ( node.get<unsigned int> ( 5, true ) );
    
    _manager = new Usul::Jobs::Manager ( poolSize, true );
    _manager->logSet ( Usul::Jobs::Manager::instance().logGet() );
    _manager->addJobFinishedListener ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
  
  return _manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point set.
//
///////////////////////////////////////////////////////////////////////////////

OOCPointDocument::PointSet *OOCPointDocument::_getPointSet()
{
  Guard guard ( this );

  if ( false == _pointSet.valid() )
  {
    _pointSet = new PointSet ( this->_getJobManager() );
  }
  
  return _pointSet.get();
}

