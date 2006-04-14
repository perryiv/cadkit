
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

#include "EdgeDetectComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/TextField.h"
#include "FoxTools/Headers/ComboBox.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "OsgTools/Images/Matrix.h"
#include "OsgTools/Images/Morphology.h"
#include "OsgTools/Images/ImageOr.h"
#include "OsgTools/Images/EdgeDetect.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( EdgeDetectComponent, EdgeDetectComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EdgeDetectComponent::EdgeDetectComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EdgeDetectComponent::~EdgeDetectComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *EdgeDetectComponent::queryInterface ( unsigned long iid )
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

std::string  EdgeDetectComponent::menuText() const
{
  return "Edge Detect";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string EdgeDetectComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string EdgeDetectComponent::statusBarText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string EdgeDetectComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute
//
///////////////////////////////////////////////////////////////////////////////

void EdgeDetectComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );

  if ( activeDocument.valid () )
  {
    FX::FXDialogBox dialog ( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Select Mask" );

    FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( &dialog, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

    FX::FXComboBox* combobox ( new FXComboBox(theFrame,3,NULL,0,COMBOBOX_INSERT_LAST|FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT) );
    combobox->setNumVisible(2);
    combobox->appendItem("Sobel");
    combobox->appendItem("Prewitt");
    combobox->setText( "Sobel" );

    FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
    new FX::FXButton ( buttonFrame, "Accept",NULL, &dialog, FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
    new FX::FXButton ( buttonFrame, "Cancel",NULL, &dialog, FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);

    dialog.create();

    if ( dialog.execute( FX::PLACEMENT_OWNER ) )
    {

      std::string filter ( ( char* )  combobox->getText().text() );

      OsgTools::Images::Matrix<int> maskX(3,3);
      OsgTools::Images::Matrix<int> maskY(3,3);

      if( filter == "Prewitt" )
      {
        maskX(0,0) = maskX(1,0) = maskX(2,0) = -1;
        maskX(0,1) = maskX(1,1) = maskX(2,1) = 0;
        maskX(0,2) = maskX(1,2) = maskX(2,2) = 1;

        maskY(0,0) = maskY(0,1) = maskY(0,2) = -1;
        maskY(1,0) = maskY(1,1) = maskY(1,2) = 0;
        maskY(2,0) = maskY(2,1) = maskY(2,2) = 1;
      }
      else if ( filter == "Sobel" )
      {
        maskX(0,0) = maskX(2,0) = -1;
        maskX(1,0) = -2;
        maskX(0,1) = maskX(1,1) = maskX(2,1) = 0;
        maskX(0,2) = maskX(2,2) = 1;
        maskX(1,2) = 2;

        maskY(0,0) = maskY(0,2) = -1;
        maskY(0,1) = -2;
        maskY(1,0) = maskY(1,1) = maskY(1,2) = 0;
        maskY(2,0) = maskY(2,2) = 1;
        maskY(2,1) = 2;
      }
      else
      {
        throw std::runtime_error ( "Error: Invalid mask specified." );
      }


      Usul::Interfaces::IImageList::QueryPtr imageList ( activeDocument->getActiveDocument() );
      Usul::Interfaces::IImage::QueryPtr     image     ( activeDocument->getActiveDocument() );

      if ( imageList.valid() )
      {
        typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
        typedef Usul::Interfaces::IImageList::ImageList ImageList;
        
        ImageList &images ( imageList->getImageList() );

        for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
        {
          *i = OsgTools::Images::edgeDetect( *(*i), maskX, maskY );
        }

        Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
      }
      else if ( image.valid() )
      {
        osg::ref_ptr< osg::Image > i ( image->getImage() );
        image->setImage ( OsgTools::Images::edgeDetect( *i, maskX, maskY ) );

        Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
      }
    }
  }
}

