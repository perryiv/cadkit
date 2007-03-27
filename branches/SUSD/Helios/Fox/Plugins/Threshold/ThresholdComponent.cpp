
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

#include "ThresholdComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"

#include "FoxTools/Widgets/SliderTextField.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "OsgTools/Images/Threshold.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ThresholdComponent, ThresholdComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ThresholdComponent::ThresholdComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ThresholdComponent::~ThresholdComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ThresholdComponent::queryInterface ( unsigned long iid )
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

std::string  ThresholdComponent::menuText() const
{
  return "Threshold";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string ThresholdComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string ThresholdComponent::statusBarText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string ThresholdComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute
//
///////////////////////////////////////////////////////////////////////////////

void ThresholdComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );

  if ( activeDocument.valid () )
  {
    Usul::Interfaces::IImageList::QueryPtr imageList ( activeDocument->getActiveDocument() );
    Usul::Interfaces::IImage::QueryPtr image ( activeDocument->getActiveDocument() );

  
    typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
    typedef Usul::Interfaces::IImageList::ImageList ImageList;
#if 1
    FX::FXDialogBox dialog ( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Threshold", FX::DECOR_TITLE | FX::DECOR_BORDER | FX::DECOR_RESIZE | FX::DECOR_CLOSE, 0, 0, 300 );

    FX::FXVerticalFrame *contents ( new FX::FXVerticalFrame( &dialog , LAYOUT_FILL_X|LAYOUT_FILL_Y) );

    FoxTools::Widgets::SliderTextField *slider = new FoxTools::Widgets::SliderTextField ( contents, "Threshold: " );

    FX::FXHorizontalFrame *hframe ( new FX::FXHorizontalFrame ( contents, FX::LAYOUT_FILL_Y| FX::LAYOUT_FILL_X ) );
    new FX::FXButton ( hframe, "Accept", 0x0, &dialog, FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK );
    new FX::FXButton ( hframe, "Cancel", 0x0, &dialog, FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK );

    if( dialog.execute ( FX::PLACEMENT_OWNER ) )
    {

      float threshold ( slider->getValue() );
#else
    if( 1 )
    {
#endif
      if ( imageList.valid() )
      {

        ImageList &images ( imageList->getImageList() );

        for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
        {
          if ( (*i)->getPixelFormat() == GL_LUMINANCE || (*i)->getPixelFormat() == GL_LUMINANCE_ALPHA )
          {
#if 1
            OsgTools::Images::threshold( *(*i), threshold );
#else
            OsgTools::Images::threshold( *(*i), 100, 175 );
#endif
          }
          else
          {
            std::ostringstream message;
            message << "Can only apply thresholding to grayscale images. " << std::endl
                    << "Run the grayscale tool first.";

            throw std::runtime_error ( message.str() );
          }
        }
      }
      else if ( image.valid() )
      {
#if 1
        OsgTools::Images::threshold( *image->getImage(), threshold );
#endif
      }

      // Rebuild
      Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
    
  }
}

