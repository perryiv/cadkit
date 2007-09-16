
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
#include "Discretization.h"
#include "BasicPackage.h"
#include "BlockCenteredFlow.h"

#include "XmlTree/Document.h"
#include "XmlTree/Predicates.h"
#include "XmlTree/XercesLife.h"

#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Strings/Case.h"
#include "Usul/System/Directory.h"
#include "Usul/System/LastError.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/WriteFile"

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
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::ModflowDocument() : BaseClass ( "Modflow Document" ),
  _gridSize ( 0, 0 ),
  _cellSize ( 0.0f, 0.0f ),
  _bounds(),
  _startHeads(),
  _landElev(),
  _bottomElev(),
  _layers()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::~ModflowDocument()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ModflowDocument::_destroy ), "2110190659" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_destroy()
{
  USUL_TRACE_SCOPE;
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModflowDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::write ( const std::string &name, Unknown *caller  ) const
{
  USUL_TRACE_SCOPE;
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
    USUL_TRACE_SCOPE_STATIC;
    if ( true == in.eof() )
    {
      throw std::runtime_error ( "Error 1290634954: unexpected end-of-file encountered while reading file" );
    }
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
    USUL_TRACE_SCOPE_STATIC;
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
    ( unsigned int numRows, unsigned int numCols, double cellX, double cellY, ArrayType &data, std::istream &in )
  {
    USUL_TRACE_SCOPE_STATIC;
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
  USUL_TRACE_SCOPE;
  typedef XmlTree::Node::Children Children;

  // Scope the directory change.
  Usul::System::Directory::ScopedCwd cwd ( Usul::File::directory ( file, false ) );

  // Add readers to a factory.
  Usul::Factory::ObjectFactory factory;
  factory.add ( new Usul::Factory::TypeCreator<Discretization>    ( "discretization"      ) );
  factory.add ( new Usul::Factory::TypeCreator<BasicPackage>      ( "basic_package"       ) );
  factory.add ( new Usul::Factory::TypeCreator<BlockCenteredFlow> ( "block_centered_flow" ) );

  // Initialize and finalize use of xerces.
  XmlTree::XercesLife life;

  // Open the input file.
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( file );

  // Find all important files.
  Children files ( document->find ( "file", true ) );

  // Find discretization file.
  Children::iterator discretization ( std::find_if ( files.begin(), files.end(), XmlTree::HasAttribute ( "type", "discretization" ) ) );
  if ( files.end() == discretization )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 1994992467: Add discretization file to: ", file );
  this->_read ( factory, discretization->get(), progress );

  // Remove discretization file from list.
  files.erase ( discretization );

  // Read the remaining files.
  for ( Children::iterator i = files.begin(); i != files.end(); ++i )
  {
    this->_read ( factory, i->get(), progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_read ( Factory &factory, XmlTree::Node *file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != file )
  {
    XmlTree::Node::Attributes &a ( file->attributes() );
    this->_read ( factory, a["type"], a["name"], progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_read ( Factory &factory, const std::string &type, const std::string &file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  BaseReader::RefPtr reader ( dynamic_cast<BaseReader *> ( factory.create ( type ) ) );
  if ( true == reader.valid() )
  {
    reader->read ( this, file, progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::clear ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _gridSize.set ( 0, 0 );
  _cellSize.set ( 0.0f, 0.0f );

  _bounds.clear(),
  _startHeads.clear(),
  _landElev.second.clear(),
  _bottomElev.clear();

  // Have to clear each layer first because they reference each other.
  std::for_each ( _layers.begin(), _layers.end(), std::mem_fun ( &Layer::clear ) );
  _layers.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Filters ModflowDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModflowDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr<osg::Group> group ( new osg::Group );
  double scaleFactor ( 0.95 );

  // Add the layers.
  const unsigned int num ( _layers.size() );
  const unsigned int flags ( Cell::Draw::CUBE | Cell::Draw::BOUNDS );
  for ( unsigned int i = 0; i < num; ++i )
  {
    const Layer::RefPtr &layer ( _layers.at(i) );
    group->addChild ( layer->buildScene ( flags, scaleFactor, caller ) );
    //scaleFactor -= 0.05;
  }

#ifdef _DEBUG
#if 0
  const std::string file ( Usul::File::fullPath ( "modflow.osg" ) );
  osgDB::writeNodeFile ( *group, file );
#endif
#endif
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_incrementProgress ( bool state, Unknown *progress, unsigned int &numerator, unsigned int denominator )
{
  USUL_TRACE_SCOPE;
  this->setProgressBar ( state, numerator, denominator, progress );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the location of the grid cell.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Vec2d ModflowDocument::_location ( unsigned int index ) const
{
  USUL_TRACE_SCOPE;
  const unsigned int col ( index % _gridSize[1] );
  const unsigned int row ( ( index - col ) / _gridSize[1] );

  const double x ( _cellSize[0] * col );

  const double height ( _cellSize[1] * _gridSize[1] );
  const double y ( height - ( _cellSize[1] * row ) );

  return Vec2d ( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layers.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::layers ( Layers &layers )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _layers = layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Layers &ModflowDocument::layers()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

const ModflowDocument::Layers &ModflowDocument::layers() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of layers.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::numLayers() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grid size.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Vec2ui ModflowDocument::gridSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Layer::RefPtr layer ( _layers.empty() ? 0x0 : _layers.front() );
  return ( ( layer.valid() ) ? Vec2ui ( layer->gridSize() ) : Vec2ui ( 0, 0 ) );
}


#if 0
  Need to register functors that map the type with a reader function.

  Important!
  -- Animate through time steps
  -- Cutting planes

  Would be good to...
  -- Color control
  -- Cell drawing control
  -- Overlay data from PostGIS
#endif
