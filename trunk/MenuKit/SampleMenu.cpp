//
// Author: John K. Grant
// All rights reserved.
//

#include "MenuKit/SampleMenu.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

using namespace MenuKit;

Menu* SampleMenu::operator ()()
{
  MenuKit::Button::Ptr newproject = new MenuKit::Button("Project","");
  MenuKit::Button::Ptr newfile = new MenuKit::Button("File","");
  MenuKit::Menu::Ptr newstuff = new MenuKit::Menu("New","",MenuKit::Menu::VERTICAL);
  newstuff->append( newproject.get() );
  newstuff->append( newfile.get() );

  MenuKit::Button::Ptr openproject = new MenuKit::Button("Project","");
  MenuKit::Button::Ptr openfile = new MenuKit::Button("File","");
  MenuKit::Menu::Ptr open = new MenuKit::Menu("Open","",MenuKit::Menu::VERTICAL);
  open->append( openproject.get() );
  open->append( openfile.get() );

  MenuKit::Button::Ptr close = new MenuKit::Button("Close","");
  MenuKit::Button::Ptr exit = new MenuKit::Button("Exit","");
  MenuKit::Button::Ptr filespacer = new MenuKit::Button("FileSpacer","");
  filespacer->separator( true );

  MenuKit::Button::Ptr undo = new MenuKit::Button("Undo","");
  MenuKit::Button::Ptr redo = new MenuKit::Button("Redo","");
  redo->enabled( false );

  MenuKit::Button::Ptr copy = new MenuKit::Button("Copy","");
  MenuKit::Button::Ptr paste = new MenuKit::Button("Paste","");

  MenuKit::Button::Ptr solutionbrowser = new MenuKit::Button("Solution Browser","");
  solutionbrowser->toggle( true );
  MenuKit::Button::Ptr classbrowser = new MenuKit::Button("Class Browser","");
  classbrowser->toggle( true );

  MenuKit::Button::Ptr standardtoolbar = new MenuKit::Button("Standard","");
  standardtoolbar->toggle( true );
  MenuKit::Button::Ptr texteditortoolbar = new MenuKit::Button("Text Editor","");
  texteditortoolbar->toggle( true );
  MenuKit::Button::Ptr formattoolbar = new MenuKit::Button("Format","");
  formattoolbar->toggle( true );
  MenuKit::Menu::Ptr toolbars = new MenuKit::Menu("Toolbars","",MenuKit::Menu::VERTICAL);
  toolbars->append( formattoolbar.get() );
  toolbars->append( standardtoolbar.get() );
  toolbars->append( texteditortoolbar.get() );

  MenuKit::Menu::Ptr file = new MenuKit::Menu("File","",MenuKit::Menu::VERTICAL);
  file->append( newstuff.get() );
  file->append( open.get() );
  file->append( close.get() );
  file->append( filespacer.get() );
  file->append( exit.get() );

  MenuKit::Menu::Ptr edit = new MenuKit::Menu("Edit","",MenuKit::Menu::VERTICAL);
  edit->append( undo.get() );
  edit->append( redo.get() );
  edit->append( copy.get() );
  edit->append( paste.get() );

  MenuKit::Menu::Ptr view = new MenuKit::Menu("View","",MenuKit::Menu::VERTICAL);
  view->append( toolbars.get() );
  view->append( solutionbrowser.get() );
  view->append( classbrowser.get() );

  MenuKit::Menu* top = new MenuKit::Menu("","",MenuKit::Menu::HORIZONTAL);
  top->append( file.get() );
  top->append( edit.get() );
  top->append( view.get() );
  return top;
}
