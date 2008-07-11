
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback to display the meta data for item.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoRSS/GeoRSSCallback.h"

#include "OsgTools/Legend/Image.h"
#include "OsgTools/Legend/Legend.h"
#include "OsgTools/Legend/LegendObject.h"
#include "OsgTools/Legend/Text.h"

using namespace Minerva::Layers::GeoRSS;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSCallback::GeoRSSCallback() : BaseClass(),
  _imageFilename(),
  _imageSize()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSCallback::~GeoRSSCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Object has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSCallback::Item* GeoRSSCallback::operator() ( const DataObject& object, Usul::Interfaces::IUnknown* caller ) const
{
  Size size ( this->imageSize() );
  
  OsgTools::Legend::Legend::RefPtr legend ( new OsgTools::Legend::Legend );
  legend->maximiumSize ( size[0], 100 + size[1] );
  legend->heightPerItem ( 256 );
  legend->position ( 20, 20 );
  legend->growDirection ( OsgTools::Legend::Legend::UP );
  
  OsgTools::Legend::LegendObject::RefPtr row0 ( new OsgTools::Legend::LegendObject );
  
  // Make some text.
  OsgTools::Legend::Text::RefPtr text0 ( new OsgTools::Legend::Text );
  text0->text ( object.name() );
  text0->wrapLine ( false );
  text0->autoSize ( false );
  text0->alignmentVertical ( OsgTools::Legend::Text::TOP );
  text0->fontSize ( 15 );
  
  // Add the items.
  row0->addItem ( text0.get() );
  
  // Set the percentage of the row.
  row0->percentage ( 0 ) = 1.00;
  
  const std::string description ( object.description() );
  if ( false == description.empty() )
  {
    OsgTools::Legend::LegendObject::RefPtr row1 ( new OsgTools::Legend::LegendObject );
    
    // Make some text.
    OsgTools::Legend::Text::RefPtr text ( new OsgTools::Legend::Text );
    text->text ( description );
    text->wrapLine ( true );
    text->alignmentVertical ( OsgTools::Legend::Text::TOP );
    text->fontSize ( 15 );
    
    // Add the items.
    row1->addItem ( text.get() );
    
    // Set the percentage of the row.
    row1->percentage ( 0 ) = 1.00;
    
    legend->addRow ( row1.get() );
  }
  
  // Add the image.
  if ( false == _imageFilename.empty() )
  {
    OsgTools::Legend::LegendObject::RefPtr row ( new OsgTools::Legend::LegendObject );
    
    // Make the image.
    OsgTools::Legend::Image::RefPtr image ( new OsgTools::Legend::Image ( _imageFilename ) );
    image->size ( this->imageSize() );
    
    row->addItem ( image.get() );
    
    // Set the percentage of the row.
    row->percentage ( 0 ) = 1.00;
    
    legend->addRow ( row.get() );
  }
  
  legend->addRow ( row0.get() );
  
  return legend.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image filename.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSCallback::imageFilename ( const std::string& filename )
{
  _imageFilename = filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image filename.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& GeoRSSCallback::imageFilename() const
{
  return _imageFilename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image size.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSCallback::imageSize ( unsigned int width, unsigned int height )
{
  _imageSize.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image size.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSCallback::Size GeoRSSCallback::imageSize() const
{
  return _imageSize;
}
