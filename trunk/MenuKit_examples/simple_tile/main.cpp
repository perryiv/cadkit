
#include "MenuKit/Menu.h"
#include "MenuKit/StreamVisitor.h"

#include <iostream>

MenuKit::Menu* create_menu();

int main(int argc, char* argv[])
{
MenuKit::Menu::Ptr menu = create_menu();

MenuKit::StreamVisitor::Ptr console = new MenuKit::StreamVisitor(std::cout);
menu->accept( *console );

return 1;
}

MenuKit::Menu* create_menu()
{
MenuKit::Menu::Ptr top = new MenuKit::Menu();
top->text("");
top->layout(MenuKit::Menu::HORIZONTAL);
top->expanded(true);

MenuKit::Menu::Ptr file = new MenuKit::Menu();
file->text("File");
file->layout(MenuKit::Menu::VERTICAL);
file->expanded(false);
top->append( file.get() );

MenuKit::Menu::Ptr edit = new MenuKit::Menu();
edit->text("Edit");
edit->layout(MenuKit::Menu::VERTICAL);
file->expanded(false);
top->append( edit.get() );

return top.get();
}

