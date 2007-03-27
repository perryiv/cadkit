
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

#include "HistogramComponent.h"

#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IImage.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/Fox/IFoxClientArea.h"
#include "Usul/Interfaces/Fox/IFoxMDIMenu.h"
#include "Usul/Documents/Document.h"

#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/GLVisual.h"

#include "OsgTools/Images/Histogram.h"
#include "OsgTools/Images/Image3d.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "Helios/Fox/Views/ChartView.h"


#include <iostream>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( HistorgramComponent, HistorgramComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

HistorgramComponent::HistorgramComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

HistorgramComponent::~HistorgramComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *HistorgramComponent::queryInterface ( unsigned long iid )
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

std::string  HistorgramComponent::menuText() const
{
  return "Histogram";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string HistorgramComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string HistorgramComponent::statusBarText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string HistorgramComponent::menuGroup() const
{
  return "Image Processing";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find histogram
//
///////////////////////////////////////////////////////////////////////////////

void HistorgramComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IActiveDocument::QueryPtr activeDocument ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr  doc    ( activeDocument->getActiveDocument() );

  if ( activeDocument.valid () )
  {
    Usul::Interfaces::IImageList::QueryPtr  imageList ( doc );
    Usul::Interfaces::IImage::QueryPtr      image     ( doc );

    typedef OsgTools::Images::Histogram Histogram;
    OsgTools::Images::Histogram red;
    OsgTools::Images::Histogram green;
    OsgTools::Images::Histogram blue;
    OsgTools::Images::Histogram gray;

    if ( imageList.valid() )
    {
      typedef Usul::Interfaces::IImageList::ImagePtr ImagePtr;
      typedef Usul::Interfaces::IImageList::ImageList ImageList;
      
      ImageList &images ( imageList->getImageList() );

      // Make a 3d image from the image list
      osg::ref_ptr< osg::Image > image3d ( OsgTools::Images::image3d ( images ) );

      // Calculate the histogram
      OsgTools::Images::calculateHistogram( red, green, blue, gray, *image3d );
    }
    else if ( image.valid() )
    {
      // Calculate the histogram
      OsgTools::Images::calculateHistogram( red, green, blue, gray, *image->getImage() );
    }

    unsigned int width ( 500 );
    unsigned int height ( 500 );

    // Typedef
    typedef FoxTools::Windows::MdiChildWindow ChildWindow;

    //Get needed interfaces
    Usul::Interfaces::IFoxClientArea::ValidQueryPtr client ( caller );
    Usul::Interfaces::IFoxMDIMenu::ValidQueryPtr    menu   ( caller );
    

    FX::FXMDIClient *clientArea ( client->clientArea() );
    FX::FXMDIMenu   *mdiMenu    ( menu->mdiMenu() );

    // Non-ref'ing smart-pointers that throw if given null.
    typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
    typedef Usul::Pointers::SmartPointer < ChildWindow, Policy > LocalChildPtr;

    // Hack... TODO fix
    if( Usul::Documents::Document *document = dynamic_cast < Usul::Documents::Document* > ( doc.get() ) )
    {
      // Make the child
      LocalChildPtr child ( new ChildWindow ( document, clientArea, "Untitled", 0x0, (FX::FXMenuPane * ) mdiMenu, 0 ) ); 

      // Make the view
      Helios::Views::ChartView *view ( new Helios::Views::ChartView ( doc, child.get(), FoxTools::Functions::visual() ) );

      typedef Helios::Views::ChartView::Point Point;
      typedef Helios::Views::ChartView::Plot Plot;
      if( false == red.empty( ) )
      {
        Plot plot;
        for ( Histogram::const_iterator iter = red.begin(); iter != red.end(); ++iter )
        {
          plot.push_back ( Point ( iter - red.begin(), *iter ) );
        }

        view->addPlot ( plot, osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
      }

      if( false == green.empty( ) )
      {
        Plot plot;
        for ( Histogram::const_iterator iter = green.begin(); iter != green.end(); ++iter )
        {
          plot.push_back ( Point ( iter - green.begin(), *iter ) );
        }

        view->addPlot ( plot, osg::Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );
      }

      if( false == blue.empty( ) )
      {
        Plot plot;
        for ( Histogram::const_iterator iter = blue.begin(); iter != blue.end(); ++iter )
        {
          plot.push_back ( Point ( iter - blue.begin(), *iter ) );
        }

        view->addPlot ( plot, osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
      }

      if( false == gray.empty( ) )
      {
        Plot plot;
        for ( Histogram::const_iterator iter = gray.begin(); iter != gray.end(); ++iter )
        {
          plot.push_back ( Point ( iter - gray.begin(), *iter ) );
        }

        view->addPlot ( plot, osg::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) );
      }

      // Get pointer to viewer
      Usul::Interfaces::IViewer::ValidQueryPtr viewer ( view );

      // Set the view
      child->view ( viewer );

      // Add the view
      document->addView ( viewer );

      // Create the child window.
      // Call create after the view has been set in the child window and the document
      child->create();

      child->setWidth( width );
      child->setHeight ( height );

      // Build scene.
      child->onBuildScene ( 0x0, 0, 0x0 );
    }
  }
}

