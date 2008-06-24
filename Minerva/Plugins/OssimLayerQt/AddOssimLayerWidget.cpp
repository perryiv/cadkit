
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

#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
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
  
  // We want exteneded selection.
  _listView->setSelectionMode ( QAbstractItemView::ExtendedSelection );

  QPushButton *browse ( new QPushButton ( "Browse..." ) );
  QPushButton *search ( new QPushButton ( "Search Directory..." ) );
  QPushButton *remove ( new QPushButton ( "Remove Selected Files" ) );

  connect ( browse, SIGNAL ( clicked() ), this, SLOT ( _browseClicked () ) );
  connect ( search, SIGNAL ( clicked() ), this, SLOT ( _searchDirectoryClicked () ) );
  connect ( remove, SIGNAL ( clicked() ), this, SLOT ( _removeSelectedFiles() ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  topLayout->addWidget ( browse );
  topLayout->addWidget ( search );
  topLayout->addWidget ( _listView );
  topLayout->addWidget ( remove );
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
    _listView->addItem ( dir );
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
      
      if ( Usul::File::IsDirectory::test ( filename ) )
      {
        Minerva::Core::Layers::RasterGroup::RefPtr group ( new Minerva::Core::Layers::RasterGroup );
        group->name ( filename );
        
        // Add the job to the manager.
        Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind2 ( group, filename, &AddOssimLayerWidget::_searchDirectory ), caller, true ) );
        Usul::Jobs::Manager::instance().addJob ( job );
        
        al->addLayer ( Usul::Interfaces::ILayer::QueryPtr ( group ) );
      }
      else
      {
        std::string ext ( Usul::File::extension ( filename ) );
        
        Usul::Interfaces::IRead::QueryPtr read ( Minerva::Core::Factory::Readers::instance().create ( ext ) );
        
        if ( read.valid() )
        {
          // Create a job to read the file.
          // Pass the QueryPtr to memberFunction so that if the user removes the layer, there is still a reference until the job finishes.
          Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind3 ( filename, caller, static_cast<Usul::Interfaces::IUnknown*> ( 0x0 ),
                                        Usul::Adaptors::memberFunction ( read, &Usul::Interfaces::IRead::read ) ), caller ) );

          // Add the job to the manager.
          Usul::Jobs::Manager::instance().addJob ( job );
        }
        
        al->addLayer ( Usul::Interfaces::ILayer::QueryPtr ( read ) );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove selected files.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::_removeSelectedFiles()
{
  typedef QList<QListWidgetItem *> Items;
  Items items ( _listView->selectedItems() );
  for ( Items::iterator iter = items.begin(); iter != items.end(); ++iter )
  {
    _listView->takeItem ( _listView->row ( *iter ) );
    delete *iter;
  }
  
  _listView->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Search a directory and add to a group.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::_searchDirectory ( Minerva::Core::Layers::RasterGroup::RefPtr group, const std::string directory )
{
  if ( false == group.valid() )
    return;
  
  typedef QtTools::FileDialog::Filters Filters;
  typedef std::vector<std::string>     Strings;
  Strings filenames;
  
  Filters filters ( Minerva::Core::Factory::Readers::instance().filters() );
  
  std::set<std::string> extensions;
  
  for ( Filters::const_iterator iter = filters.begin(); iter != filters.end(); ++iter )
  {
    std::string extension ( iter->second );
    
    Strings strings;
    Usul::Strings::split ( extension, ",", false, strings );
    for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
      extensions.insert ( Usul::File::extension ( *iter ) );
  }
  
  typedef boost::filesystem::directory_iterator Iterator;
  
  Iterator iter ( directory );
  Iterator end;
  for( ; iter != end; ++iter )
  {
    const boost::filesystem::path &path = BOOST_FILE_SYSTEM_ITERATOR_TO_PATH ( iter );
    const std::string name ( path.native_directory_string() );
    
    // Make a recursive call if its a directory.
    if ( boost::filesystem::is_directory ( BOOST_FILE_SYSTEM_ITERATOR_TO_STATUS ( iter ) ) )
    {
      Minerva::Core::Layers::RasterGroup::RefPtr subGroup ( new Minerva::Core::Layers::RasterGroup );
      subGroup->showLayer ( false );
      subGroup->name ( name );
      group->append ( subGroup.get() );
      
      // Search this directory.
      AddOssimLayerWidget::_searchDirectory ( subGroup, name );
      
      if ( 0 == subGroup->size() )
        group->remove ( subGroup.get() );
    }
    
    // Add it to our list if its a file and the extenstion matches.
    else if ( extensions.end() != extensions.find ( Usul::File::extension ( name ) ) )
    {
      const std::string extension ( Usul::File::extension ( name ) );
      const std::string base ( Usul::File::base ( name ) );
      
      if ( "adf" == extension && "w001001" != base )
        continue;
      
      try
      {
        Usul::Interfaces::IRead::QueryPtr read ( Minerva::Core::Factory::Readers::instance().create ( extension ) );
        
        if ( read.valid() )
        {
          read->read ( name, 0x0, 0x0 );
        }
        
        Usul::Interfaces::ITreeNode::QueryPtr tn ( read );
        if ( tn.valid() )
          tn->setTreeNodeName ( base + "." + extension );
        
        Usul::Interfaces::IBooleanState::QueryPtr state ( read );
        if ( state.valid() )
          state->setBooleanState ( false );
          
        group->append ( Usul::Interfaces::IRasterLayer::QueryPtr ( read ) );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2020505170" );
    }
  }
}
