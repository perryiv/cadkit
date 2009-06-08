
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/ElevationGroup.h"
#include "Minerva/Core/ElevationData.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/Color.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Types/Types.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include <set>
#include <cstring>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( ElevationGroup );


///////////////////////////////////////////////////////////////////////////////
//
//  Typedef.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::set<float> NoDataValues;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::ElevationGroup() : 
  BaseClass ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::ElevationGroup ( const ElevationGroup& rhs ) : BaseClass ( rhs )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::~ElevationGroup()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ElevationGroup::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new ElevationGroup ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster data as elevation data.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::IElevationData::RefPtr ElevationGroup::elevationData ( 
  double minLon,
  double minLat,
  double maxLon,
  double maxLat,
  unsigned int width,
  unsigned int height,
  unsigned int level,
  Usul::Jobs::Job* job,
  Usul::Interfaces::IUnknown* caller )
{
  Layers layers;
  this->layers ( layers );
  return this->_elevationData ( layers, minLon, minLat, maxLon, maxLat, width, height, level, job, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster data as elevation data.
//
///////////////////////////////////////////////////////////////////////////////

ElevationGroup::IElevationData::RefPtr ElevationGroup::_elevationData ( 
  const Layers& layers,
  double minLon,
  double minLat,
  double maxLon,
  double maxLat,
  unsigned int width,
  unsigned int height,
  unsigned int level,
  Usul::Jobs::Job* job,
  Usul::Interfaces::IUnknown* caller )
{
  // Are we in range?
  if ( false == this->isInLevelRange ( level ) )
    return IElevationData::RefPtr ( 0x0 );

  Extents requestExtents ( minLon, minLat, maxLon, maxLat );

  IElevationData::RefPtr answer ( 0x0 );

  for ( Layers::const_iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    RasterLayer::_checkForCanceledJob ( job );

    Minerva::Interfaces::ITileElevationData::QueryPtr tileElevationData ( (*iter).get() );
    if ( tileElevationData.valid() )
    {
      // Query for needed interfaces.
      Usul::Interfaces::ILayer::QueryPtr layer ( tileElevationData );
      Usul::Interfaces::ILayerExtents::QueryPtr le ( tileElevationData );

      // Get the extents.
      const double minLonLayer ( le.valid() ? le->minLon() : -180.0 );
      const double minLatLayer ( le.valid() ? le->minLat() :  -90.0 );
      const double maxLonLayer ( le.valid() ? le->maxLon() :  180.0 );
      const double maxLatLayer ( le.valid() ? le->maxLat() :   90.0 );
      
      Extents e ( minLonLayer, minLatLayer, maxLonLayer, maxLatLayer );
      
      // Should the layer be shown?
      const bool shown ( layer.valid() ? layer->showLayer() : true );
      const bool isLevelRange ( layer.valid() ? layer->isInLevelRange ( level ) : true );
      
      if ( ( true == shown ) && ( true == requestExtents.intersects ( e ) ) && ( true == isLevelRange ) )
      {
        IElevationData::RefPtr elevationData ( tileElevationData->elevationData ( minLon, minLat, maxLon, maxLat, width, height, level, job, caller ) );
        if ( elevationData.valid() )
        {
          if ( false == answer.valid() )
          {
            answer = new Minerva::Core::ElevationData ( width, height );
          }

          for ( unsigned int i = 0; i < width; ++i )
          {
            for ( unsigned int j = 0; j < height; ++j )
            {
              // Get the current value.
              const float value ( elevationData->value ( i, j ) );

              // See if the value is in the list of no data values.
              const bool isNoData ( Usul::Predicates::CloseFloat<IElevationData::ValueType>::compare ( value, elevationData->noDataValue(), 10 ) );

              // Set the value, if it isn't a no data.
              if ( false == isNoData )
              {
                answer->value ( i, j, value );
              }
            }
          }
        }
      }
    }
  }

  return answer;
}
