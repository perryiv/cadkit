#ifndef _menukit_example_create_menu_create_scene_h_
#define _menukit_example_create_menu_create_scene_h_

struct create_menu
{
  MenuKit::Menu* operator ()();
};


/** defines the structure of the menu */
MenuKit::Menu* create_menu::operator()()
{
  MenuKit::Button::Ptr spacer = new MenuKit::Button("Spacer","");
  spacer->separator( true );

  MenuKit::Button::Ptr newfile = new MenuKit::Button("New","");
  MenuKit::Button::Ptr open = new MenuKit::Button("Open","");
  MenuKit::Button::Ptr close = new MenuKit::Button("Close","");
  MenuKit::Button::Ptr exit = new MenuKit::Button("Exit","");

  MenuKit::Button::Ptr undo = new MenuKit::Button("Undo","");
  MenuKit::Button::Ptr redo = new MenuKit::Button("Redo","");
  redo->enabled( false );
  MenuKit::Button::Ptr copy = new MenuKit::Button("Copy","");
  MenuKit::Button::Ptr paste = new MenuKit::Button("Paste","");

  MenuKit::Button::Ptr solutionbrowser = new MenuKit::Button("Solution Browser","");
  solutionbrowser->toggle( true );
  MenuKit::Button::Ptr classbrowser = new MenuKit::Button("Class Browser","");
  classbrowser->toggle( true );

  MenuKit::Menu::Ptr toolbars = new MenuKit::Menu("Toolbars","",MenuKit::Menu::VERTICAL);
  toolbars->append( solutionbrowser.get() );
  toolbars->append( classbrowser.get() );

  MenuKit::Menu::Ptr file = new MenuKit::Menu("File","",MenuKit::Menu::VERTICAL);
  file->append( newfile.get() );
  file->append( open.get() );
  file->append( close.get() );
  file->append( spacer.get() );
  file->append( exit.get() );

  MenuKit::Menu::Ptr edit = new MenuKit::Menu("Edit","",MenuKit::Menu::VERTICAL);
  edit->append( undo.get() );
  edit->append( redo.get() );
  edit->append( copy.get() );
  edit->append( paste.get() );

  MenuKit::Menu::Ptr view = new MenuKit::Menu("View","",MenuKit::Menu::VERTICAL);
  view->append( toolbars.get() );

  MenuKit::Menu* top = new MenuKit::Menu("","",MenuKit::Menu::HORIZONTAL);
  top->append( file.get() );
  top->append( edit.get() );
  top->append( view.get() );
  return top;
}

#endif
