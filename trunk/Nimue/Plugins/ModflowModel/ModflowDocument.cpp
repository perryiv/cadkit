
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for scene files.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Builders/CellWall.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/Readers/AuxiliaryData.h"
#include "ModflowModel/Readers/BasicPackage.h"
#include "ModflowModel/Readers/BlockCenteredFlow.h"
#include "ModflowModel/Readers/Discretization.h"
#include "ModflowModel/Readers/HeadLevelOutput.h"
#include "ModflowModel/Readers/Recharge.h"
#include "ModflowModel/Readers/Pumping.h"

#include "XmlTree/Predicates.h"
#include "XmlTree/XercesLife.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Group.h"
#include "OsgTools/Visitor.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/Execute.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IAxes.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IStringGridGet.h"
#include "Usul/Interfaces/IStringGridSet.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Policies/Update.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/System/LastError.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "osgUtil/IntersectVisitor"

#include "osgDB/WriteFile"

#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "boost/algorithm/string/trim.hpp"

#include "ogr_api.h"
#include "ogr_spatialref.h"
#include "cpl_error.h"

#include <algorithm>
#include <functional>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <sstream>

using namespace Modflow;

USUL_IMPLEMENT_TYPE_ID ( ModflowDocument );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModflowDocument, ModflowDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize OGR.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      ::OGRRegisterAll();
    }
    ~Init()
    {
      ::OGRCleanupAll();
    }
  } _init;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::ModflowDocument() : BaseClass ( "Modflow Document" ),
  _bounds(),
  _startHeads(),
  _landElev(),
  _bottomElev(),
  _layers(),
  _dataLayer ( 0x0 ),
  _root ( new osg::MatrixTransform ),
  _built ( 0x0 ),
  _flags ( Modflow::Flags::DIRTY | Modflow::Flags::VISIBLE ),
  _colors(),
  _zRange ( INVALID_MIN, INVALID_MAX ),
  _verticalScale ( 1.0 ),
  _cellMargin ( 500 ),
  _builders(),
  _timeStep ( 0 ),
  _numTimeSteps ( 0 ),
  _noData(),
  _sourceCoordinateSystem ( 0x0 ),
  _destinationCoordinateSystem ( 0x0 ),
  _coordinateTransformation ( 0x0 ),
  _attributes(),
  _lengthConversion ( 1 ),
  _offsets(),
  _timeStamps(),
  _cylinderSides ( 40 ),
  _elevations(),
  _xmlDoc ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // Default colors.
  this->color ( Modflow::Names::MIN_ELEVATION,  Color ( 0.0f, 0.0f, 1.0f, 1.0f ) );
  this->color ( Modflow::Names::MAX_ELEVATION,  Color ( 102.0f / 255.0f, 204.0f / 255.0f, 1.0f, 1.0f ) );
  this->color ( Modflow::Names::MIN_HEAD_LEVEL, this->color ( Modflow::Names::MIN_ELEVATION ) );
  this->color ( Modflow::Names::MAX_HEAD_LEVEL, this->color ( Modflow::Names::MAX_ELEVATION ) );
  this->color ( Modflow::Names::HEAD_SURFACE,   Color ( 50.0f / 255.0f, 100.0f / 255.0f, 1.0f, 1.0f ) );

  // Populate the builders.
  _builders[Modflow::Geometry::CELL_TOP]    = new Modflow::Builders::CellWall ( "Builder for Cell Top",    Modflow::Geometry::CELL_TOP );
  _builders[Modflow::Geometry::CELL_BOTTOM] = new Modflow::Builders::CellWall ( "Builder for Cell Bottom", Modflow::Geometry::CELL_BOTTOM );
  _builders[Modflow::Geometry::CELL_NORTH]  = new Modflow::Builders::CellWall ( "Builder for Cell North",  Modflow::Geometry::CELL_NORTH );
  _builders[Modflow::Geometry::CELL_SOUTH]  = new Modflow::Builders::CellWall ( "Builder for Cell South",  Modflow::Geometry::CELL_SOUTH );
  _builders[Modflow::Geometry::CELL_EAST]   = new Modflow::Builders::CellWall ( "Builder for Cell East",   Modflow::Geometry::CELL_EAST );
  _builders[Modflow::Geometry::CELL_WEST]   = new Modflow::Builders::CellWall ( "Builder for Cell West",   Modflow::Geometry::CELL_WEST );
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
  _dataLayer = 0x0;
  _root = 0x0;
  _built = 0x0;
  _builders.clear();
  _noData.clear();
  _attributes.clear();
  _offsets.clear();
  _timeStamps.clear();
  _sourceCoordinateSystem.reset ( 0x0 );
  _destinationCoordinateSystem.reset ( 0x0 );
  _xmlDoc = 0x0;
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
  case Usul::Interfaces::IDirtyState::IID:
    return static_cast < Usul::Interfaces::IDirtyState* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case Usul::Interfaces::IIntersectListener::IID:
    return static_cast < Usul::Interfaces::IIntersectListener * > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode * > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState * > ( this );
  case Modflow::Interfaces::IModflowModel::IID:
    return static_cast < Modflow::Interfaces::IModflowModel * > ( this );
  case Usul::Interfaces::ITimeVaryingData::IID:
    return static_cast < Usul::Interfaces::ITimeVaryingData * > ( this );
  case Usul::Interfaces::IGetBoundingBox::IID:
    return static_cast < Usul::Interfaces::IGetBoundingBox * > ( this );
  case Usul::Interfaces::ICoordinateTransformOSG::IID:
    return static_cast < Usul::Interfaces::ICoordinateTransformOSG * > ( this );
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
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
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _xmlDoc.valid() )
  {
    _xmlDoc->write ( name );
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
  using Usul::Factory::TypeCreator;

  // Scope the directory change.
  Usul::Scope::CurrentDirectory cwd ( Usul::File::directory ( file, false ), false );

  // Add readers to a factory.
  ObjectFactory factory;
  factory.add ( new TypeCreator<Modflow::Readers::Discretization,Usul::Base::Referenced>    ( Modflow::Readers::DISCRETIZATION      ) );
  factory.add ( new TypeCreator<Modflow::Readers::BasicPackage,Usul::Base::Referenced>      ( Modflow::Readers::BASIC_PACKAGE       ) );
  factory.add ( new TypeCreator<Modflow::Readers::Recharge,Usul::Base::Referenced>          ( Modflow::Readers::RECHARGE            ) );
  factory.add ( new TypeCreator<Modflow::Readers::Pumping,Usul::Base::Referenced>           ( Modflow::Readers::PUMPING             ) );
  factory.add ( new TypeCreator<Modflow::Readers::BlockCenteredFlow,Usul::Base::Referenced> ( Modflow::Readers::BLOCK_CENTERED_FLOW ) );
  factory.add ( new TypeCreator<Modflow::Readers::HeadLevelOutput,Usul::Base::Referenced>   ( Modflow::Readers::HEAD_LEVEL_OUTPUT   ) );
  factory.add ( new TypeCreator<Modflow::Readers::AuxiliaryData,Usul::Base::Referenced>     ( Modflow::Readers::AUXILIARY_DATA      ) );

  // Initialize and finalize use of xerces.
  XmlTree::XercesLife life;

  // Open the input file.
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( file );

  // Look for a coordinate system.
  this->_setCoordinateSystem ( document.get() );

  // Find all important files.
  Children files ( document->find ( "file", true ) );

  // Find discretization file.
  Children::iterator discretization ( std::find_if ( files.begin(), files.end(), XmlTree::HasAttribute ( "type", "discretization" ) ) );
  if ( files.end() == discretization )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 1994992467: Add discretization file to: ", file );

  // Capture the number of files.
  const unsigned int numFiles ( files.size() );

  // Read discretization file.
  this->_read ( factory, discretization->get(), 0x0 );

  // Set progress bar.
  unsigned int count ( 0 );
  this->setProgressBar ( true, ++count, numFiles, progress );

  // Remove discretization file from list.
  files.erase ( discretization );

  // Read the remaining files.
  for ( unsigned int i = 0; i < files.size(); ++i )
  {
    // Read current file.
    this->_read ( factory, files.at ( i ), 0x0 );

    // Progress feedback.
    this->setProgressBar ( true, ++count, numFiles, progress );
  }

  // Make sure we are not modified.
  this->modified ( false );

  // If we get to here then assign the xml-document member.
  _xmlDoc = document;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_read ( ObjectFactory &factory, XmlTree::Node *file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != file )
  {
    XmlTree::Node::Attributes &a ( file->attributes() );
    this->_read ( factory, a["type"], a["name"], a["no_data"], FileAttributes ( a.begin(), a.end() ), progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_read ( ObjectFactory &factory, const std::string &type, const std::string &file, const std::string &noData, const FileAttributes &fa, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make the reader.
  BaseReader::RefPtr reader ( dynamic_cast<BaseReader *> ( factory.create ( type ) ) );
  if ( false == reader.valid() )
    return;

  // Scope the directory change.
  Usul::Scope::CurrentDirectory cwd ( Usul::File::directory ( file, false ), false );

  // Feedback.
  const std::string base ( Usul::File::base ( file ) );
  const std::string ext ( Usul::File::extension ( file ) );
  const std::string path ( Usul::File::fullPath ( Usul::Strings::format ( base, '.', ext ) ) );
  this->setStatusBar ( "Reading: " + path, progress );

  // Is there a "no data" value?
  if ( false == noData.empty() )
  {
    reader->noDataSet ( this, static_cast < long > ( Modflow::Constants::NO_DATA_MULTIPLIER * Usul::Strings::fromString<double> ( noData ) ) );
  }

  // Set the attributes.
  reader->fileAttributes ( fa );

  // Set registry path.
  Usul::Registry::Node::Path regNodePath;
  regNodePath.push_back ( Usul::Registry::Sections::DOCUMENT_SETTINGS );
  regNodePath.push_back ( this->registryTagName() );
  regNodePath.push_back ( type );
  regNodePath.push_back ( Usul::Registry::Database::instance().convertToTag ( file ) );
  reader->registryPath ( regNodePath );

  // Read the file.
  reader->read ( this, path, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::clear ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _bounds.clear(),
  _startHeads.clear(),
  _landElev.second.clear(),
  _bottomElev.clear();
  this->_clearLayers();

  // Clear the data-layer.
  if ( true == _dataLayer.valid() )
    _dataLayer->clear();

  // Clear the scene.
  OsgTools::Group::removeAllChildren ( _root.get() );
  OsgTools::Group::removeAllChildren ( _built.get() );

  // Clear attributes.
  _attributes.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the layers.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_clearLayers()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

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
  return this->filtersOpen();
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
  Guard guard ( this );

  // Should never happen...
  if ( false == _root.valid() )
    _root = new osg::MatrixTransform;

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

  // Make copies.
  Layers layers;
  Layer::RefPtr dataLayer ( 0x0 );
  Attributes attributes;
  double cellMargin ( 0 );
  {
    Guard guard ( this );
    layers = _layers;
    dataLayer = _dataLayer;
    attributes = _attributes;
    cellMargin = this->getCellMargin();
  }

  // Make a new group.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Loop through the layers.
  {
    const unsigned int num ( layers.size() );
    for ( unsigned int i = 0; i < num; ++i )
    {
      Layer::RefPtr layer ( layers.at(i) );
      layer->margin ( cellMargin, cellMargin );
      group->addChild ( layer->buildScene ( this, caller ) );
    }
  }

  // You are now setting the margin when you set the layers, but you need a way to update the scene.

  // Add the data-layer.
  if ( true == dataLayer.valid() )
  {
    dataLayer->margin ( cellMargin, cellMargin );
    group->addChild ( dataLayer->buildScene ( this, caller ) );
  }

  // Loop through the attributes.
  {
    const unsigned int num ( attributes.size() );
    for ( unsigned int i = 0; i < num; ++i )
    {
      Attribute::RefPtr attribute ( attributes.at(i) );
      group->addChild ( attribute->buildScene ( this, 0x0, caller ) );
    }
  }

  // Transform coordinates.
  {
    osg::ref_ptr<osg::NodeVisitor> visitor (
      OsgTools::MakeVisitor<osg::Geode>::make (
        Usul::Adaptors::bind1 ( caller, Usul::Adaptors::memberFunction (
          this, &ModflowDocument::_transformCoordinates ) ) ) );
    group->accept ( *visitor );
  }

  // Assign the new group to our staging area.
  {
    Guard guard ( this );
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
  Guard guard ( this );

  // Update the layers and attributes.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  Usul::Functions::executeMemberFunctions ( _layers, &Layer::update, me.get() );
  Usul::Functions::executeMemberFunctions ( _attributes, &Attribute::update, me.get() );

  // Get the interfaces needed for displaying the time steps.
  Usul::Interfaces::ITimeVaryingData::QueryPtr timeData ( me.get() );
  Usul::Interfaces::ITextMatrix::QueryPtr textMatrix ( caller );
  if ( ( true == timeData.valid() ) && ( true == textMatrix.valid() ) )
  {
    const unsigned int step ( timeData->getCurrentTimeStep() );
    const unsigned int total ( timeData->getNumberOfTimeSteps() );

    const osg::Vec4f fgColor ( 1, 1, 1, 1 );
    const osg::Vec4f bgColor ( 0, 0, 0, 1 );
    const unsigned int y ( 10 ), x ( 10 );

    std::string out ( Usul::Strings::format ( "Time Step: ", step + 1, " of ", total ) );

    TimeStamp stamp ( 0 );
    if ( true == this->timeStampGet ( step, stamp ) )
    {
      out += Usul::Strings::format ( ", Year: ", stamp );
    }

    textMatrix->setText ( x, y, out, fgColor, bgColor );
  }

  // Are we dirty?
  if ( false == this->dirtyState() )
    return;

  // Should never happen...
  if ( false == _root.valid() )
    _root = new osg::MatrixTransform;

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

    // However, the bounding sphere is dirty.
    _root->dirtyBound();
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
  Guard guard ( this );

  // Clear existing layers.
  this->_clearLayers();

  // Loop through the layers.
  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  for ( Layers::iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    Modflow::Model::Layer::RefPtr layer ( *iter );
    if ( true == layer.valid() )
    {
      // Add the layer.
      _layers.push_back ( layer );

      // Tell this layer it belongs to us.
      layer->document ( me.get() );

      // Set the layer's cell margin.
      const double margin ( this->getCellMargin() );
      layer->margin ( margin, margin );
    }
  }

  // Set z-range.
  _zRange.set ( INVALID_MIN, INVALID_MAX );
  for ( Layers::const_iterator i = _layers.begin(); i != _layers.end(); ++i )
  {
    Layers::value_type layer ( *i );
    if ( true == layer.valid() )
    {
      const Vec2d zRange ( layer->zRange() );
      _zRange[0] = Usul::Math::minimum ( _zRange[0], zRange[0] );
      _zRange[1] = Usul::Math::maximum ( _zRange[1], zRange[1] );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layers.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Layers &ModflowDocument::layers()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
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
  Guard guard ( this );
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
  Guard guard ( this );
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
  Guard guard ( this );
  Layer::RefPtr layer ( _layers.empty() ? 0x0 : _layers.front() );
  return ( ( layer.valid() ) ? Vec2ui ( layer->gridSize() ) : Vec2ui ( 0, 0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell size.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Vec2d ModflowDocument::cellSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Layer::RefPtr layer ( _layers.empty() ? 0x0 : _layers.front() );
  return ( ( layer.valid() ) ? Vec2d ( layer->cellSize() ) : Vec2d ( 0, 0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::dirtyState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Don't set to same state.
  if ( this->dirtyState() == state )
    return;

  // Set local flag.
  this->flags ( Usul::Bits::set ( this->flags(), Modflow::Flags::DIRTY, state ) );

  // Generate a re-draw.
  this->requestRedraw();
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
  Guard guard ( this );
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
  Guard guard ( this );
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template
  <
    class TreeNodeType,
    class LayersType,
    class AttributesType
  >
  TreeNodeType *getChildNode ( unsigned int i,
                               LayersType &layers,
                               AttributesType &attributes,
                               const ModflowDocument &doc )
  {
    USUL_TRACE_SCOPE_STATIC;

    if ( i < layers.size() )
    {
      return layers.at(i).get();
    }
    else if ( i < doc.getNumChildNodes() )
    {
      return attributes.at ( i - layers.size() );
    }
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child at the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode::RefPtr ModflowDocument::getChildNode ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef Usul::Interfaces::ITreeNode TreeNodeType;
  return Helper::getChildNode<TreeNodeType> ( i, _layers, _attributes, *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _layers.size() + _attributes.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the node name.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::setTreeNodeName ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node name.
//
///////////////////////////////////////////////////////////////////////////////

std::string ModflowDocument::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node name.
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when there is an intersection.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef Modflow::Base::BaseObject BaseObject;
  typedef BaseObject::UserData UserData;
  typedef BaseObject::StringGrid StringGrid;
  typedef BaseObject::StringRow StringRow;
  typedef Usul::Interfaces::IStringGridGet IStringGridGet;
  typedef Usul::Interfaces::IStringGridSet IStringGridSet;

  // Get interface from delegate.
  IStringGridSet::QueryPtr setter ( this->delegate() );
  if ( false == setter.valid() )
    return;

  // Loop over the node path and collect data.
  StringGrid grid;
  const osg::NodePath &path ( hit.nodePath );
  for ( osg::NodePath::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    // Get basic string data.
    osg::ref_ptr<osg::Node> node ( *i );
    osg::ref_ptr<UserData> ud ( ( true == node.valid() ) ? dynamic_cast < UserData * >  ( node->getUserData() ) : 0x0 );
    IStringGridGet::QueryPtr getter ( ( true == ud.valid() ) ? ( ud->value().get() ) : 0x0 );
    if ( true == getter.valid() )
    {
      StringGrid local;
      getter.get()->getStringGrid ( local );
      grid.insert ( grid.end(), local.begin(), local.end() );
    }

    // Are there vertices?
    if ( false == hit.indexList.empty() )
    {
      // Get the vertices involved.
      const unsigned int vertexStart ( hit.indexList.front() );

      // Get the attribute.
      Modflow::Attributes::Attribute::RefPtr attr ( ( true == ud.valid() ) ?
        dynamic_cast < Modflow::Attributes::Attribute * > ( ud->value().get() ) : 0x0 );
      if ( true == attr.valid() )
      {
        // Get the cell.
        const Modflow::Model::Cell *cell ( attr->getCellAtVertex ( vertexStart ) );
        if ( 0x0 != cell )
        {
          // Get the cell's properties.
          grid.push_back ( Modflow::Base::BaseObject::makeStringRow ( "Cell X",      Usul::Strings::toString ( cell->x() ) ) );
          grid.push_back ( Modflow::Base::BaseObject::makeStringRow ( "Cell Y",      Usul::Strings::toString ( cell->y() ) ) );
          grid.push_back ( Modflow::Base::BaseObject::makeStringRow ( "Cell Top",    Usul::Strings::toString ( cell->top() ) ) );
          grid.push_back ( Modflow::Base::BaseObject::makeStringRow ( "Cell Bottom", Usul::Strings::toString ( cell->bottom() ) ) );
          grid.push_back ( Modflow::Base::BaseObject::makeStringRow ( "Cell Row",    Usul::Strings::toString ( cell->indices()[0] ) ) );
          grid.push_back ( Modflow::Base::BaseObject::makeStringRow ( "Cell Column", Usul::Strings::toString ( cell->indices()[1] ) ) );
        }
      }
    }
  }

  // Set the grid data in the delegate.
  setter->setStringGrid ( grid );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Color ModflowDocument::color ( const std::string &name ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Colors::const_iterator i ( _colors.find ( name ) );
  return ( ( _colors.end() == i ) ? Color() : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::color ( const std::string &name, const Color &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _colors[name] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Vec2d ModflowDocument::zRange() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _zRange;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vertical scale.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::setVerticalScale ( double s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // To avoid excessive rebuilds, set only if it's different.
  if ( s != _verticalScale )
  {
    _verticalScale = s;

    if ( true == _root.valid() )
    {
      // Set the z-scale.
      _root->setMatrix ( osg::Matrixd::scale ( 1, 1, s ) );

      // If we're close to 1 then we don't need GL_NORMALIZE.
      typedef Usul::Predicates::CloseFloat<double> CloseFloat;
      const bool normalize ( false == CloseFloat::compare ( 1, s, 10 ) );
      OsgTools::State::StateSet::setNormalize ( _root.get(), normalize );
    }

    this->requestRedraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertical scale.
//
///////////////////////////////////////////////////////////////////////////////

double ModflowDocument::getVerticalScale() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _verticalScale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cell margin.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::setCellMargin ( double margin )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // To avoid excessive rebuilds, set only if it's different.
  if ( margin != _cellMargin )
  {
    _cellMargin = margin;
    this->dirtyState ( true );
    this->requestRedraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell margin.
//
///////////////////////////////////////////////////////////////////////////////

double ModflowDocument::getCellMargin() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _cellMargin;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the builder.
//
///////////////////////////////////////////////////////////////////////////////

Modflow::Builders::Geometry *ModflowDocument::builder ( Modflow::Geometry::ID id ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  BuilderMap::const_iterator i ( _builders.find ( id ) );
  return ( ( _builders.end() == i ) ? 0x0 : i->second.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time step.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::setCurrentTimeStep ( unsigned int step )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( step < _numTimeSteps )
  {
    _timeStep = step;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current time step.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::getCurrentTimeStep() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _timeStep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of time steps.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::setNumberOfTimeSteps ( unsigned int num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _numTimeSteps = num;

  if ( _timeStep >= _numTimeSteps )
  {
    _timeStep = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of time steps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::getNumberOfTimeSteps() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _numTimeSteps;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the "no data" value.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::noDataSet ( const std::string &name, long value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _noData[name] = NoDataValue ( true, value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given value a "no data"?
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::NoDataValue ModflowDocument::noDataGet ( const std::string &name ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  NoDataValues::const_iterator i ( _noData.find ( name ) );
  return ( ( _noData.end() == i ) ? NoDataValue ( false, 0 ) : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the coordinate system, if one is available.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_setCoordinateSystem ( XmlTree::Node *node )
{
  USUL_TRACE_SCOPE;

  // Handle bad input.
  if ( 0x0 == node )
    return;

  // Look for a coordinate system.
  XmlTree::Node::Children coordinateSystems ( node->find ( "coordinate_system", true ) );
  XmlTree::Node::RefPtr coordinateSystem ( ( true == coordinateSystems.empty() ) ? 0x0 : coordinateSystems.front() );
  if ( true == coordinateSystem.valid() )
  {
    // Make a new spatial reference system.
    CoordinateSystemPtr spatialReference ( new OGRSpatialReference );

    XmlTree::Node::Attributes &attr ( coordinateSystem->attributes() );
    const std::string code ( attr["numeric_code"] );
    const std::string wkt  ( attr["well_known_text"] );

    if ( false == code.empty() )
    {
      const unsigned int value ( Usul::Strings::fromString<unsigned int> ( code ) );
      if ( CE_None == spatialReference->importFromEPSG ( value ) )
        this->destinationCoordinateSystem ( spatialReference.release() );
    }

    else if ( false == wkt.empty() )
    {
      std::vector<char> text ( wkt.size() + 1, '\0' );
      std::copy ( wkt.begin(), wkt.end(), text.begin() );
      char *ptr ( &text[0] );
      if ( CE_None == spatialReference->importFromWkt ( &ptr ) )
        this->destinationCoordinateSystem ( spatialReference.release() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::addAttribute ( Attribute *attribute )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != attribute )
  {
    Guard guard ( this );
    _attributes.push_back ( attribute );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length conversion.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::lengthConversion ( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _lengthConversion = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the length conversion.
//
///////////////////////////////////////////////////////////////////////////////

double ModflowDocument::lengthConversion() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _lengthConversion;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the offset.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::offsetSet ( const std::string &name, const ModflowDocument::Vec3d &value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _offsets[name] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the offset.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::Vec3d ModflowDocument::offsetGet ( const std::string &name, const ModflowDocument::Vec3d &defaultValue ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Offsets::const_iterator i ( _offsets.find ( name ) );
  return ( ( _offsets.end() != i ) ? i->second : defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time-stamp.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::timeStampSet ( unsigned int i, const TimeStamp &stamp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _timeStamps[i] = stamp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time-stamp.
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::timeStampGet ( unsigned int i, TimeStamp &stamp ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  TimeStamps::const_iterator itr ( _timeStamps.find ( i ) );
  if ( _timeStamps.end() == itr )
  {
    return false;
  }

  else
  {
    stamp = itr->second;
    return true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of time-stamps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::numTimeStamps() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _timeStamps.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data-layer.
//
///////////////////////////////////////////////////////////////////////////////

Modflow::Model::Layer *ModflowDocument::dataLayer()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Check state.
  if ( false == _dataLayer.valid() )
  {
    // Get top layer.
    Layer::RefPtr topLayer ( ( false == _layers.empty() ) ? _layers.at ( 0 ) : 0x0 );
    if ( true == topLayer.valid() )
    {
      // Allocate data-layer.
      const Layer::Vec2ui gridSize ( topLayer->gridSize() );
      _dataLayer = new Layer ( "Data Layer", gridSize[0], gridSize[1], topLayer->cellSize() );
    }
  }

  // Return what we have, which may be null.
  return _dataLayer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::addView ( BaseClass::View* view )
{
  USUL_TRACE_SCOPE;

  // Call the base class.
  BaseClass::addView ( view );

  Usul::Interfaces::IAxes::QueryPtr axes ( view );
  if ( axes.valid() )
    axes->axesLabels ( "E", "N", "Up" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding-box (Usul::Interfaces::IGetBoundingBox).
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox ModflowDocument::getBoundingBox() const
{
  USUL_TRACE_SCOPE;

  Usul::Math::Vec2ui gridSize ( this->gridSize() );
  Usul::Math::Vec2d  cellSize (this->cellSize() );

  Guard guard ( this );

  // Width and height of grid.
  const double width  ( gridSize[1] * cellSize[1] * _lengthConversion );
  const double height ( gridSize[0] * cellSize[0] * _lengthConversion );

  const double maximum ( Usul::Math::maximum ( width, height ) );

  Offsets::const_iterator iter ( _offsets.find ( Modflow::Attributes::CELL_GRID_ORIGIN ) );
  Usul::Math::Vec3d offset ( ( iter != _offsets.end() ) ? iter->second : Usul::Math::Vec3d ( 0.0, 0.0, 0.0 ) );

  // Lower left and upper right points.
  Usul::Math::Vec3d v0 ( offset[0],         offset[1],          offset[2] + _zRange[0] );
  Usul::Math::Vec3d v1 ( offset[0] + width, offset[1] + height, offset[2] + _zRange[1] );

  // Vertical scale.
  v0[2] *= _verticalScale;
  v1[2] *= _verticalScale;

  // Make sure that the height is about our minimum threshold.
  const double zHeight ( v1[2] - v0[2] );
  if ( zHeight < maximum )
  {
    const double half ( maximum / 2.0 );
    v0[2] = -half;
    v1[2] =  half;
  }

  // Calculate the center.  This is for the work around below...
  Usul::Math::Vec2d center;
  center[0] = ( v0[0] + v1[0] ) / 2;
  center[1] = ( v0[1] + v1[1] ) / 2;

  // Make the distance equal in the x and y direction.
  // I think there is a bug in the y-axes clip plane.  This is a work around.
  v0[0] = center[0] - maximum;
  v0[1] = center[1] - maximum;
  v1[0] = center[0] + maximum;
  v1[1] = center[1] + maximum;

  return osg::BoundingBox ( v0[0], v0[1], v0[2], v1[0], v1[1], v1[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of sides to use when making cylinders.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::numCylinderSides ( unsigned int sides )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _cylinderSides = sides;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of sides to use when making cylinders.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModflowDocument::numCylinderSides() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _cylinderSides;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation for the position in the grid.
//
///////////////////////////////////////////////////////////////////////////////

double ModflowDocument::elevation ( unsigned int row, unsigned int column ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _elevations.empty() )
    throw std::runtime_error ( "Error 2555821633: No elevations present" );

  const unsigned int numCols ( this->gridSize()[1] );
  const unsigned int index ( ( row * numCols ) + column );
  if ( index >= _elevations.size() )
  {
    throw std::runtime_error ( Usul::Strings::format ( "Error 3681533051: No elevation data at grid position (", row, ",", column, ")" ) );
  }

  const double top ( _elevations.at ( index ) );
  return top;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevations.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::elevations ( const GridData &data )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _elevations = data;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the destination coordinate system.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::CoordinateSystem* ModflowDocument::destinationCoordinateSystem() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _destinationCoordinateSystem.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the destination coordinate system.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::destinationCoordinateSystem( CoordinateSystem* coordinateSystem )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _destinationCoordinateSystem.reset ( coordinateSystem );

  if ( 0x0 != _destinationCoordinateSystem.get() && 0x0 != this->sourceCoordinateSystem() )
    _coordinateTransformation.reset ( ::OGRCreateCoordinateTransformation ( this->sourceCoordinateSystem(), _destinationCoordinateSystem.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the destination source system.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDocument::CoordinateSystem* ModflowDocument::sourceCoordinateSystem() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _sourceCoordinateSystem.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the destination source system.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::sourceCoordinateSystem( CoordinateSystem* coordinateSystem )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _sourceCoordinateSystem.reset ( coordinateSystem );

  if ( 0x0 != this->destinationCoordinateSystem() && 0x0 != _sourceCoordinateSystem.get() )
    _coordinateTransformation.reset ( ::OGRCreateCoordinateTransformation ( _sourceCoordinateSystem.get(), this->destinationCoordinateSystem() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Transform one coordinate.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::transformCoordinate ( osg::Vec3f& v ) const
{
  USUL_TRACE_SCOPE;

  // Typedef.
  typedef osg::Vec3f::value_type ValueType;

  // Get the transform.
  CoordinateTransformation* transform ( Usul::Threads::Safe::get ( this->mutex(), _coordinateTransformation.get() ) );

  // Make sure we have a valid transform.
  if ( 0x0 != transform )
  {
    double x ( v[0] ), y ( v[1] ), z ( v[2] );
    if ( false == transform->Transform ( 1, &x, &y, &z ) )
      std::cout << "Warning 2351370480: Failed to transform coordinate " << v[0] << " " << v[1] << " " << v[2] << std::endl;
    else
      v.set ( static_cast<ValueType> ( x ), static_cast<ValueType> ( y ), static_cast<ValueType> ( z ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Transform many coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::transformCoordinates ( osg::Vec3Array& vertices ) const
{
  USUL_TRACE_SCOPE;

  // Typedefs.
  typedef osg::Vec3Array::value_type VertexType;
  typedef VertexType::value_type FloatType;

  // Get the transform.
  CoordinateTransformation* transform ( Usul::Threads::Safe::get ( this->mutex(), _coordinateTransformation.get() ) );

  // Make sure we have a valid transform.
  if ( 0x0 != transform )
  {
    for ( osg::Vec3Array::iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      osg::Vec3Array::reference v ( *iter );
      double x ( v[0] ), y ( v[1] ), z ( v[2] );

      if ( false == transform->Transform ( 1, &x, &y, &z ) )
      {
        std::cout << "Warning 1425477964: Failed to transform coordinate " << v[0] << " " << v[1] << " " << v[2] << std::endl;
      }
      else
      {
        v.set ( static_cast<FloatType> ( x ), static_cast<FloatType> ( y ), static_cast<FloatType> ( z ) );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Transform a geode.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::_transformCoordinates ( osg::Geode *geode, Usul::Interfaces::IUnknown *caller ) const
{
  USUL_TRACE_SCOPE;

  // Check input.
  if ( 0x0 == geode )
    return;

  // Get the needed interface.
  typedef Usul::Interfaces::IPlanetCoordinates IPlanetCoordinates;
  IPlanetCoordinates::QueryPtr planetCoordinates ( caller );
  if ( false == planetCoordinates.valid() )
    return;

  // Needed in the loop below.
  const std::string projection ( this->_wellKnownText() );
  ::OGRSpatialReference source ( projection.c_str() );
  ::OGRSpatialReference destination;
  destination.SetWellKnownGeogCS ( "WGS84" );

  Usul::Math::Vec3d to;

  ::OGRCoordinateTransformation* transform ( ::OGRCreateCoordinateTransformation( &source, &destination ) );
  Usul::Scope::Caller::RefPtr clean ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( transform, ::OCTDestroyCoordinateTransformation ) ) );

  // Loop through the geometries.
  const unsigned int numDrawables ( geode->getNumDrawables() );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    osg::ref_ptr<osg::Geometry> geometry ( dynamic_cast < osg::Geometry * > ( geode->getDrawable ( i ) ) );
    if ( true == geometry.valid() )
    {
      osg::ref_ptr<osg::Vec3Array> vertices ( dynamic_cast < osg::Vec3Array * > ( geometry->getVertexArray() ) );
      if ( true == vertices.valid() )
      {
        // Loop through the vertices.
        for ( osg::Vec3Array::iterator v = vertices->begin(); v != vertices->end(); ++v )
        {
          Usul::Math::Vec3d from ( v->x(), v->y(), v->z() );

          if ( 0x0 != transform )
            transform->Transform ( 1, &from[0], &from[1] );

          planetCoordinates->convertToPlanet ( from, to );
          v->set ( to[0], to[1], to[2] );
        }

        // Dirty the geometry.
        geometry->dirtyBound();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the well-known-text from the document's coordinate system.
//
///////////////////////////////////////////////////////////////////////////////

std::string ModflowDocument::_wellKnownText() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( 0x0 != _destinationCoordinateSystem.get() )
  {
    char *wkt ( 0x0 );
    Usul::Scope::Caller::RefPtr clean ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( wkt, ::OGRFree ) ) );
    if ( CE_None == _destinationCoordinateSystem->exportToWkt ( &wkt ) )
    {
      if ( 0x0 != wkt )
      {
        std::string result ( wkt );
        return result;
      }
    }
  }

  return std::string ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string ModflowDocument::guid() const
{
  USUL_TRACE_SCOPE;
  return Usul::Functions::GUID::generate();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string ModflowDocument::name() const
{
  USUL_TRACE_SCOPE;
  return BaseClass::name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::name ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  BaseClass::name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show or hide the layer.
//
///////////////////////////////////////////////////////////////////////////////

void ModflowDocument::showLayer ( bool b )
{
  USUL_TRACE_SCOPE;
  this->setBooleanState ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the layer shown?
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::showLayer() const
{
  USUL_TRACE_SCOPE;
  return this->getBooleanState();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given level falls within this layer's range of levels.
//
///////////////////////////////////////////////////////////////////////////////

bool ModflowDocument::isInLevelRange ( unsigned int level ) const
{
  USUL_TRACE_SCOPE;
  return true;
}
