
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to download lines.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/LineJob.h"
#include "Minerva/Layers/OSM/XAPIMapQuery.h"

#include "Minerva/Layers/OSM/Parser.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineJob::LineJob ( Usul::Jobs::Manager* manager, 
                  Cache::RefPtr cache, 
                  const std::string& url, 
                  const Extents& extents, 
                  unsigned int level, 
                  const Predicate& predicate,
                  Usul::Interfaces::IUnknown::RefPtr caller ) : 
  BaseClass ( manager, cache, url, extents, level, predicate ),
  _caller ( caller ),
  _lineStyle ( LineStyle::create ( Usul::Math::Vec4f ( 1.0f, 1.0f, 0.0f, 1.0f ), 2.0 ) )
{
  this->priority ( static_cast<int> ( level ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineJob::~LineJob()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the request.  Check the cache first.
//
///////////////////////////////////////////////////////////////////////////////

void LineJob::_started()
{
  this->_setStatus ( "Started" );

  XAPIMapQuery query ( this->_makeQuery() );

  Lines lines;
  query.makeLinesQuery ( lines, this );

  this->_setStatus ( "Building data objects" );

  this->_buildDataObjects ( lines );

  this->_setStatus ( "Finished" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void LineJob::_buildDataObjects ( const Lines& lines )
{
  LineStyle::RefPtr style ( this->lineStyle() );

  DataObject::RefPtr object ( new DataObject );

  // TODO: change the stride based of the current level. (Or interpolate a NURBS curve and evaluate the desired number of points.)
#ifdef _DEBUG
  const unsigned int stride ( 8 );
#else

  unsigned int stride ( 1 );

  if ( this->level() < 10 )
    stride = 5;
#endif

  // Add all the nodes.
  for ( Lines::const_iterator iter = lines.begin(); iter != lines.end(); ++iter )
  {
    LineString::RefPtr line ( *iter );
    if ( line.valid() )
    {
      // Make a line.
      object->addGeometry ( line->buildGeometry ( style, stride ) );
    }
  }

  object->preBuildScene ( _caller );
  Usul::Interfaces::IUnknown::QueryPtr unknown ( object );
  this->_addData ( unknown );
}


/// Set/get the line style.
void LineJob::lineStyle ( LineStyle::RefPtr style )
{
  Guard guard ( this );
  _lineStyle = style;
}

LineJob::LineStyle::RefPtr LineJob::lineStyle() const
{
  Guard guard ( this );
  return _lineStyle;
}
