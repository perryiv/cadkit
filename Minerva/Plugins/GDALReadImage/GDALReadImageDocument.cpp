
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/GDALReadImage/GDALReadImageDocument.h"

#include "Minerva/Layers/GDAL/Convert.h"
#include "Minerva/Layers/GDAL/MakeImage.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "gdal.h"

#include <vector>
#include <fstream>
#include <iostream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GDALReadImageDocument, GDALReadImageDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::GDALReadImageDocument() : 
  BaseClass ( "GDAL Image Document" )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::~GDALReadImageDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void GDALReadImageDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *GDALReadImageDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IReadImageFile::IID:
    return static_cast < Usul::Interfaces::IReadImageFile* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GDALReadImageDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GDALReadImageDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GDALReadImageDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GDALReadImageDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( "jpg" == ext || "gif" == ext || "tif" == ext || "tiff" == ext || "png" == ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void GDALReadImageDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void GDALReadImageDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::Filters GDALReadImageDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::Filters GDALReadImageDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::Filters GDALReadImageDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::Filters GDALReadImageDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a filename and return an image (IReadImageFile).
//
///////////////////////////////////////////////////////////////////////////////

GDALReadImageDocument::ImagePtr GDALReadImageDocument::readImageFile ( const std::string& filename ) const
{
	// Open the file.
	GDALDataset *data ( static_cast<GDALDataset*> ( ::GDALOpen ( filename.c_str(), GA_ReadOnly ) ) );

	// Return if no data.
  if ( 0x0 == data )
    return 0x0;

	// Make sure data set is closed.
  Usul::Scope::Caller::RefPtr closeDataSet ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( data, GDALClose ) ) );

  // Get the number of bands.
  const int bands ( data->GetRasterCount() );

	// Get the data type.  Assume that all bands have the same type.
  GDALDataType type ( data->GetRasterBand ( 1 )->GetRasterDataType() );
	
	// Get the width and height.
	const unsigned int width ( data->GetRasterXSize() );
	const unsigned int height ( data->GetRasterYSize() );

	ImagePtr image ( Minerva::GDAL::makeImage ( width, height, bands, type ) );
  
  // Return if we couldn't create the proper image type.
  if ( false == image.valid() )
    return 0x0;
  
  switch ( type )
  {
    case GDT_Byte:
      Minerva::convert<unsigned char> ( image.get(), data, type );
      break;
    case GDT_UInt16:
      Minerva::convert<unsigned short> ( image.get(), data, type );
      break;
    case GDT_Int16:
      Minerva::convert<short> ( image.get(), data, type );
      break;
    case GDT_UInt32:
      Minerva::convert<unsigned int> ( image.get(), data, type );
      break;
    case GDT_Int32:
      Minerva::convert<int> ( image.get(), data, type );
      break;
    case GDT_Float32:
      Minerva::convert<float> ( image.get(), data, type );
      break;
    case GDT_Float64:
      Minerva::convert<double> ( image.get(), data, type );
      break;
    default:
      return 0x0; // We don't handle this data type.
  }
  
  return image;
}
