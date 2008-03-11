
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

#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Split.h"
#include "Usul/User/Directory.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "QtTools/FileDialog.h"

#include "QtGui/QFileDialog"
#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"

#include "boost/filesystem/operations.hpp"
#include "Usul/File/Boost.h"

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

  QPushButton *browse ( new QPushButton ( "Browse..." ) );
  QPushButton *search ( new QPushButton ( "Search Directory..." ) );

  connect ( browse, SIGNAL ( clicked () ), this, SLOT ( _browseClicked () ) );
  connect ( search, SIGNAL ( clicked () ), this, SLOT ( _searchDirectoryClicked () ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  topLayout->addWidget ( browse );
  topLayout->addWidget ( search );
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
  
  Filters f ( Minerva::Core::Factory::Readers::instance().filters() );
  
  Filters filters ( f.begin(), f.end() );
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
//  Search a directory.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::_searchDirectoryClicked()
{
  QString dir ( QFileDialog::getExistingDirectory ( this, tr ( "Search Directory" ),
                                                  "",
                                                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks ) );
  
  if ( false == dir.isEmpty() )
  {
    typedef QtTools::FileDialog::Filters Filters;
    typedef std::vector<std::string>     Strings;
    Strings filenames;
    
    std::string directory ( dir.toStdString() );
    Filters filters ( Minerva::Core::Factory::Readers::instance().filters() );
    
    for ( Filters::const_iterator iter = filters.begin(); iter != filters.end(); ++iter )
    {
      std::string extension ( iter->second );
      
      Strings strings;
      Usul::Strings::split ( extension, ",", false, strings );
      
      // Find the files that we can load.
      for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
        Usul::File::findFiles ( directory, Usul::File::extension( *iter ), filenames );
    }
    
    // Add the filenames to our list.
    for ( Strings::iterator iter = filenames.begin(); iter != filenames.end (); ++iter )
      _listView->addItem ( iter->c_str() );
  }
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
      
      Usul::Interfaces::IRead::QueryPtr read ( Minerva::Core::Factory::Readers::instance().create ( ext ) );
      
      if ( read.valid() )
        read->read ( filename );
      
      al->addLayer ( Usul::Interfaces::ILayer::QueryPtr ( read ) );
    }
  }
}
