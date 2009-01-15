
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "P3D2FVDocument.h"

#include "Usul/Predicates/FileExists.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( P3D2FVDocument, P3D2FVDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDocument::P3D2FVDocument() : 
  BaseClass ( "Plot3D to FieldView Converter" )
{
  USUL_TRACE_SCOPE;
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDocument::~P3D2FVDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *P3D2FVDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool P3D2FVDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool P3D2FVDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool P3D2FVDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "x" || ext == "y" || ext == "z" || ext == "u" || ext == "v" || ext == "w" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool P3D2FVDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "x" || ext == "y" || ext == "z" || ext == "u" || ext == "v" || ext == "w" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  this->_read( name, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDocument::Filters P3D2FVDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDocument::Filters P3D2FVDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDocument::Filters P3D2FVDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Plot3D Files (x) (*.x)", "*.x" ) );
  filters.push_back ( Filter ( "Plot3D Files (y) (*.y)", "*.y" ) );
  filters.push_back ( Filter ( "Plot3D Files (z) (*.z)", "*.z" ) );
  filters.push_back ( Filter ( "Plot3D Files (u) (*.x)", "*.u" ) );
  filters.push_back ( Filter ( "Plot3D Files (v) (*.y)", "*.v" ) );
  filters.push_back ( Filter ( "Plot3D Files (w) (*.z)", "*.w" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDocument::Filters P3D2FVDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *P3D2FVDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  GroupPtr group ( new osg::Group );

  return group.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a Plot3D file.
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::_read( const std::string& filename, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // create a file handle
  std::ifstream* ifs ( new std::ifstream );

  // open the file
  ifs->open( filename.c_str(), std::ifstream::out | std::ifstream::binary );

  // make sure the file was opened
  if( false == ifs->is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filename );

  // Read and set the number of clocks
  Usul::Types::Uint32 blocks ( 0 );
  ifs->read ( reinterpret_cast< char* > ( &blocks ), sizeof( Usul::Types::Uint32 ) );
  _numBlocks = blocks;

  // reserve space for the dimensions
  _dimensions.reserve( _numBlocks );

  // Initialize the grid space
  _grid.resize( _numBlocks );

  // loop through and read the individual block dimensions
  for( unsigned int i = 0; i < _numBlocks; ++i )
  {

    // Read and set the dimensions
    Usul::Types::Uint32 ni ( 0 );
    Usul::Types::Uint32 nj ( 0 );
    Usul::Types::Uint32 nk ( 0 );

    ifs->read ( reinterpret_cast< char* > ( &ni ), sizeof( Usul::Types::Uint32 ) );
    ifs->read ( reinterpret_cast< char* > ( &nj ), sizeof( Usul::Types::Uint32 ) );
    ifs->read ( reinterpret_cast< char* > ( &nk ), sizeof( Usul::Types::Uint32 ) );

    Usul::Math::Vec3ui dimensions ( ni, nj, nk );

    // add the dimensions of this block to the collections
    _dimensions.push_back( dimensions );

  }
  
#if 1
  for( unsigned int i = 0; i < _numBlocks; ++i )
  {
    // set the size to read
    Usul::Types::Uint32 size ( _dimensions.at( i )[0] * _dimensions.at( i )[1] * _dimensions.at( i )[2] );
    
    // initialize the column strip
    Grid1D column( size );    
    
    // read the column
    ifs->read( reinterpret_cast< char * > ( &column[0] ), size * sizeof( Usul::Math::Vec3f ) );

    _grid.at( i ) = column;
  }
#else
  
  // loop through and read the x,y,z values for the blocks
  for( unsigned int i = 0; i < _numBlocks; ++i )
  { 
    // Initialize the block
    this->_initializeGridAt( i, _dimensions.at( i ) );

    // loop through z values
    for( unsigned int z = 0; z < _dimensions.at( i )[2]; ++z )
    {
      // loop through x values
      for( unsigned int x = 0; x < _dimensions.at( i )[0]; ++x )
      {
        // initialize the column strip
        Grid1D column ( _dimensions.at( i )[1] );
        
        // set the size to read
        Usul::Types::Uint32 size ( sizeof( Usul::Types::Float32 ) * _dimensions.at( i )[1] );

        // read the column
        ifs->read( reinterpret_cast< char * > ( &column[0] ), size );
        
        // set the column
        this->_setColumnAt( i, z, x, column );
        
      }
     
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the grid space.
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::_initializeGrid( Usul::Types::Uint32 n )
{
  Guard guard ( this );
  
  //resize the grid
  _grid.resize( n );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize a block in the grid
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::_initializeGridAt( Usul::Types::Uint32 index, Usul::Math::Vec3ui size )
{
  Guard guard ( this );


}



///////////////////////////////////////////////////////////////////////////////
//
//  Set a value in the grid at position t,i,j,k
//
///////////////////////////////////////////////////////////////////////////////

void P3D2FVDocument::_setValueAt( Usul::Types::Uint32 t, Usul::Types::Uint32 i, Usul::Types::Uint32 j, Usul::Types::Uint32 k, Usul::Math::Vec3f value )
{
  Guard guard ( this );

  // get the dimensions of this time slice
  Usul::Math::Vec3ui d ( _dimensions.at( t ) );

  // get the requested index
  unsigned int index ( j + ( i * d[1] ) + ( k * d[0] * d[1] ) );

  // set the value in the grid
  _grid.at( t ).at( index ) = value;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a value in the grid at position t,i,j,k
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3f P3D2FVDocument::_getValueAt( Usul::Types::Uint32 t, Usul::Types::Uint32 i, Usul::Types::Uint32 j, Usul::Types::Uint32 k )
{
  Guard guard ( this );

  // Get the dimensions of this time slice
  Usul::Math::Vec3ui d ( _dimensions.at( t ) );

  // Get the index
  unsigned int index ( j + ( i * d[1] ) + ( k * d[0] * d[1] ) );

  // Get the value at the desired index
  Usul::Math::Vec3f value ( _grid.at( t ).at( index ) );

  // return
  return value;
}