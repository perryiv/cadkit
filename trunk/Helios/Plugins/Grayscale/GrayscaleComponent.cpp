
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "GrayscaleComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "osg/Vec4"
#include "osg/ref_ptr"
#include "osg/Image"

#include "OsgTools/Images/Grayscale.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GrayscaleComponent, GrayscaleComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GrayscaleComponent::GrayscaleComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GrayscaleComponent::~GrayscaleComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *GrayscaleComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast < Usul::Interfaces::IMenuEntry* >  (this );
  case Usul::Interfaces::ICommand::IID:
    return static_cast < Usul::Interfaces::ICommand* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string  GrayscaleComponent::menuText() const
{
  return "RGB to Grayscale";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string GrayscaleComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string GrayscaleComponent::statusBarText() const
{
  return "";
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string GrayscaleComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert all images to grayscale
//
///////////////////////////////////////////////////////////////////////////////

void GrayscaleComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );
  Usul::Interfaces::IImageList::QueryPtr imageList ( activeDocument.valid () ? activeDocument->getActiveDocument() : 0x0 );
  Usul::Interfaces::IImage::QueryPtr     image     ( activeDocument.valid () ? activeDocument->getActiveDocument() : 0x0 );

  if ( imageList.valid() )
  {
    typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
    typedef Usul::Interfaces::IImageList::ImageList ImageList;
    
    ImageList &images ( imageList->getImageList() );
    

    for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
    {
      *i = OsgTools::Images::rgbToGrayscale( i->get() );
    }

    Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
    sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
    sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
  }
  else if ( image.valid() )
  {
    osg::ref_ptr< osg::Image > i ( image->getImage() );
    image->setImage ( OsgTools::Images::rgbToGrayscale( i.get() ) );

    Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
    sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
    sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  }
}

