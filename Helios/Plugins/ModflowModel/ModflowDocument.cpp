
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for scene files.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDocument.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Strings/Case.h"
#include "Usul/System/Directory.h"
#include "Usul/System/LastError.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "boost/algorithm/string/trim.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModflowDocument, ModflowDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

namespace Modflow
{
  const unsigned int LINE_BUFFER_SIZE ( 512 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::ModflowDocument() : BaseClass ( "Modflow Document" ),
  _numLayers ( 0 ),
  _gridSize ( 0, 0 ),
  _cellSize ( 0.0f, 0.0f ),
  _bounds(),
  _startHeads(),
  _landElev(),
  _bottomElev()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::~ModflowDocument()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ModflowDocument::_destroy ), "2110190659" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_destroy()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModflowDocument::queryInterface ( unsigned long iid )
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
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::canInsert ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "modflow" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::canSave ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::write ( const std::string &name, Unknown *caller  ) const
{
  Guard guard ( this->mutex() );
  std::cout << "Warning 3910751487: writing Modflow documents is not implemented" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void checkStream ( std::istream &in )
  {
    if ( true == in.eof() )
    {
      throw std::runtime_error ( "Error 1290634954: unexpected end-of-file encountered while reading file" );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  std::string getLine ( std::istream &in )
  {
    Helper::checkStream ( in );
    std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );
    in.getline ( &buffer[0], buffer.size() - 1 );
    return std::string ( &buffer[0] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the first file with the given extension.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline std::string findFileWithExtension ( const XmlTree::Node::Children &files, const std::string &lookFor )
  {
    typedef XmlTree::Node::Children Children;
    for ( Children::const_iterator i = files.begin(); i != files.end(); ++i )
    {
      if ( 0x0 != i->get() )
      {
        const std::string file ( i->get()->value() );
        const std::string ext ( Usul::File::extension ( Usul::Strings::lowerCase ( file ) ) );
        if ( lookFor == ext )
        {
          return Usul::File::fullPath ( file );
        }
      }
    }
    return std::string();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the array.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class ArrayType > inline void readAsciiArray 
    ( unsigned int numRows, unsigned int numCols, float cellX, float cellY, ArrayType &data, std::istream &in )
  {
    const unsigned int num ( numRows * numCols );
    data.resize ( num );
    for ( unsigned int i = 0; i < num; ++i )
    {
      Helper::checkStream ( in );
      in >> data[i];
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::read ( const std::string &file, Unknown *caller, Unknown *progress )
{
  typedef XmlTree::Node::Children Children;

  // Scope the directory change.
  Usul::System::Directory::ScopedCwd cwd ( Usul::File::directory ( file, false ) );

  // Initialize and finalize use of xerces.
  XmlTree::XercesLife life;

  // Open the input file.
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( file );

  // Find all important nodes.
  Children files ( document->find ( "file", true ) );

  // Read the discretization file first, it has the number of layers.
  this->_readDiscretizationFile ( Helper::findFileWithExtension ( files, "dis" ), progress );

  // Read the basic package file.
  this->_readBasicPackageFile ( Helper::findFileWithExtension ( files, "ba6" ), progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the discretization file.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_readDiscretizationFile ( const std::string &file, Unknown *progress )
{
  Guard guard ( this->mutex() );

  // Ignore empty strings.
  if ( true == file.empty() )
    return;

  std::cout << "Reading: " << file << std::endl;

  // Open the file.
  Usul::System::LastError::init();
  std::ifstream in ( file.c_str() );
  if ( false == in.is_open() )
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1126947177: failed to open file '", file, "', ", Usul::System::LastError::message() );

  // Skip comment lines.
  this->_skipLines ( '#', in );

  // Read the header.
  Helper::checkStream ( in );
  in >> _numLayers >> _gridSize[0] >> _gridSize[1];

  // Skip to the grid points.
  this->_seekToLine ( "CONSTANT", in );

  // Read the cell size.
  std::string dummy;
  Helper::checkStream ( in );
  in >> dummy >> _cellSize[0] >> dummy;
  in >> dummy >> _cellSize[1] >> dummy;

  // Read the land surface.
  this->_seekToLine ( "INTERNAL", in );
  Helper::checkStream ( in );
  this->_readGrid ( in, _landElev );

  // For each layer...
  _bottomElev.resize ( _numLayers );
  for ( unsigned int i = 0; i < _numLayers; ++i )
  {
    // Read the grid.
    this->_seekToLine ( "INTERNAL", in );
    Helper::checkStream ( in );
    this->_readGrid ( in, _bottomElev.at(i) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the basic package file.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_readBasicPackageFile ( const std::string &file, Unknown *progress )
{
  Guard guard ( this->mutex() );

  // Ignore empty strings.
  if ( true == file.empty() )
    return;

  std::cout << "Reading: " << file << std::endl;

  // Open the file.
  Usul::System::LastError::init();
  std::ifstream in ( file.c_str() );
  if ( false == in.is_open() )
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 4171642072: failed to open file '", file, "', ", Usul::System::LastError::message() );

  // For each layer...
  _bounds.resize ( _numLayers );
  for ( unsigned int i = 0; i < _numLayers; ++i )
  {
    // Read the grid.
    this->_seekToLine ( "INTERNAL", in );
    Helper::checkStream ( in );
    this->_readGrid ( in, _bounds.at(i) );
  }

  // For each layer...
  _startHeads.resize ( _numLayers );
  for ( unsigned int i = 0; i < _numLayers; ++i )
  {
    // Read the grid.
    this->_seekToLine ( "INTERNAL", in );
    Helper::checkStream ( in );
    this->_readGrid ( in, _startHeads.at(i) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the grid.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_readGrid ( std::istream &in, GridInfo &grid )
{
  Guard guard ( this->mutex() );

  // Read the name.
  std::string dummy;
  in >> dummy >> dummy >> dummy >> dummy;
  Helper::checkStream ( in );
  std::string name ( Helper::getLine ( in ) );
  boost::algorithm::trim ( name );

  // Make the map entry and get reference to data.
  grid.first = name;
  GridData &data = grid.second;

  // Read the grid points.
  const unsigned int numCells ( _gridSize[0] * _gridSize[1] );
  data.resize ( numCells );
  Helper::readAsciiArray ( _gridSize[0], _gridSize[1], _cellSize[0], _cellSize[1], data, in );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek until you read the given word. Put cursor at beginning of the line.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_seekToLine ( const std::string &word, std::istream &in )
{
  Guard guard ( this->mutex() );

  std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );
  std::string s;

  while ( false == in.eof() )
  {
    std::fill ( buffer.begin(), buffer.end(), '\0' );

    const std::istream::pos_type pos ( in.tellg() );
    in.getline ( &buffer[0], buffer.size() - 1 );

    std::istringstream line ( &buffer[0] );
    line >> s;

    if ( word == s )
    {
      in.seekg ( pos );
      return;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek past all lines that start with the given character.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_skipLines ( char c, std::istream &in )
{
  Guard guard ( this->mutex() );

  std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );

  while ( false == in.eof() )
  {
    const char next ( in.peek() );
    if ( next == c )
    {
      in.getline ( &buffer[0], buffer.size() - 1 );
    }
    else
    {
      return;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::clear ( Usul::Interfaces::IUnknown * )
{
  Guard guard ( this->mutex() );

  _numLayers = 0;
  _gridSize.set ( 0, 0 );
  _cellSize.set ( 0.0f, 0.0f );

  _bounds.clear(),
  _startHeads.clear(),
  _landElev.second.clear(),
  _bottomElev.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersSave() const
{
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersExport() const
{
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Modflow Output (*.modflow)", "*.modflow" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersInsert() const
{
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModflowDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  Guard guard ( this->mutex() );

  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Add land surface.
  //group->addChild ( this->_buildGrid ( 0, _landElev.second, false, caller ) );

  // Add the water surfaces.
  for ( unsigned int i = 0; i < _numLayers; ++i )
  {
    group->addChild ( this->_buildGrid ( i, _bottomElev.at(i).second, true, caller ) );
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the grid.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModflowDocument::_buildGrid ( unsigned int layer, GridData &grid, bool useBounds, Unknown *caller )
{
  Guard guard ( this->mutex() );

  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

  const unsigned int numCells ( grid.size() );
  osg::ref_ptr<osg::Vec3Array> quads ( new osg::Vec3Array );
  quads->reserve ( numCells * 4 );

  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
  normals->push_back ( osg::Vec3f ( 0.0f, 0.0f, 1.0f ) );

  const osg::Vec2f size ( _cellSize[0] / 2.0f, _cellSize[1] / 2.0f );

  // Make the cells.
  for ( unsigned int j = 0; j < numCells; ++j )
  {
    if ( ( ( true == useBounds ) && ( 0.0f != _bounds.at(layer).second.at(j) ) ) || ( false == useBounds ) )
    {
      const Vec2f location ( this->_location ( j ) );
      const Vec3f center ( location[0], location[1], grid.at(j) );
      quads->push_back ( osg::Vec3f ( center[0] - size[0], center[1] - size[1], center[2] ) );
      quads->push_back ( osg::Vec3f ( center[0] + size[0], center[1] - size[1], center[2] ) );
      quads->push_back ( osg::Vec3f ( center[0] + size[0], center[1] + size[1], center[2] ) );
      quads->push_back ( osg::Vec3f ( center[0] - size[0], center[1] + size[1], center[2] ) );
    }
  }

  // We often skip many cells above.
  quads->trim();

  // Make geometry.
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  geom->setVertexArray ( quads.get() );

  // Add vectors.
  geom->setNormalArray ( normals.get() );
  geom->setNormalBinding ( osg::Geometry::BIND_OVERALL );

  // Add primitive set.
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::DrawArrays::QUADS, 0, quads->size() ) );
  geode->addDrawable ( geom.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_incrementProgress ( bool state, Unknown *progress, unsigned int &numerator, unsigned int denominator )
{
  this->setProgressBar ( state, numerator, denominator, progress );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the location of the grid cell.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Vec2f ModflowDocument::_location ( unsigned int index ) const
{
  const unsigned int col ( index % _gridSize[1] );
  const unsigned int row ( ( index - col ) / _gridSize[1] );

  const float x ( _cellSize[0] * col );

  const float height ( _cellSize[1] * _gridSize[1] );
  const float y ( height - ( _cellSize[1] * row ) );

  return Vec2f ( x, y );
}
