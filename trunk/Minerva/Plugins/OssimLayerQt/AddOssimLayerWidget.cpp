
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/OssimLayerQt/CompileGuard.h"
#include "Minerva/Plugins/OssimLayerQt/AddOssimLayerWidget.h"

#include "Minerva/Core/Layers/RasterLayerOssim.h"
#include "Minerva/Core/Layers/ElevationLayerDem.h"

#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Registry/Database.h"
#include "Usul/User/Directory.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "QtTools/FileDialog.h"

#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddOssimLayerWidget::AddOssimLayerWidget( Usul::Interfaces::IUnknown* caller, QWidget *parent ) : BaseClass ( parent ),
  _caller ( caller ),
  _listView ( 0x0 )
{
  _listView = new QListWidget ( this );

  QPushButton *browse ( new QPushButton ( "Browse" ) );

  connect ( browse, SIGNAL ( clicked () ), this, SLOT ( _browseClicked () ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  topLayout->addWidget ( browse );
  topLayout->addWidget ( _listView );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddOssimLayerWidget::~AddOssimLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Browse.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::_browseClicked()
{
  // Useful typedefs.
  typedef QtTools::FileDialog              FileDialog;
  typedef FileDialog::FilesResult          FilesResult;
  typedef FileDialog::Filter               Filter;
  typedef FileDialog::Filters              Filters;
  typedef FileDialog::FileNames            FileNames;
  
  Filters filters;
  filters.push_back ( Filter ( "JPEG (*.jpg)", "*.jpg" ) );
  filters.push_back ( Filter ( "TIFF (*.tiff *.tif)", "*.tiff,*tif" ) );
  filters.push_back ( Filter ( "PNG (*.png)", "*.png" ) );
  filters.push_back ( Filter ( "Digital Elevation Model (*.dem)", "*.dem" ) );
  filters.push_back ( Filter ( "All Files (*.*)", "*.*" ) );
  
  // Prompt the user.
  FilesResult results ( FileDialog::getLoadFileNames ( this, "Open Geospatial Data", filters ) );
  
  // Get the filenames.
  FileNames filenames ( results.first );
  
  // Add the filenames to our list.
  for ( FileNames::iterator iter = filenames.begin(); iter != filenames.end (); ++iter )
    _listView->addItem ( iter->c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add images to caller.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );

  if ( false == al.valid () )
    return;

  for ( int i = 0; i < _listView->count (); ++i )
  {
    QListWidgetItem *item ( _listView->item ( i ) );

    if( 0x0 != item )
    {
      std::string filename ( item->text().toStdString () );
      std::string ext ( Usul::File::extension ( filename ) );
      
      if ( "dem" == ext )
      {
        Minerva::Core::Layers::ElevationLayerDem::RefPtr layer ( new Minerva::Core::Layers::ElevationLayerDem );
        layer->open ( filename );
        layer->name ( filename );
        
        al->addLayer ( Usul::Interfaces::ILayer::QueryPtr ( layer ) );
      }
      else
      {
        Minerva::Core::Layers::RasterLayerOssim::RefPtr layer ( new Minerva::Core::Layers::RasterLayerOssim );
        layer->open ( filename );
        layer->name ( filename );
      
        al->addLayer ( Usul::Interfaces::ILayer::QueryPtr ( layer ) );
      }
    }
  }
}
