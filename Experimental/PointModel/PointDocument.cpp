
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
_points( new PointSet() )
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
  // Redirect to triangle set
  return _points->buildScene ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a .point3d file.
//
///////////////////////////////////////////////////////////////////////////////

void PointDocument::_readPoint3DFile( const std::string &filename, Unknown *caller, Unknown *progress )
{
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

      _points->addPoint( p );
    }
  }
}
