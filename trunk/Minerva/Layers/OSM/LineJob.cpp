
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
  BaseClass ( manager, cache, url, extents, predicate ),
  _caller ( caller )
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
  XAPIMapQuery query ( this->_makeQuery() );

  Lines lines;
  query.makeLinesQuery ( lines );

  this->_buildDataObjects ( lines );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void LineJob::_buildDataObjects ( const Lines& lines )
{
  this->_reserveDataSize ( lines.size() );

  // Add all the nodes.
  for ( Lines::const_iterator iter = lines.begin(); iter != lines.end(); ++iter )
  {
    LineString::RefPtr line ( *iter );
    if ( line.valid() )
    {
      // Make a line.
      typedef Minerva::Core::Data::Line Line;
      typedef Minerva::Core::Data::LineStyle LineStyle;

      DataObject::RefPtr object ( new DataObject );
      object->addGeometry ( line->buildGeometry ( LineStyle::create ( Usul::Math::Vec4f ( 1.0f, 1.0f, 0.0f, 1.0f ), 2.0 ) ) );
      object->name ( Usul::Strings::format ( line->tag<std::string> ( "ref" ) ) );
      object->preBuildScene ( _caller );

      Usul::Interfaces::IUnknown::QueryPtr unknown ( object );
      this->_addData ( unknown );
    }
  }
}
