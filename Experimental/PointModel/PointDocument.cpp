
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#include "PointDocument.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/File/Path.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Math/MinMax.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <list>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PointDocument, PointDocument::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( PointDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::PointDocument() : BaseClass ( "Point Document" ),
_pointSet( new PointSet() )
{

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
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "point3d"  );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool PointDocument::canSave ( const std::string &file ) const
{
  return ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "point3d" == ext )
  {
    this->_readAndSetBounds( name, caller, progress );
    this->_readPoint3DFile( name, caller, progress );
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
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

PointDocument::Filters PointDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "3D Point Files (*.point3d )", "*.point3d" ) );
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
  
  return _pointSet->buildScene ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_readPoint3DFile( const std::string &filename, Unknown *caller, Unknown *progress )
{
  osg::Vec3f min( 0.0, 0.0, 0.0 );
  osg::Vec3f max( 0.0, 0.0, 0.0 );

  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  std::ifstream in;
  in.open ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2265042350: Failed to open file: " + filename );

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
  // Read the first line to establisht the min and max values
  // to be used to compare against remaining points in order
  // to determine the bounds of the data
  
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
      osg::Vec3d p ( 0.0, 0.0, 0.0 );
      if( true == foundX )
      {
        p.x() = Usul::Convert::Type< std::string, double >::convert( sv.at( x_index ) );
      }
      if( true == foundY )
      {
        p.y() = Usul::Convert::Type< std::string, double >::convert( sv.at( y_index ) );
      }
      if( true == foundZ )
      {
        p.z() = Usul::Convert::Type< std::string, double >::convert( sv.at( z_index ) );
      }
      min = osg::Vec3f( static_cast< float > ( p.x() ), static_cast< float > ( p.y() ), static_cast< float > ( p.z() ) );
      max = osg::Vec3f( static_cast< float > ( p.x() ), static_cast< float > ( p.y() ), static_cast< float > ( p.z() ) );
      _pointSet->addPoint( p );
    }

  }

  // Read the rest of the file.
  // If the x, y, or z header wasn't found then set
  // their corresponding values to 0  
  while( false == in.eof() )
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
      osg::Vec3d p ( 0.0, 0.0, 0.0 );
      if( true == foundX )
      {
        p.x() = Usul::Convert::Type< std::string, double >::convert( sv.at( x_index ) );
      }
      if( true == foundY )
      {
        p.y() = Usul::Convert::Type< std::string, double >::convert( sv.at( y_index ) );
      }
      if( true == foundZ )
      {
        p.z() = Usul::Convert::Type< std::string, double >::convert( sv.at( z_index ) );
      }
      
      min.x() = Usul::Math::minimum( static_cast< float > ( p.x() ), min.x() );
      min.y() = Usul::Math::minimum( static_cast< float > ( p.y() ), min.y() );
      min.z() = Usul::Math::minimum( static_cast< float > ( p.z() ), min.z() );

      max.x() = Usul::Math::maximum( static_cast< float > ( p.x() ), max.x() );
      max.y() = Usul::Math::maximum( static_cast< float > ( p.y() ), max.y() );
      max.z() = Usul::Math::maximum( static_cast< float > ( p.z() ), max.z() );

      _pointSet->addPoint( p );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_readAndSetBounds( const std::string &filename, Unknown *caller, Unknown *progress )
{
  osg::Vec3f min( 0.0, 0.0, 0.0 );
  osg::Vec3f max( 0.0, 0.0, 0.0 );
  unsigned int numPoints = 0;
  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  std::ifstream in;
  in.open ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2265042350: Failed to open file: " + filename );

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
  // Read the first line to establisht the min and max values
  // to be used to compare against remaining points in order
  // to determine the bounds of the data
  
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
      osg::Vec3d p ( 0.0, 0.0, 0.0 );
      if( true == foundX )
      {
        p.x() = Usul::Convert::Type< std::string, double >::convert( sv.at( x_index ) );
      }
      if( true == foundY )
      {
        p.y() = Usul::Convert::Type< std::string, double >::convert( sv.at( y_index ) );
      }
      if( true == foundZ )
      {
        p.z() = Usul::Convert::Type< std::string, double >::convert( sv.at( z_index ) );
      }
      min = osg::Vec3f( static_cast< float > ( p.x() ), static_cast< float > ( p.y() ), static_cast< float > ( p.z() ) );
      max = osg::Vec3f( static_cast< float > ( p.x() ), static_cast< float > ( p.y() ), static_cast< float > ( p.z() ) );

      ++numPoints;
    }

  }

  // Read the rest of the file.
  // If the x, y, or z header wasn't found then set
  // their corresponding values to 0  
  while( false == in.eof() )
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
      osg::Vec3d p ( 0.0, 0.0, 0.0 );
      if( true == foundX )
      {
        p.x() = Usul::Convert::Type< std::string, double >::convert( sv.at( x_index ) );
      }
      if( true == foundY )
      {
        p.y() = Usul::Convert::Type< std::string, double >::convert( sv.at( y_index ) );
      }
      if( true == foundZ )
      {
        p.z() = Usul::Convert::Type< std::string, double >::convert( sv.at( z_index ) );
      }
      
      min.x() = Usul::Math::minimum( static_cast< float > ( p.x() ), min.x() );
      min.y() = Usul::Math::minimum( static_cast< float > ( p.y() ), min.y() );
      min.z() = Usul::Math::minimum( static_cast< float > ( p.z() ), min.z() );

      max.x() = Usul::Math::maximum( static_cast< float > ( p.x() ), max.x() );
      max.y() = Usul::Math::maximum( static_cast< float > ( p.y() ), max.y() );
      max.z() = Usul::Math::maximum( static_cast< float > ( p.z() ), max.z() );

    }
    ++numPoints;
  }
  _pointSet->bounds( min, max );
  _pointSet->allocate( numPoints );
  unsigned int tolerance = numPoints / 200;
  _pointSet->tolerance( tolerance );
}

