
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

#include "TransparencyComponent.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Headers/ColorDialog.h"
#include "FoxTools/Headers/Vec4f.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/Slider.h"
#include "FoxTools/Headers/Label.h"

#include "FoxTools/Widgets/SliderTextField.h"

#include "osg/Vec4"
#include "osg/ref_ptr"
#include "osg/Image"



USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TransparencyComponent, TransparencyComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TransparencyComponent::TransparencyComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TransparencyComponent::~TransparencyComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TransparencyComponent::queryInterface ( unsigned long iid )
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
  case Usul::Interfaces::IHandle::IID:
    return static_cast < Usul::Interfaces::IHandle* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string  TransparencyComponent::menuText() const
{
  return "Transparency Control";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string TransparencyComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string TransparencyComponent::statusBarText() const
{
  return "";
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string TransparencyComponent::menuGroup() const
{
  return "Volumes";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void TransparencyComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IDocument::QueryPtr  document  ( Usul::Documents::Manager::instance().active() );
  Usul::Interfaces::IImageList::QueryPtr imageList ( document );

  if ( imageList.valid() )
  {
    FX::FXColorDialog dialog ( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Choose Color" );
    dialog.setOpaqueOnly ( true );

    if ( dialog.execute ( FX::PLACEMENT_OWNER ) )
    {
      FX::FXVec4f c ( dialog.getRGBA() );
    
      osg::Vec4 c1 ( c[0] * 255, c[1] * 255, c[2] * 255, 1.0 );

      FX::FXDialogBox trans ( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Set Transparency", FX::DECOR_TITLE | FX::DECOR_BORDER | FX::DECOR_RESIZE | FX::DECOR_CLOSE, 0, 0, 300 );

      FX::FXVerticalFrame *contents ( new FX::FXVerticalFrame( &trans , LAYOUT_FILL_X|LAYOUT_FILL_Y) );

      FoxTools::Widgets::SliderTextField *slider = new FoxTools::Widgets::SliderTextField ( contents, "Transparency: " );

      FX::FXHorizontalFrame *hframe ( new FX::FXHorizontalFrame ( contents, FX::LAYOUT_FILL_Y| FX::LAYOUT_FILL_X ) );
      new FX::FXButton ( hframe, "Accept", 0x0, &trans, FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK );
      new FX::FXButton ( hframe, "Cancel", 0x0, &trans, FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK );

      if( trans.execute ( FX::PLACEMENT_OWNER ) )
      {

        float transparency ( slider->getValue() );

        typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
        typedef Usul::Interfaces::IImageList::ImageList ImageList;
        
        ImageList &images ( imageList->getImageList() );

        for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
        {
          ImagePtr image ( *i );

          const unsigned int numBits ( image->getPixelSizeInBits() );
          const int width ( image->s() );
          const int height ( image->t() );
          const unsigned int size ( image->getImageSizeInBytes() );

          switch ( image->getPixelFormat() )
          {
          case(GL_ALPHA):
            break;
          case(GL_LUMINANCE_ALPHA):
            {
              for(int t=0;t<height;++t)
              {
                unsigned char* source = image->data(0,t,0);

                for(int i=0;i<width*2;++i)
                {
                  unsigned char* value ( source++ );
                  unsigned char* alpha ( source++ );
                  
                  if ( *value == c1[0] )
                  {
                    *alpha = transparency; 
                  }
                }

              }
            }
            break;
          case(GL_RGBA):
            // TODO
            break;
          default:
            throw std::runtime_error ( "Image format not supported." );
          };

          
        }

        // Rebuild the scene
        Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the IUnknown have the proper interfaces?
//
///////////////////////////////////////////////////////////////////////////////

bool TransparencyComponent::doesHandle( Usul::Interfaces::IUnknown *unknown )
{
  Usul::Interfaces::IImageList::QueryPtr imageList( unknown );

  return imageList.valid();
}

