
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/EditWmsLayerWidget.h"
#include "Minerva/Qt/Widgets/OptionsDialog.h"
#include "Minerva/Qt/Widgets/AlphasDialog.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/File/Boost.h"
#include "Usul/Strings/Format.h"
#include "Usul/Scope/Caller.h"

#include "QtGui/QMessageBox"

#include "boost/algorithm/string/replace.hpp"

#ifdef _MSC_VER // Visual C++
# define NOMINMAX
# include <windows.h>
# include <shlobj.h>
#if ( NTDDI_VERSION < NTDDI_WINXP ) // Not sure why the prototype is pre-processed out.
SHSTDAPI SHOpenFolderAndSelectItems ( PCIDLIST_ABSOLUTE pidlFolder, UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY apidl, DWORD dwFlags );
#endif
#endif

#include <list>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EditWmsLayerWidget::EditWmsLayerWidget ( RasterLayerNetwork *layer, QWidget * parent ) : BaseClass ( parent ),
  _layer ( layer )
{
  // Initialize code from designer.
  this->setupUi ( this );
  
  if ( _layer.valid() )
  {
    _name->setText( _layer->name().c_str() );
    _server->setText ( _layer->urlBase().c_str() );
    
    // Disable counting of the number of files in the cache until this is threaded so that it doesn't take a long time to show the dialog.
#if 0
    // Find the number of files.
    typedef std::list<std::string> Filenames;
    Filenames files;
    Usul::File::findFiles ( _layer->cacheDirectory(), _layer->cacheFileExtension(), files );
    _cacheInfoText->setText ( Usul::Strings::format ( files.size() ).c_str() );
#else
    _cacheInfoText->setVisible ( false );
    _numberOfFilesLabel->setVisible ( false );
#endif
    
    _cacheDirectoryText->setText ( _layer->cacheDirectory().c_str() );
  }
  
  // Currently implemented on OS X, and Windows.
#ifdef __linux
  viewCacheButton->setVisible ( false );
#endif
  
  // Connect slots and signals.
  this->connect ( _name, SIGNAL ( editingFinished() ), this, SLOT ( _nameFinishedEditing() ) );
  this->connect ( _server, SIGNAL ( editingFinished() ), this, SLOT ( _serverFinishedEditing() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EditWmsLayerWidget::~EditWmsLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the new name.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::_nameFinishedEditing()
{
  if ( _layer.valid() )
    _layer->name ( _name->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the new server.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::_serverFinishedEditing()
{
  if ( _layer.valid() )
    _layer->urlBase ( _server->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The view options button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_viewOptionsButton_clicked()
{
  if ( _layer.valid() )
  {
    Minerva::Widgets::OptionsDialog dialog ( _layer->options(), this );
    
    if ( QDialog::Accepted == dialog.exec() )
      _layer->options ( dialog.options() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The view options button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_viewAlphasButton_clicked()
{
  if ( _layer.valid() )
  {
    RasterLayerNetwork::Alphas alphas ( _layer->alphas() );
    
    Minerva::Widgets::AlphasDialog dialog ( alphas, this );
    
    if ( QDialog::Accepted == dialog.exec() )
      _layer->alphas ( dialog.alphas() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the cache.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_deleteCacheButton_clicked()
{
  if ( 0x0 == _layer )
    return;
  
  const std::string message ( "Are you sure you want to delete the cache?" );
  if ( QMessageBox::Ok != QMessageBox::question ( this, "Confirm", message.c_str(), QMessageBox::Ok | QMessageBox::Cancel ) )
    return;
  
  const std::string directory ( _layer->cacheDirectory() );
  if ( boost::filesystem::exists ( directory ) && boost::filesystem::is_directory ( directory ) )
    boost::filesystem::remove_all ( directory );
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the cache.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_viewCacheButton_clicked()
{
  if ( 0x0 == _layer )
    return;
  
#ifdef __APPLE__

  const std::string directory ( _layer->cacheDirectory() );
  const std::string command ( Usul::Strings::format ( "open ", directory ) );
  ::system ( command.c_str() );

#elif _WIN32

  const std::string directory ( _layer->cacheDirectory() );

	// Initialize COM. Ensure it gets uninitialized too.
	::CoInitializeEx ( 0x0, COINIT_MULTITHREADED );
  Usul::Scope::Caller::RefPtr uninitialize ( Usul::Scope::makeCaller ( ::CoUninitialize ) );

	// NOTE: ILCreateFromPath will reject the path if it contains / instead of \.
	std::string path ( directory );
	boost::algorithm::replace_all ( path, "/", "\\" );

  {
	  // http://msdn.microsoft.com/en-us/library/bb776438(VS.85).aspx
	  ITEMIDLIST *pidl ( ::ILCreateFromPath ( path.c_str() ) );

	  // http://msdn.microsoft.com/en-us/library/bb776441(VS.85).aspx
    Usul::Scope::Caller::RefPtr freeItemList ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( pidl, ::ILFree ) ) );

	  // http://msdn.microsoft.com/en-us/library/bb762232(VS.85).aspx
	  ::SHOpenFolderAndSelectItems ( pidl, 0, 0x0, 0 );
  }

#endif
}
