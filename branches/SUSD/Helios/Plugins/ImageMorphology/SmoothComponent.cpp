
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

#include "SmoothComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "OsgTools/Images/Morphology.h"
#include "OsgTools/Images/Matrix.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SmoothComponent, SmoothComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SmoothComponent::SmoothComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SmoothComponent::~SmoothComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SmoothComponent::queryInterface ( unsigned long iid )
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

std::string  SmoothComponent::menuText() const
{
  return "Smooth Image";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothComponent::statusBarText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute
//
///////////////////////////////////////////////////////////////////////////////

void SmoothComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );

  if ( activeDocument.valid () )
  {
    Usul::Interfaces::IImageList::QueryPtr imageList ( activeDocument->getActiveDocument() );
    Usul::Interfaces::IImage::QueryPtr image ( activeDocument->getActiveDocument() );

    OsgTools::Images::Matrix< int > mask(3,3);

    for( unsigned int i = 0; i < mask.rows(); ++i )
      for( unsigned int j = 0; j < mask.columns(); ++ j )
        mask(i,j) = 1;

    if ( imageList.valid() )
    {
      typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
      typedef Usul::Interfaces::IImageList::ImageList ImageList;
      
      ImageList &images ( imageList->getImageList() );

      for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
      {
        OsgTools::Images::smooth( *(*i), mask );
      }

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
    else if ( image.valid() )
    {
      OsgTools::Images::smooth( *image->getImage(), mask );

      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }

  }
}

