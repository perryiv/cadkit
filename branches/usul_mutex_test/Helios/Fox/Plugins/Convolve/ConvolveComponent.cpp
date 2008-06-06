
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

#include "ConvolveComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"

#include "FoxTools/Functions/MainWindow.h"

#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/Matrix.h"
#include "FoxTools/Headers/TextField.h"

#include "OsgTools/Images/Morphology.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include <iostream>
#include <sstream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ConvolveComponent, ConvolveComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ConvolveComponent::ConvolveComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ConvolveComponent::~ConvolveComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ConvolveComponent::queryInterface ( unsigned long iid )
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

std::string  ConvolveComponent::menuText() const
{
  return "Convolve";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string ConvolveComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string ConvolveComponent::statusBarText() const
{
  return "";
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string ConvolveComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convole the image
//
///////////////////////////////////////////////////////////////////////////////

void ConvolveComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );

  if ( activeDocument.valid () )
  {
    Usul::Interfaces::IImageList::QueryPtr imageList ( activeDocument->getActiveDocument() );
    Usul::Interfaces::IImage::QueryPtr image ( activeDocument->getActiveDocument() );

    FX::FXDialogBox dialog ( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Create the convolution matrix" );

    FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( &dialog, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
    
    unsigned int numRows ( 3 );
    unsigned int numCols ( 3 );

    FX::FXMatrix *matrix ( new FXMatrix( theFrame, numCols, LAYOUT_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|MATRIX_BY_COLUMNS ) );

    OsgTools::Images::Matrix< FX::FXTextField* > values ( numRows, numCols );

    for( unsigned int i = 0; i < numRows; ++i )
    {
      for( unsigned int j = 0; j < numCols; ++j )
      {
        values.at( i, j ) = new FX::FXTextField ( matrix, 3 );
        values.at( i, j )->setText("0");
      }
    }

    FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
    new FX::FXButton ( buttonFrame, "Accept",NULL, &dialog, FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
    new FX::FXButton ( buttonFrame, "Cancel",NULL, &dialog, FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);

    dialog.create();

    if ( dialog.execute( FX::PLACEMENT_OWNER ) )
    {
      std::cout << "Convolving with kernel: " << std::endl;

      OsgTools::Images::Matrix<int> mask ( numRows, numCols );
      
      for( unsigned int i = 0; i < numRows; ++i )
      {
        for( unsigned int j = 0; j < numCols; ++j )
        {
          std::istringstream in ( (char*) values.at( i, j )->getText().text() );
          int v;
          in >> v;
          mask.at( i, j ) = v;

          std::cout << v;
          if( j != ( numCols - 1 ) )
            std::cout << ",";
        }
        std::cout << std::endl;
      }

      if ( imageList.valid() )
      {
        typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
        typedef Usul::Interfaces::IImageList::ImageList ImageList;
        
        ImageList &images ( imageList->getImageList() );

        for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
        {
          OsgTools::Images::convolve( *(*i), mask );
        }

        Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
      }
      else if ( image.valid() )
      {
        OsgTools::Images::convolve( *image->getImage(), mask );

        Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( activeDocument->getActiveDocument() );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
      }
    }
  }
}

