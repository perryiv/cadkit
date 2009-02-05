
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/GDALReadImage/GDALReadImageComponent.h"

#include "Minerva/Layers/GDAL/Convert.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Components/Factory.h"
#include "Usul/File/Path.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Trace/Trace.h"

#include "gdal.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize Gdal.
//  On Linux each dynamic library that uses GDAL, has to initialize GDAL.  
//  Need to find out why.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __linux

namespace Detail
{
  struct Init
  {
    Init()
    {
      GDALAllRegister();
    }
    ~Init()
    {
      GDALDestroyDriverManager();
    }
  } _init;
}

#endif

#include <algorithm>

USUL_DECLARE_COMPONENT_FACTORY ( GDALReadImageComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GDALReadImageComponent, GDALReadImageComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageComponent::GDALReadImageComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageComponent::~GDALReadImageComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *GDALReadImageComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IReadImageFile::IID:
    return static_cast < Usul::Interfaces::IReadImageFile* > ( this );
#if 0
  case Usul::Interfaces::IWriteImageFile::IID:
    return static_cast < Usul::Interfaces::IWriteImageFile* > ( this );
#endif
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GDALReadImageComponent::canRead ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( /*"jpg" == ext ||*/ "gif" == ext || "tif" == ext || "tiff" == ext || "png" == ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a filename and return an image (IReadImageFile).
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageComponent::ImagePtr GDALReadImageComponent::readImageFile ( const std::string& filename ) const
{
  USUL_TRACE_SCOPE;
  
	// Open the file.
	GDALDataset *data ( static_cast<GDALDataset*> ( ::GDALOpen ( filename.c_str(), GA_ReadOnly ) ) );

	// Return if no data.
  if ( 0x0 == data )
    return 0x0;
  
	// Make sure data set is closed.
  Usul::Scope::Caller::RefPtr closeDataSet ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( data, GDALClose ) ) );
  
  return ImagePtr ( Minerva::convert ( data ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a filename and return an image (IReadImageFile).
//
///////////////////////////////////////////////////////////////////////////////

void GDALReadImageComponent::writeImageFile ( const std::string& file, osg::Image & ) const
{
  USUL_TRACE_SCOPE;
  throw std::runtime_error ( "Error 4025700686: Imagw writing not implemented yet" );
}

