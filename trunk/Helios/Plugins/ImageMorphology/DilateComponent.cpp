
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

#include "DilateComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "OsgTools/Images/Morphology.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DilateComponent, DilateComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DilateComponent::DilateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DilateComponent::~DilateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DilateComponent::queryInterface ( unsigned long iid )
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

std::string  DilateComponent::menuText() const
{
  return "Dilate Image";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string DilateComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string DilateComponent::statusBarText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string DilateComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute
//
///////////////////////////////////////////////////////////////////////////////

void DilateComponent::execute ( Usul::Interfaces::IUnknown *caller )
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
      
      // Dilate each image
      for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
      {
        OsgTools::Images::dilate( *(*i), 3, 3 );
      }

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
    else if ( image.valid() )
    {
      OsgTools::Images::dilate( *image->getImage(), 3, 3 );

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }

  }
}

