
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoRSS/Item.h"

#include "OsgTools/Widgets/Image.h"
#include "OsgTools/Widgets/Legend.h"
#include "OsgTools/Widgets/LegendObject.h"
#include "OsgTools/Widgets/Text.h"

using namespace Minerva::Layers::GeoRSS;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item() : BaseClass(),
  _date(),
  _imageFilename(),
  _imageSize(),
  _categories()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::~Item()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image filename.
//
///////////////////////////////////////////////////////////////////////////////

void Item::imageFilename ( const std::string& filename )
{
  _imageFilename = filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image filename.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Item::imageFilename() const
{
  return _imageFilename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image size.
//
///////////////////////////////////////////////////////////////////////////////

void Item::imageSize ( unsigned int width, unsigned int height )
{
  _imageSize.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image size.
//
///////////////////////////////////////////////////////////////////////////////

Item::Size Item::imageSize() const
{
  return _imageSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date.
//
///////////////////////////////////////////////////////////////////////////////

void Item::date ( const std::string& s )
{
  _date = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Item::date() const
{
  return _date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make text.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline OsgTools::Widgets::Text* makeText ( const std::string& s, bool wrap, bool autoSize )
  {
    OsgTools::Widgets::Text::RefPtr text ( new OsgTools::Widgets::Text );
    text->text ( s );
    text->wrapLine ( wrap );
    text->autoSize ( autoSize );
    text->alignmentVertical ( OsgTools::Widgets::Text::TOP );
    text->fontSize ( 15 );
    text->textColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
    
    return text.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  DataObject has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Widgets::Item* Item::clicked ( Usul::Interfaces::IUnknown* caller ) const
{
  Size size ( this->imageSize() );
  
  OsgTools::Widgets::Legend::RefPtr legend ( new OsgTools::Widgets::Legend );
  legend->maximiumSize ( 350, 750 );
  legend->position ( 20, 40 );
  legend->growDirection ( OsgTools::Widgets::Legend::GROW_DIRECTION_UP );
  legend->backgroundColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  legend->borderColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  
  OsgTools::Widgets::LegendObject::RefPtr row0 ( new OsgTools::Widgets::LegendObject );
  
  // Make some text.
  OsgTools::Widgets::Text::RefPtr text0 ( Helper::makeText ( this->name(), false, false ) );
  
  // Add the items.
  row0->addItem ( text0.get() );
  
  // Set the percentage of the row.
  row0->percentage ( 0 ) = 1.00;
  
  std::string description ( this->description() );
  if ( false == description.empty() )
  {
    OsgTools::Widgets::LegendObject::RefPtr row1 ( new OsgTools::Widgets::LegendObject );
    
    // Make some text.
    OsgTools::Widgets::Text::RefPtr text ( Helper::makeText ( description, true, true ) );
    
    // Add the items.
    row1->addItem ( text.get() );
    
    // Set the percentage of the row.
    row1->percentage ( 0 ) = 1.00;
    
    legend->addRow ( row1.get() );
  }
  
  // Add the image.
  if ( false == _imageFilename.empty() )
  {
    OsgTools::Widgets::LegendObject::RefPtr row ( new OsgTools::Widgets::LegendObject );
    
    // Make the image.
    OsgTools::Widgets::Image::RefPtr image ( new OsgTools::Widgets::Image ( _imageFilename ) );
    image->size ( this->imageSize() );
    
    row->addItem ( image.get() );
    
    // Set the percentage of the row.
    row->percentage ( 0 ) = 1.00;
    
    legend->addRow ( row.get() );
  }
  
  const std::string date ( this->date() );
  if ( false == date.empty() )
  {
    OsgTools::Widgets::LegendObject::RefPtr row1 ( new OsgTools::Widgets::LegendObject );
    
    // Add the items.
    row1->addItem ( Helper::makeText ( date, true, false ) );
    
    // Set the percentage of the row.
    row1->percentage ( 0 ) = 1.00;
    
    legend->addRow ( row1.get() );
  }
  
  legend->addRow ( row0.get() );
  
  return legend.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the categories.
//
///////////////////////////////////////////////////////////////////////////////

void Item::categories ( const Categories& c )
{
  _categories = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the categories.
//
///////////////////////////////////////////////////////////////////////////////

const Item::Categories& Item::categories() const
{
  return _categories;
}
