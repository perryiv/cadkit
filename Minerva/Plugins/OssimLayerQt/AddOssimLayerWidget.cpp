
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

#include "QtGui/QFileDialog"
#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"

#include <iostream>

namespace Detail
{
  const std::string SECTION  ( "add_file_system_layer" );
  const std::string KEY      ( "directory" );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddOssimLayerWidget::AddOssimLayerWidget( QWidget *parent ) : BaseClass ( parent ),
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
  try
  {
    std::string filters ( "All Files (*.*);;JPEG (*.jpg);;TIFF (*.tiff *.tif);;PNG (*.png);;Digital Elevation Model (*.dem);;OSSIM Key Word List (*kwl)" );

    // Get the last directory.
    const std::string defaultDir ( Usul::User::Directory::documents ( true, false ) );
    const std::string directory ( Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY].get ( defaultDir ) );

    // Need to use this static function to get native file dialog.
    QStringList answer ( QFileDialog::getOpenFileNames ( this, "Open Image", directory.c_str(), filters.c_str(), 0x0 ) );

    if ( false == answer.empty() )
    {
      std::string directory ( Usul::File::directory ( answer.first().toStdString(), false ) );
      Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY] = directory;
    }

    for ( QStringList::iterator iter = answer.begin(); iter != answer.end (); ++iter )
      _listView->addItem ( *iter );
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 3063467427: Standard exception caught: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 2199256948: Unknown exception caught." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add images to caller.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::apply ( Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( Usul::Documents::Manager::instance().activeDocument() );

  if ( false == al.valid () )
    return;

  for ( int i = 0; i < _listView->count (); ++i )
  {
    QListWidgetItem *item ( _listView->item ( i ) );

    if( 0x0 != item )
    {
      std::string filename ( item->text ().toStdString () );
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
