
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WMS layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Core/Layers/RasterLayerWms.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/File/Stats.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Network/WMS.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/ReadFile"

#include "osg/ref_ptr"
#include "osg/Image"

#include <iomanip>
#include <fstream>

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterLayerWms );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const Extents &maxExtents, const std::string &url, const Options &options ) : 
  BaseClass()
{
  USUL_TRACE_SCOPE;

  this->extents ( maxExtents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::RasterLayerWms ( const RasterLayerWms& rhs ) :
  BaseClass ( rhs )
{  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayerWms::~RasterLayerWms()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterLayerWms::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterLayerWms ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::_download ( const std::string& file, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown * )
{
  USUL_TRACE_SCOPE;

  // Get url.
  std::string url ( this->url() );
  if ( true == url.empty() )
    return;

  // Get options.
  Options options ( this->options() );
  if ( true == options.empty() )
    return;

  // Add bounding box.  Use Usul convert for full precision.
  options[Usul::Network::Names::BBOX] = Usul::Strings::format ( 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[0] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.minimum()[1] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[0] ), ',', 
    Usul::Convert::Type<Extents::ValueType,std::string>::convert ( extents.maximum()[1] ) );

  // Add width and height.
  options[Usul::Network::Names::WIDTH]  = Usul::Convert::Type<unsigned int,std::string>::convert ( width  );
  options[Usul::Network::Names::HEIGHT] = Usul::Convert::Type<unsigned int,std::string>::convert ( height );

  // Make wms object and ask it for file extension.
  Usul::Network::WMS wms ( url, file, options.begin(), options.end() );

  std::ostream *stream ( 0x0 );
  Usul::Interfaces::IUnknown::QueryPtr caller ( job );
  wms.download ( this->maxNumAttempts(), stream, caller.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayerWms::deserialize ( const XmlTree::Node& node )
{
  USUL_TRACE_SCOPE;
  
  BaseClass::deserialize ( node );
}
