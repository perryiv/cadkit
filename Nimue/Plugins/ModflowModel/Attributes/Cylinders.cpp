
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
//  Cylinders attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/Cylinders.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Model/Layer.h"

#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geode"
#include "osg/Geometry"

#include <stdexcept>

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( Cylinders );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Cylinders::Cylinders ( const std::string &name, bool needToInit, const RegistryPath &regPath ) : 
  BaseClass ( name, 0x0, regPath ),
  _switch ( new osg::Switch ),
  _steps ( new osg::Group ),
  _values(),
  _shapes ( new OsgTools::ShapeFactory ),
  _needToInit ( needToInit ),
  _heightScale ( 1 ),
  _radiusScale ( 1 ),
  _sortTriangles ( false )
{
  USUL_TRACE_SCOPE;
  _switch->setAllChildrenOff();

  // Default colors.
  Color color ( 226.0f / 255, 226.0f / 255, 0, 1 );
  this->minColor ( color );
  this->maxColor ( color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Cylinders::~Cylinders()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Cylinders::_destroy ), "3449176655" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::_destroy()
{
  USUL_TRACE_SCOPE;
  _switch = 0x0;
  _steps = 0x0;
  _values.clear();
  _shapes = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  OsgTools::Group::removeAllChildren ( _switch.get() );
  OsgTools::Group::removeAllChildren ( _steps.get() );

  _values.clear();

  if ( true == _shapes.valid() )
    _shapes->clear();

  BaseClass::clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Cylinders::buildScene ( Modflow::ModflowDocument *document, Modflow::Model::Layer *, IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no document.
  if ( 0x0 == document )
    return new osg::Group;

  // Do this once.
  if ( true == _needToInit )
  {
    TimeStamp lastStamp ( 0 );
    const bool hasStamp ( document->timeStampGet ( document->getNumberOfTimeSteps() - 1, lastStamp ) );
    if ( true == hasStamp )
    {
      // Causes weird exception in Nvidia driver and a Windows structured 
      // exception -- access violation. Only happens in debug build.
      #ifndef _DEBUG
      this->fill ( lastStamp );
      #endif
    }

    // Set this now.
    _needToInit = false;
  }

  // Just return existing switch if we're not dirty.
  if ( false == this->dirtyState() )
    return _switch.get();

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // Clear the switch.
  OsgTools::Group::removeAllChildren ( _switch.get() );

  // If the conditions are right...
  if ( true == this->visible() )
  {
    // The first time...
    if ( 0 == _steps->getNumChildren() )
    {
      // Build the time-steps.
      Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Cylinders::_buildTimeSteps ), document, "2105112676" );

      // Set the color.
      const Color color ( 0.5f * ( this->minColor() + this->maxColor() ) );
      const osg::Vec4f channel ( color[0], color[1], color[2], color[3] );
      OsgTools::State::StateSet::setMaterial ( _switch.get(), channel, channel, channel[3] );
      OsgTools::State::StateSet::setMaterial ( _steps.get(), channel, channel, channel[3] );

      // Sort triangles?
      if ( true == _sortTriangles )
      {
        OsgTools::DisplayLists displayLists ( false );
        displayLists ( _switch.get() );
        displayLists ( _steps.get() );

        osg::ref_ptr < OsgTools::Callbacks::SetSortToFrontCallback > visitor ( new OsgTools::Callbacks::SetSortToFrontCallback );
        _switch->accept ( *visitor );
        _steps->accept  ( *visitor );
      }
    }

    // Add all the children from the group to the switch.
    OsgTools::Group::addAllChildren ( _steps.get(), _switch.get() );
  }

  // Set new scene and return it.
  this->_setScene ( _switch.get() );
  return BaseClass::buildScene ( document, 0x0, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the time-steps.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::_buildTimeSteps ( Modflow::ModflowDocument *document )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Clear the existing steps.
  OsgTools::Group::removeAllChildren ( _steps.get() );

  // Handle bad input.
  if ( 0x0 == document )
    return;

  // Get the length scale and offset.
  const double lengthScale ( document->lengthConversion() );
  const Usul::Math::Vec3d offset ( document->offsetGet ( Modflow::Attributes::CELL_GRID_ORIGIN ) );

  // Loop through the time-steps.
  const unsigned int numSteps ( document->getNumberOfTimeSteps() );
  for ( unsigned int t = 0; t < numSteps; ++t )
  {
    // Add the time step.
    _steps->addChild ( this->_buildTimeStep ( t, document, offset, lengthScale ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the time-step.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Cylinders::_buildTimeStep ( unsigned int timeStep, Modflow::ModflowDocument *document, const Vec3d &offset, double scale ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

  // Handle bad state.
  if ( false == _shapes.valid() )
    return geode.release();

  // Handle bad input.
  if ( 0x0 == document )
    return geode.release();

  // Get the time-stamp for this step.
  TimeStamp stamp ( 0 );
  const bool hasTimeStamp ( document->timeStampGet ( timeStep, stamp ) );
  if ( false == hasTimeStamp )
    return geode.release();

  // Get the vector.
  ValuesPtr data ( this->_getValues ( stamp ) );
  if ( 0x0 == data.get() )
    return geode.release();

  // Get grid size.
  const unsigned int numRows ( document->gridSize()[0] );
  const unsigned int numCols ( document->gridSize()[1] );

  // Determine cylinder radius.
  const Usul::Math::Vec2d cellSize ( document->cellSize() );
  const double radius ( _radiusScale * 0.5 * scale * Usul::Math::absolute ( Usul::Math::minimum ( cellSize[0], cellSize[1] ) - document->getCellMargin() ) );

  // The number of sides on the cylinder.
  const unsigned int sides ( document->numCylinderSides() );

  // Needed to offset the cylinders.
  const double maxY ( cellSize[1] * numRows );

  // Get the "no data" value.
  const Modflow::ModflowDocument::NoDataValue noData ( document->noDataGet ( this->name() ) );

  // Loop through the values.
  for ( unsigned int i = 0; i < numRows; ++i )
  {
    // Get the y-coordinate for this row.
    const double y ( offset[1] + ( scale * ( maxY - ( cellSize[1] * i ) ) ) );

    for ( unsigned int j = 0; j < numCols; ++j )
    {
      // Some functions below may throw.
      try
      {
        // Calculate the index.
        const unsigned int index ( i * numCols + j );

        // Get the value.
        const double value ( data->at ( index ) );

        // See if the value is "no data".
        const bool isNoData ( ( true == noData.first ) ? ( noData.second == ( static_cast<long> ( value * Modflow::Constants::NO_DATA_MULTIPLIER ) ) ) : false );
        if ( false == isNoData )
        {
          // Get the x-coordinates for this cell column.
          const double x ( offset[0] + ( scale * ( cellSize[0] * j ) ) );

          // Get the elevation for this cell position. It should be the top of the top-most cell's body.
          const double elevation ( scale * document->elevation ( i, j ) );

          // Calculate both z-values.
          const double z0 ( elevation );
          const double z1 ( elevation + ( value * _heightScale ) );

          // The base and top of the cylinder. Find min and max because the 
          // scale and/or value might be negative, and we don't want 
          // inside-out cylinders.
          osg::Vec3f base ( x, y, Usul::Math::minimum ( z0, z1 ) );
          osg::Vec3f top  ( x, y, Usul::Math::maximum ( z0, z1 ) );
          
          // Transform the vertices.
          document->transformCoordinate ( base );
          document->transformCoordinate ( top );

          // Add the cylinder.
          geode->addDrawable ( _shapes->cylinder ( radius, sides, base, top, !_sortTriangles ) );
        }
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2036319300" );
    }
  }

  // Return the scene.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update this attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::update ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get the interface we need.
  Usul::Interfaces::ITimeVaryingData::QueryPtr data ( caller );
  if ( false == data.valid() )
    return;

  // Handle no children.
  const unsigned int numChildren ( _switch->getNumChildren() );
  if ( 0 == numChildren )
    return;

  // Handle out of range steps.
  const unsigned int step ( Usul::Math::minimum ( data->getCurrentTimeStep(), numChildren - 1 ) );

  // Set the child.
  _switch->setSingleChildOn ( step );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add data.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::addValues ( const TimeStamp &start, ValuesPtr v, const TimeStamp &num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for ( TimeStamp i = 0; i < num; ++i )
  {
    const TimeStamp t ( start + i );

    // Make sure there is not one already.
    if ( 0x0 != _values[t].get() )
    {
      throw std::runtime_error ( Usul::Strings::format ( "Error 3493393858: Values already exist for time-stamp = ", t ) );
    }

    _values[t] = v;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add data.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::addValues ( const TimeStamp &start, const Values &values, const TimeStamp &num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  ValuesPtr v ( new Values ( values.begin(), values.end() ) );
  this->addValues ( start, v, num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

const Cylinders::ValuesPtr Cylinders::_getValues ( const TimeStamp &stamp ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  ValuesMap::const_iterator i ( _values.find ( stamp ) );
  return ( ( _values.end() == i ) ? ValuesPtr ( static_cast < ValuesPtr::value_type * > ( 0x0 ) ) : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->visible ( state );
  _switch->setNodeMask ( ( true == state ) ? 0xFFFFFFFF : 0 );
  BaseClass::requestActiveDocumentRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill in missing values for all time-stamps starting with the first one.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::fill ( const TimeStamp &lastStamp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // We're going to fill this map.
  ValuesMap newMap;

  // Get the first entry in the existing map. It's sorted by time-stamp so 
  // it should be the earliest time.
  const TimeStamp firstStamp ( _values.begin()->first );
  ValuesPtr lastGoodValues ( _values.begin()->second );

  // The first one can't be null.
  if ( 0x0 == lastGoodValues.get() )
    return;

  // Loop through the years from the first to the given last.
  for ( TimeStamp i = firstStamp; i <= lastStamp; ++i )
  {
    // Get the i'th entry.
    ValuesPtr values ( _values[i] );

    // Update last good values.
    if ( 0x0 != values.get() )
    {
      lastGoodValues = values;
    }

    // Populate the new map.
    newMap[i] = lastGoodValues;
  }

  // Replace existing map with new one.
  _values.clear();
  _values = newMap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the height scale.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::heightScale ( const std::string &scale )
{
  USUL_TRACE_SCOPE;

  if ( false == scale.empty() )
  {
    this->heightScale ( Usul::Convert::Type<std::string,double>::convert ( scale ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the height scale.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::heightScale ( double scale )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _heightScale = scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radius scale.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::radiusScale ( const std::string &scale )
{
  USUL_TRACE_SCOPE;

  if ( false == scale.empty() )
  {
    this->radiusScale ( Usul::Convert::Type<std::string,double>::convert ( scale ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radius scale.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::radiusScale ( double scale )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  scale = Usul::Math::absolute ( scale );

  if ( scale > 0 )
  {
    _radiusScale = scale;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sort the triangles?
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::sortTriangles ( const std::string &sort )
{
  USUL_TRACE_SCOPE;

  if ( false == sort.empty() )
  {
    this->sortTriangles ( Usul::Convert::Type<std::string,bool>::convert ( sort ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sort the triangles?
//
///////////////////////////////////////////////////////////////////////////////

void Cylinders::sortTriangles ( bool sort )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _sortTriangles = sort;
}
