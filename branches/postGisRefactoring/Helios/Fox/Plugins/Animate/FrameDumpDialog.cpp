
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
#define _CPP_CMATH 1
#endif

// Disable deprecated warning in Visual Studio 8 for sprintf
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "Animate/FrameDumpDialog.h"

#include "FoxTools/Functions/App.h"
#include "FoxTools/Functions/MainWindow.h"

#include "FoxTools/Headers/Registry.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/Matrix.h"
#include "FoxTools/Headers/Label.h"
#include "FoxTools/Headers/TextField.h"
#include "FoxTools/Headers/ComboBox.h"
#include "FoxTools/Headers/Spinner.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/DirDialog.h"
#include "FoxTools/Headers/MainWindow.h"

#include "Usul/File/Path.h"
#include "Usul/Interfaces/IActiveView.h"

#include <string>
#include <memory>
#include <sstream>

using namespace Animate;



///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FrameDumpDialog ) MessageMap[] = 
{
  //          Message Type     ID                                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND,  FrameDumpDialog::ID_GET_DIRECTORY,      FrameDumpDialog::onCommandGetDirectory   ),
};

FOX_TOOLS_IMPLEMENT ( FrameDumpDialog, FrameDumpDialog::BaseClass, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

FrameDumpDialog::FrameDumpDialog ( ) :
BaseClass(),
_directory ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Display dialog and set frame dump properties
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IFrameDump* FrameDumpDialog::getFrameDumpProperties( Usul::Interfaces::IUnknown *caller )
{
  //Useful typedefs
  typedef Usul::Interfaces::IFrameDump IFrameDump;
  typedef Usul::Interfaces::IActiveView IActiveView;

  //Get the registry
  FX::FXRegistry &reg = FoxTools::Functions::application()->reg();

  //Get previous values from registry
  const FX::FXchar *directoryPath = reg.readStringEntry ( "FrameDump", "Directory", "" );
  const FX::FXchar *filename = reg.readStringEntry ( "FrameDump", "Filename", "filename" );
  const FX::FXchar *extension = reg.readStringEntry ( "FrameDump", "Extension", ".jpg" );
  int startNumber = reg.readIntEntry ( "FrameDump", "StartNumber", 0 );
  int numberDigits = reg.readIntEntry ( "FrameDump", "NumberDigits", 10 );

  //The dialog box
  std::auto_ptr< FX::FXDialogBox >  dialog ( new FX::FXDialogBox ( FoxTools::Functions::mainWindow(), "Frame Dump", FX::DECOR_CLOSE|DECOR_TITLE|DECOR_BORDER,0,0,0,0, 10,10,10,10, 4,4 ) );

  FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( dialog.get(), LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

  FX::FXMatrix *matrix ( new FXMatrix( theFrame ,5,LAYOUT_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|MATRIX_BY_ROWS) );

  //Labels
  new FX::FXLabel  ( matrix, "Directory:",            0x0, FX::LAYOUT_RIGHT );
  new FX::FXLabel  ( matrix, "Base Filename:",        0x0, FX::LAYOUT_RIGHT );
  new FX::FXLabel  ( matrix, "File Type:",            0x0, FX::LAYOUT_RIGHT );
  new FX::FXLabel  ( matrix, "Starting file number:", 0x0, FX::LAYOUT_RIGHT );
  new FX::FXLabel  ( matrix, "Number of digits:",     0x0, FX::LAYOUT_RIGHT );

  //The directory
  _directory = new FX::FXTextField ( matrix, 75, 0x0, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT,0,0,0,0 );
  _directory->setText( directoryPath );

  //Base file name
  FX::FXTextField* fileText ( new FX::FXTextField ( matrix, 25, 0x0, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT,0,0,0,0 ) );
  fileText->setText( filename );

  //possible extensions
  FX::FXComboBox* combobox ( new FXComboBox(matrix,3,NULL,0,COMBOBOX_INSERT_LAST|FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT) );
  combobox->setNumVisible(3);
  combobox->appendItem(".jpg");
  combobox->appendItem(".png");
  combobox->appendItem(".bmp");
  combobox->setText( extension );

  //what number to start at
  FX::FXTextField* numText ( new FX::FXTextField ( matrix, 3, 0x0, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT,0,0,0,0 ) );
  char buf[128];
  ::sprintf( buf, "%d", startNumber ); //itoa( startNumber, buf, 10 );
  numText->setText( buf );

  //number of digits in the filename
  FX::FXSpinner* digitsSpinner ( new FX::FXSpinner(matrix,2, this , 0x0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_LEFT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW) );
  digitsSpinner->setValue( numberDigits );
  digitsSpinner->setRange(1,15);

  //Browse Button
  FX::FXButton *dirButton ( new FX::FXButton ( matrix, "Browse", 0x0, this, FrameDumpDialog::ID_GET_DIRECTORY ) );

  //Accept and cancel buttons
  FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
  new FX::FXButton ( buttonFrame, "Accept",NULL, dialog.get(), FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
  new FX::FXButton ( buttonFrame, "Cancel",NULL, dialog.get(), FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);

  dialog->create();

  if ( dialog->execute( FX::PLACEMENT_OWNER ) )
  {
    IActiveView::ValidQueryPtr view ( caller );
    IFrameDump::ValidQueryPtr fd ( view->getActiveView() );

    std::istringstream in ( ( char * ) numText->getText().text() );

    //Get data from widgets
    std::string dir     ( ( char* )  _directory->getText().text()  );
    std::string base    ( ( char* )  fileText->getText().text() );
    std::string ext     ( ( char* )  combobox->getText().text() );
    unsigned int start;
    in >> start;
    unsigned int digits ( ( unsigned int ) digitsSpinner->getValue() );

    if( !dir.empty() && !base.empty() && !ext.empty() )
    {
      //write values to registry
      reg.writeStringEntry ( "FrameDump", "Directory", dir.c_str() );
      reg.writeStringEntry ( "FrameDump", "Filename", base.c_str() );
      reg.writeStringEntry ( "FrameDump", "Extension", ext.c_str() );
      reg.writeIntEntry    ( "FrameDump", "StartNumber", start );
      reg.writeIntEntry    ( "FrameDump", "NumberDigits", digits );

      fd->frameDumpProperties ( dir, base, ext, start, digits );
      return fd.release();
    }
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory name, set text field to answer
//
///////////////////////////////////////////////////////////////////////////////

long FrameDumpDialog::onCommandGetDirectory     ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXDirDialog open( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Select the directory");
  if( open.execute() )
  {
    FX::FXString string = open.getDirectory();
    _directory->setText( string );
  }
  return 1;
}

