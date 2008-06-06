
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "FileDialog.h"

#include "Usul/File/Path.h"
#include "Usul/Registry/Database.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"
#include "Usul/User/Directory.h"

#include "QtGui/QFileDialog"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constants for the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  // Registry section.
  const std::string FILE_DIALOG               ( "file_dialog" );

  // File dialog properties.
  const std::string LAST_DIRECTORY            ( "last_directory" );
  const std::string LAST_FILTER               ( "last_filter" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to load from.
//
///////////////////////////////////////////////////////////////////////////////

FileDialog::FileResult FileDialog::getLoadFileName  ( QWidget* parent, const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE_STATIC;
  const FilesResult result ( FileDialog::getLoadFileNames ( parent, title, filters ) );
  return ( ( result.first.empty() ) ? FileResult() : FileResult ( result.first.front(), result.second ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the names of the files to load from.
//
///////////////////////////////////////////////////////////////////////////////

FileDialog::FilesResult FileDialog::getLoadFileNames ( QWidget* parent, const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE_STATIC;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "3678389711" );
  
  // Initialize the answer.
  FilesResult files;
  
  // Set the filter string.
  const std::string allFilters ( FileDialog::_formatFilters ( filters ) );
  
  // Get the directory.
  const std::string dir ( FileDialog::_lastFileDialogDir ( title ) );
  
  // Get the current filter.
  QString filter ( FileDialog::_lastFileDialogFilter( title ).c_str() );
  
  // Need to use this static function to get native file dialog.
  QStringList answer ( QFileDialog::getOpenFileNames ( parent, title.c_str(), dir.c_str(), allFilters.c_str(), &filter ) );
  
  // If we have at least one file.
  if( false == answer.empty() )
  {
    std::string filename ( answer.first().toStdString() );
    std::string directory ( Usul::File::directory ( filename, false ) );
    
    // Save the directory and filter.
    FileDialog::_lastFileDialogDir    ( title, directory );
    FileDialog::_lastFileDialogFilter ( title, filter.toStdString() );
    
    files.second.first = filter.toStdString();
    
    // Fox and WinForms both use a string for the filter "label" and another 
    // one for the actual file extensions. Qt does not, so the fact that 
    // files.second is a pair is legacy.
    files.second.second = files.second.first;
  }
  
  // Convert the answer.
  for( QStringList::const_iterator iter = answer.begin(); iter != answer.end(); ++iter )
  {
    files.first.push_back ( iter->toStdString() );
  }
  
  // Return the result, which may be an empty list.
  return files;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to save to
//
///////////////////////////////////////////////////////////////////////////////

FileDialog::FileResult FileDialog::getSaveFileName  ( QWidget* parent, const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE_STATIC;
  USUL_THREADS_ENSURE_GUI_THREAD_OR_THROW ( "1901356729" );
  
  // Initialize the answer.
  FileResult file;
  
  // Set the filter string.
  const std::string allFilters ( FileDialog::_formatFilters ( filters ) );
  
  // Get the current filter.
  QString filter ( FileDialog::_lastFileDialogFilter ( title ).c_str() );
  
  // Get the directory.
  const std::string dir ( FileDialog::_lastFileDialogDir ( title ) );
  
  // Need to use this static function to get native file dialog.
  QString answer ( QFileDialog::getSaveFileName ( parent, title.c_str(), dir.c_str(), allFilters.c_str(), &filter ) );
  
  if( 0 != answer.size() )
  {
    std::string filename ( answer.toStdString() );
    std::string directory ( Usul::File::directory ( filename, false ) );
    
    // Save the directory and filter.
    FileDialog::_lastFileDialogDir    ( title, directory );
    FileDialog::_lastFileDialogFilter ( title, filter.toStdString() );
    
    file.first = filename;
  }
  
  return file;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Format the given filters into a single string.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::_formatFilters ( const Filters &filters ) 
{
  USUL_TRACE_SCOPE_STATIC;
  
  // Initialize the answer.
  QStringList allFilters;
  
  // Loop through and append each filter.
  for ( Filters::const_iterator i = filters.begin(); i != filters.end(); ++i )
  {
    const std::string filter ( i->first );
    allFilters.push_back ( filter.c_str() );
  }
  
  // Sort the filters.
  allFilters.sort();
  
  // Put a semi-colon between each filter.
  QString answer ( allFilters.join ( ";;" ) );
  
  // Return the answer.
  return answer.toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::_lastFileDialogDir ( const std::string &title ) 
{
  USUL_TRACE_SCOPE_STATIC;
  
  // Copy the title and replace any white space.
  std::string key ( title );
  std::replace ( key.begin(), key.end(), ' ', '_' );
  
  // Get the value and return it.
  const std::string defaultDir ( Usul::User::Directory::documents ( true, false ) );
  const std::string dir ( Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_DIRECTORY][key].get ( defaultDir ) );
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last directory.
//
///////////////////////////////////////////////////////////////////////////////

void FileDialog::_lastFileDialogDir ( const std::string &title, const std::string &dir )
{
  USUL_TRACE_SCOPE_STATIC;
  
  // Copy the title and replace any white space.
  std::string key ( title );
  std::replace ( key.begin(), key.end(), ' ', '_' );
  
  // Set the value and return it.
  Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_DIRECTORY][key] = dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last file dialog filter.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileDialog::_lastFileDialogFilter ( const std::string &title ) 
{
  USUL_TRACE_SCOPE_STATIC;
  
  // Copy the title and replace any white space.
  std::string key ( title );
  std::replace ( key.begin(), key.end(), ' ', '_' );
  
  const std::string filter ( Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_FILTER][key].get ( "" ) );
  return filter;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last file dialog filter.
//
///////////////////////////////////////////////////////////////////////////////

void FileDialog::_lastFileDialogFilter ( const std::string &title, const std::string &filter ) 
{
  USUL_TRACE_SCOPE_STATIC;
  
  // Copy the title and replace any white space.
  std::string key ( title );
  std::replace ( key.begin(), key.end(), ' ', '_' );
  
  Usul::Registry::Database::instance()[Detail::FILE_DIALOG][Detail::LAST_FILTER][key] = filter;
}
