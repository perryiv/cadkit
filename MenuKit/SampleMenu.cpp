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
  MenuKit::Button::Ptr blanksolution = new MenuKit::Button("Blank Solution","");
  MenuKit::Menu::Ptr newstuff = new MenuKit::Menu("New","",MenuKit::Menu::VERTICAL);
  newstuff->append( newproject.get() );
  newstuff->append( newfile.get() );
  newstuff->append( blanksolution.get() );

  MenuKit::Button::Ptr openproject = new MenuKit::Button("Project","");
  MenuKit::Button::Ptr openfile = new MenuKit::Button("File","");
  MenuKit::Menu::Ptr open = new MenuKit::Menu("Open","",MenuKit::Menu::VERTICAL);
  open->append( openproject.get() );
  open->append( openfile.get() );

  MenuKit::Button::Ptr close = new MenuKit::Button("Close","");
  MenuKit::Button::Ptr filespacer = new MenuKit::Button("FileSpacer","");
  filespacer->separator( true );
  MenuKit::Button::Ptr exit = new MenuKit::Button("Exit","");

  MenuKit::Button::Ptr undo = new MenuKit::Button("Undo","");
  MenuKit::Button::Ptr redo = new MenuKit::Button("Redo","");
  redo->enabled( false );
  MenuKit::Button::Ptr editspacer = new MenuKit::Button("EditSpacer","");
  editspacer->separator( true );
  MenuKit::Button::Ptr cut = new MenuKit::Button("Cut","");
  MenuKit::Button::Ptr copy = new MenuKit::Button("Copy","");
  MenuKit::Button::Ptr paste = new MenuKit::Button("Paste","");

  MenuKit::Button::Ptr solutionexplorer = new MenuKit::Button("Solution Browser","");
  MenuKit::Button::Ptr classview = new MenuKit::Button("Class Browser","");
  MenuKit::Button::Ptr viewspacer = new MenuKit::Button("ViewSpacer","");
  viewspacer->separator( true );

  MenuKit::Button::Ptr standardtoolbar = new MenuKit::Button("Standard","");
  standardtoolbar->toggle( true );
  MenuKit::Button::Ptr texteditortoolbar = new MenuKit::Button("Text Editor","");
  texteditortoolbar->toggle( true );
  MenuKit::Menu::Ptr toolbars = new MenuKit::Menu("Toolbars","",MenuKit::Menu::VERTICAL);
  toolbars->append( standardtoolbar.get() );
  toolbars->append( texteditortoolbar.get() );

  Button::Ptr addnewitem = new Button("Add New Item","");
  Button::Ptr addexistingitem = new Button("Add Existing Item","");
  Button::Ptr projectspacer = new Button("ProjectSpacer","");
  projectspacer->separator( true );
  Button::Ptr buildorder = new Button("Build Order","");
  Button::Ptr builddependencies = new Button("Build Dependencies","");
  Menu::Ptr project = new Menu("Project","",Menu::VERTICAL);
  project->append( addnewitem.get() );
  project->append( addexistingitem.get() );
  project->append( projectspacer.get() );
  project->append( buildorder.get() );
  project->append( builddependencies.get() );

  Button::Ptr buildsolution = new Button("Build Solution","");
  Button::Ptr rebuildsolution = new Button("Rebuild Solution","");
  Button::Ptr cleansolution = new Button("Clean Solution","");
  Menu::Ptr build = new Menu("Build","",Menu::VERTICAL);
  build->append( buildsolution.get() );
  build->append( rebuildsolution.get() );
  build->append( cleansolution.get() );

  Button::Ptr startdebug = new Button("Start","");
  Button::Ptr stopdebug = new Button("Stop","");
  Button::Ptr debugspacer = new Button("DebugSpacer","");
  debugspacer->separator( true );
  Button::Ptr stepinto = new Button("Step Into","");
  Button::Ptr stepover = new Button("Step Over","");
  Button::Ptr debugspacer2 = new Button("DebugSpacer2","");
  debugspacer2->separator( true );
  Button::Ptr newbreakpoint = new Button("New Breakpoint...","");
  Menu::Ptr debug = new Menu("Debug","",Menu::VERTICAL);
  debug->append( startdebug.get() );
  debug->append( stopdebug.get() );
  debug->append( debugspacer.get() );
  debug->append( stepinto.get() );
  debug->append( stepover.get() );
  debug->append( debugspacer2.get() );
  debug->append( newbreakpoint.get() );

  Button::Ptr customize = new Button("Customize...","");
  Button::Ptr options = new Button("Options...","");
  Menu::Ptr tools = new Menu("Tools","",Menu::VERTICAL);
  tools->append( customize.get() );
  tools->append( options.get() );

  Button::Ptr newwindow = new Button("New Window","");
  Button::Ptr split = new Button("Split","");
  Button::Ptr windowspacer = new Button("WindowSpacer","");
  windowspacer->separator( true );
  Button::Ptr closedocuments = new Button("Close All Documents","");
  Menu::Ptr window = new Menu("Window","",Menu::VERTICAL);
  window->append( newwindow.get() );
  window->append( split.get() );
  window->append( windowspacer.get() );
  window->append( closedocuments.get() );

  MenuKit::Menu::Ptr file = new MenuKit::Menu("File","",MenuKit::Menu::VERTICAL);
  file->append( newstuff.get() );
  file->append( open.get() );
  file->append( close.get() );
  file->append( filespacer.get() );
  file->append( exit.get() );

  Menu::Ptr edit = new Menu("Edit","",MenuKit::Menu::VERTICAL);
  edit->append( undo.get() );
  edit->append( redo.get() );
  edit->append( editspacer.get() );
  edit->append( cut.get() );
  edit->append( copy.get() );
  edit->append( paste.get() );

  MenuKit::Menu::Ptr view = new MenuKit::Menu("View","",MenuKit::Menu::VERTICAL);
  view->append( solutionexplorer.get() );
  view->append( classview.get() );
  view->append( viewspacer.get() );
  view->append( toolbars.get() );

  Button::Ptr about = new Button("About","");
  Menu::Ptr help = new Menu("Help","",Menu::VERTICAL);
  help->append( about.get() );

  MenuKit::Menu* top = new MenuKit::Menu("","",MenuKit::Menu::HORIZONTAL);
  top->append( file.get() );
  top->append( edit.get() );
  top->append( view.get() );
  top->append( project.get() );
  top->append( build.get() );
  top->append( debug.get() );
  top->append( tools.get() );
  top->append( window.get() );
  top->append( help.get() );
  return top;
}
