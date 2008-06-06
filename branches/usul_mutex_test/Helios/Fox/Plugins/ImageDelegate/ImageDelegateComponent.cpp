
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "ImageDelegateComponent.h"

#include "Usul/Documents/Document.h"

#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/Headers/Image.h"
#include "FoxTools/Menu/Bar.h"
#include "FoxTools/Menu/Group.h"
#include "FoxTools/Menu/Button.h"

#include "Usul/Interfaces/Fox/IFoxClientArea.h"
#include "Usul/Interfaces/Fox/IFoxMDIMenu.h"
#include "Usul/Interfaces/IImageView.h"
#include "Usul/Interfaces/IImage.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/Fox/IMenuBar.h"
#include "Usul/Resources/MenuBar.h"

#include "Helios/Fox/Views/ImageView.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ImageDelegateComponent , ImageDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ImageDelegateComponent::ImageDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ImageDelegateComponent::~ImageDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ImageDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  case Usul::Interfaces::IHandleActivatingDocument::IID:
    return static_cast < Usul::Interfaces::IHandleActivatingDocument* > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token.
//
/////////////////////////////////////////////////////////////////////////////

bool ImageDelegateComponent::doesHandle( const std::string& token ) const
{
  return "Image Document" == token;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI.
//
/////////////////////////////////////////////////////////////////////////////

void ImageDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  // Typedef
  typedef FoxTools::Windows::MdiChildWindow ChildWindow;

  //Get needed interfaces
  Usul::Interfaces::IFoxClientArea::ValidQueryPtr client ( caller );
  Usul::Interfaces::IFoxMDIMenu::ValidQueryPtr    menu   ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr      doc    ( document->queryInterface ( Usul::Interfaces::IDocument::IID ) );

  FX::FXMDIClient *clientArea ( client->clientArea() );
  FX::FXMDIMenu   *mdiMenu    ( menu->mdiMenu() );

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < ChildWindow, Policy > LocalChildPtr;

  // Make the child
  LocalChildPtr child ( new ChildWindow ( document, clientArea, "Untitled", 0x0, (FX::FXMenuPane * ) mdiMenu, 0 ) ); 

  // Create the view
  Helios::Views::ImageView* view ( new Helios::Views::ImageView ( doc, child.get(), FoxTools::Functions::visual() ) );

  // Get pointer to viewer
  Usul::Interfaces::IViewer::ValidQueryPtr viewer ( view );

  // Set the view
  child->view ( viewer );

  // Add the view
  document->addView ( view );

  // Create the child window.
  // Call create after the view has been set in the child window and the document
  child->create();

  // Build the view
  document->sendMessage ( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Refresh the view.
//
/////////////////////////////////////////////////////////////////////////////

void ImageDelegateComponent::refreshView  ( Usul::Documents::Document *document, Usul::Interfaces::IViewer *viewer )
{
  Usul::Interfaces::IImageView::QueryPtr getImageView ( viewer );

  if( getImageView.valid() )
  {
    Helios::Views::ImageView *imageView ( getImageView->imageView() );
    
    Usul::Interfaces::IImage::QueryPtr getImage ( document );

    if( getImage.valid() )
    {
      osg::Image* image ( getImage->getImage() );

      imageView->setImage ( image );
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Given document is no longer active.
//
/////////////////////////////////////////////////////////////////////////////

void ImageDelegateComponent::noLongerActive ( Usul::Documents::Document* document, const std::string& activeType )
{
  Usul::Interfaces::IMenuBar::QueryPtr getMenuBar ( Usul::Resources::menuBar() );

  if( !getMenuBar.valid () )
    return;

  // Get the menu bar.
  FoxTools::Menu::Bar::ValidRefPtr menuBar ( getMenuBar->getMenuBar() );

  // Purge any menu entries this document added.
  menuBar->purge ( reinterpret_cast < unsigned int > ( document ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Given document is now active.
//
/////////////////////////////////////////////////////////////////////////////

void ImageDelegateComponent::nowActive      ( Usul::Documents::Document* document, const std::string& oldType )
{
  Usul::Interfaces::IMenuBar::QueryPtr getMenuBar ( Usul::Resources::menuBar() );

  if( !getMenuBar.valid () )
    return;

  // Get the menu bar
  FoxTools::Menu::Bar::ValidRefPtr menuBar ( getMenuBar->getMenuBar() );

  // Get the window menu
  FoxTools::Menu::Group::RefPtr windowMenu ( menuBar->group ( "Window" ) );

  if( !windowMenu.valid() )
    return;

#ifdef _DEBUG
  windowMenu->append ( new FoxTools::Menu::Button ( "Histogram", 0x0, 0, reinterpret_cast < unsigned int > ( document ) ) );

  menuBar->build();
  menuBar->create();
#endif
}


