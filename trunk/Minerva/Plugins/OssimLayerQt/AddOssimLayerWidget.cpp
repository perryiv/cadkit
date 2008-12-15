
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

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Polygon.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Functions/ReadFile.h"
#include "Minerva/Core/Functions/SearchDirectory.h"
#include "Minerva/Interfaces/IAddLayer.h"

#include "QtTools/FileDialog.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Split.h"
#include "Usul/User/Directory.h"

#include "QtGui/QCheckBox"
#include "QtGui/QFileDialog"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"
#include "QtGui/QVBoxLayout"

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
  _listView ( 0x0 ),
  _loadDataCheck ( new QCheckBox ),
  _showExtentsCheck ( new QCheckBox )
{
  // Set initial check state.
  _loadDataCheck->setChecked ( true );
  _showExtentsCheck->setChecked ( false );
  
  // Set the txt.
  _loadDataCheck->setText ( "Load data" );
  _showExtentsCheck->setText ( "Show data extents" );

  // Make the list.
  _listView = new QListWidget ( this );
  
  // We want exteneded selection.
  _listView->setSelectionMode ( QAbstractItemView::ExtendedSelection );

  QPushButton *browse ( new QPushButton ( "Browse..." ) );
  QPushButton *search ( new QPushButton ( "Search Directory..." ) );
  QPushButton *remove ( new QPushButton ( "Remove Selected Files" ) );

  QObject::connect ( browse, SIGNAL ( clicked() ), this, SLOT ( _browseClicked () ) );
  QObject::connect ( search, SIGNAL ( clicked() ), this, SLOT ( _searchDirectoryClicked () ) );
  QObject::connect ( remove, SIGNAL ( clicked() ), this, SLOT ( _removeSelectedFiles() ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  topLayout->addWidget ( browse );
  topLayout->addWidget ( search );
  topLayout->addWidget ( _listView );
  topLayout->addWidget ( _loadDataCheck );
  topLayout->addWidget ( _showExtentsCheck );
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
  if ( 0x0 == _listView )
    return;

  // How many items.
  const unsigned int size ( _listView->count() );
  
  // Vector to contain filenames (or directories).
  Filenames names;
  names.reserve ( size );

  for ( unsigned int i = 0; i < size; ++i )
  {
    QListWidgetItem *item ( _listView->item ( i ) );

    if( 0x0 != item )
    {
      names.push_back ( item->text().toStdString() );
    }
  }

  if ( Qt::Checked == _loadDataCheck->checkState() )
  {
    // Add the job to the manager.
    Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind3 ( names, parent, caller, &AddOssimLayerWidget::_loadData ), caller, true ) );
    Usul::Jobs::Manager::instance().addJob ( job );
  }
  if ( Qt::Checked == _showExtentsCheck->checkState() )
  {
    // Add the job to the manager.
    Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind3 ( names, parent, caller, &AddOssimLayerWidget::_showDataExtents ), caller, true ) );
    Usul::Jobs::Manager::instance().addJob ( job );
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Load the data.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::_loadData ( Filenames filenames, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );

  if ( false == al.valid () )
    return;

  for ( Filenames::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter )
  {
    const std::string filename ( *iter );

    // See if the file is a directory...
    if ( Usul::File::IsDirectory::test ( filename ) )
    {
      Minerva::Core::Layers::RasterGroup::RefPtr group ( new Minerva::Core::Layers::RasterGroup );
      group->name ( filename );

      // Add the group.
      al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( group ) );

      // Search the directory (and all sub-directories).
      Minerva::Core::Functions::searchDirectory ( *group, filename );
    }
    else
    {
      Usul::Interfaces::IUnknown::QueryPtr unknown ( Minerva::Core::Functions::readFile ( filename ) );
      al->addLayer ( unknown );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to get color based on filename.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class ColorChooser
  {
  public:

    typedef std::map<std::string,Usul::Math::Vec4f> ColorMap;

    ColorChooser() : _random ( 0.0f, 1.0f ), _colors() 
    {
    }

    Usul::Math::Vec4f operator () ( const std::string& filename )
    {
      const std::string ext ( Usul::File::extension ( filename ) );
      
      ColorMap::const_iterator iter ( _colors.find ( ext ) );
      if ( iter != _colors.end() )
        return iter->second;

      Usul::Math::Vec4f color ( _random(), _random(), _random(), 1.0 );
      _colors.insert ( std::make_pair ( ext, color ) );
      return color;
    }
  private:
    Usul::Adaptors::Random<float> _random;
    ColorMap _colors;

  } colors;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make polygon for extents of filename.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  Minerva::Core::Data::DataObject* makeExtentsPolygon ( const std::string& filename )
  {
    // Typedefs.
    typedef Minerva::Core::Data::DataObject DataObject;
    typedef Minerva::Core::Data::PolyStyle PolyStyle;
    typedef Minerva::Core::Data::Polygon Polygon;
    typedef Polygon::Vertices Vertices;
    typedef Polygon::Vertex Vertex;

    // Make the data object.
    DataObject::RefPtr object ( new DataObject );
    object->name ( filename );

    // Read the file.
    Usul::Interfaces::IUnknown::QueryPtr unknown ( Minerva::Core::Functions::readFile ( filename ) );

    if ( false == unknown.valid() )
      return 0x0;

    // Get the lat/lon extents.
    Usul::Interfaces::ILayerExtents::QueryPtr le ( unknown );
    if ( false == le.valid() )
      return 0x0;

    // Get the extents values.
    const double minLon ( le->minLon() );
    const double minLat ( le->minLat() );
    const double maxLon ( le->maxLon() );
    const double maxLat ( le->maxLat() );

    // Get the filesize.
    const unsigned long long size ( boost::filesystem::file_size ( filename ) );
    const double elevation ( static_cast<double> ( size ) / 100000.0 );

    // Make the polygon.
    Polygon::RefPtr polygon ( new Polygon );

    // The vertices.
    Vertices vertices;
    vertices.push_back ( Vertex ( minLon, minLat, elevation ) );
    vertices.push_back ( Vertex ( maxLon, minLat, elevation ) );
    vertices.push_back ( Vertex ( maxLon, maxLat, elevation ) );
    vertices.push_back ( Vertex ( minLon, maxLat, elevation ) );
    vertices.push_back ( Vertex ( minLon, minLat, elevation ) );

    polygon->outerBoundary ( vertices );
    polygon->altitudeMode ( Polygon::RELATIVE_TO_GROUND );
    polygon->extrude ( true );

    Usul::Math::Vec4f color ( Detail::colors ( filename ) );
    color[3] = 0.5;
    
    PolyStyle::RefPtr polyStyle ( new PolyStyle );
    polyStyle->color ( color );
    
    polygon->polyStyle ( polyStyle.get() );

    object->addGeometry ( polygon.get() );

    return object.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show data extents and file size.
//
///////////////////////////////////////////////////////////////////////////////

void AddOssimLayerWidget::_showDataExtents( Filenames filenames, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );

  if ( false == al.valid () )
    return;

  for ( Filenames::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter )
  {
    const std::string filename ( *iter );

    // See if the file is a directory...
    if ( Usul::File::IsDirectory::test ( filename ) )
    {
      Filenames names;

      typedef boost::filesystem::directory_iterator Iterator;

      Iterator iter ( filename );
      Iterator end;
      for( ; iter != end; ++iter )
      {
        const boost::filesystem::path &path = BOOST_FILE_SYSTEM_ITERATOR_TO_PATH ( iter );
        names.push_back ( path.native_directory_string() );
      }

      AddOssimLayerWidget::_showDataExtents ( names, parent, caller );
    }
    else
    {
      try
      {
        Minerva::Core::Data::DataObject::RefPtr object ( Detail::makeExtentsPolygon ( filename ) );
        al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( object ) );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1383982710" )
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
