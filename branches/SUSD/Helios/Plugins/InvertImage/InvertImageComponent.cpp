
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "InvertImageComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "osg/Vec4"
#include "osg/ref_ptr"
#include "osg/Image"

#include "OsgTools/Images/Invert.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( InvertImageComponent, InvertImageComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InvertImageComponent::InvertImageComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InvertImageComponent::~InvertImageComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *InvertImageComponent::queryInterface ( unsigned long iid )
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

std::string  InvertImageComponent::menuText() const
{
  return "Invert Image";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string InvertImageComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string InvertImageComponent::statusBarText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string InvertImageComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute
//
///////////////////////////////////////////////////////////////////////////////

void InvertImageComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );

  if ( activeDocument.valid () )
  {
    Usul::Interfaces::IImageList::QueryPtr imageList ( activeDocument->getActiveDocument() );
    Usul::Interfaces::IImage::QueryPtr image ( activeDocument->getActiveDocument() );

    if ( imageList.valid() )
    {
      typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
      typedef Usul::Interfaces::IImageList::ImageList ImageList;
      
      ImageList &images ( imageList->getImageList() );
      

      for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
      {
        if ( (*i)->getPixelFormat() == GL_LUMINANCE || (*i)->getPixelFormat() == GL_LUMINANCE_ALPHA )
          OsgTools::Images::invert( *(*i) );
        else
        {
          std::ostringstream message;
          message << "Can only apply invert to grayscale images. " << std::endl
                  << "Run the grayscale tool first.";

          throw std::runtime_error ( message.str() );
        }
      }

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
    else if ( image.valid() )
    {
      OsgTools::Images::invert( *image->getImage() );

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
  }
}

