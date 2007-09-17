
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
#include "Constants.h"
#include "Discretization.h"
#include "BasicPackage.h"
#include "BlockCenteredFlow.h"

#include "XmlTree/Document.h"
#include "XmlTree/Predicates.h"
#include "XmlTree/XercesLife.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
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
  _layers(),
  _root ( new osg::Group ),
  _built ( 0x0 ),
  _flags ( Modflow::Flags::DIRTY | Modflow::Flags::VISIBLE )
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
  _root = 0x0;
  _built = 0x0;
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
  case Usul::Interfaces::ILayerList::IID:
    return static_cast < Usul::Interfaces::ILayerList* > ( this );
  case Usul::Interfaces::IDirtyState::IID:
    return static_cast < Usul::Interfaces::IDirtyState* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
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

  // Clear the scene.
  OsgTools::Group::removeAllChildren ( _root.get() );
  OsgTools::Group::removeAllChildren ( _built.get() );
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
//  Build the scene. Actually, just return the root.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModflowDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Should never happen...
  if ( false == _root.valid() )
    _root = new osg::Group;

  // Return the scene.
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_buildScene ( Unknown *caller )
{
  USUL_TRACE_SCOPE;

  // Make a copy of the layer pointers.
  Layers layers;
  {
    Guard guard ( this->mutex() );
    layers.assign ( _layers.begin(), _layers.end() );
  }

  // Make a new group.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Add the layers.
  const unsigned int num ( layers.size() );
  for ( unsigned int i = 0; i < num; ++i )
  {
    Layer::RefPtr layer ( layers.at(i) );
    layer->flags ( Usul::Bits::add ( layer->flags(), Modflow::Flags::CUBE | Modflow::Flags::BOUNDS ) );
    layer->margin ( 500, 500, 0 );
    group->addChild ( layer->buildScene ( caller ) );
  }

  // Assign the new group to our staging area.
  {
    Guard guard ( this->mutex() );
    _built = group;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the viewer is about to render.
//
//  Note: more then one viewer for this document will still work as long as 
//  they all render in the same thread.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Are we dirty?
  if ( false == this->dirtyState() )
    return;

  // Should never happen...
  if ( false == _root.valid() )
    _root = new osg::Group;

  // Build the scene. TODO: Put this in a job.
  this->_buildScene ( caller );

  // Swap scenes if we should.
  if ( true == _built.valid() )
  {
    // Remove and add because _root was given to the viewer.
    OsgTools::Group::removeAllChildren ( _root.get() );
    OsgTools::Group::addAllChildren ( _built.get(), _root.get() );

    // Reset this!
    _built = 0x0;

    // We're not dirty now.
    this->dirtyState ( false );
  }
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
//  Set the layers.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::layers ( Layers &layers )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set layers.
  _layers = layers;

  // Tell all layers they belong to this document.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  std::for_each ( _layers.begin(), _layers.end(), std::bind2nd ( std::mem_fun1 ( &Layer::document ), me.get() ) );
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

unsigned int ModflowDocument::numberLayers() const
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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer at position i.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ILayer *ModflowDocument::layer ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Layer::RefPtr layer ( ( i >= _layers.size() ) ? 0x0 : _layers.at ( i ) );
  return layer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::dirtyState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->flags ( Usul::Bits::set ( this->flags(), Modflow::Flags::DIRTY, state ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::dirtyState() const
{
  USUL_TRACE_SCOPE;
  return Usul::Bits::has ( this->flags(), Modflow::Flags::DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::flags ( unsigned int bits )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = bits;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::flags() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _flags;
}
