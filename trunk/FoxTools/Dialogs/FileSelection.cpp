
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File selection dialog.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
#define _CPP_CMATH 1
#endif

#include "FoxTools/Dialogs/FileSelection.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/FileDialog.h"
#include "FoxTools/Headers/App.h"

#include "Usul/Bits/Bits.h"

#include <sstream>
#include <algorithm>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX            // Do not define min and max as macros.
# include <windows.h>
# include <commdlg.h>        // For OPENFILENAME
#define SLASH '\\'
#else
#define SLASH '/'
#endif

#define FILTER_INDEX "filter_index"

using namespace FoxTools;
using namespace Dialogs;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

FileSelection::FileSelection() :
  _title       ( "Default Title" ),
  _defaultFile ( "filename" ),
  _placement   ( FX::PLACEMENT_OWNER ),
  _filterIndex ( 0 ),
  _flags       ( 0 ),
  _filters     (),
  _filenames   (),
  _type        ( FileSelection::OPEN )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FileSelection::FileSelection ( const Type &type, 
                               const std::string &title, 
                               const Filters &filters, 
                               unsigned int initialFilter ) :
  _title       ( title ),
  _defaultFile ( "filename" ),
  _placement   ( FX::PLACEMENT_OWNER ),
  _filterIndex ( ( initialFilter < filters.size() ) ? initialFilter : 0 ),
  _flags       ( ( FileSelection::OPEN == type ) ? ( CONFIRM_FILE_EXISTS | ALLOW_MULTIPLE_FILES ) : 0 ),
  _filters     ( filters ),
  _filenames   (),
  _type        ( type )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FileSelection::~FileSelection()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the dialog in a modal loop.
//
///////////////////////////////////////////////////////////////////////////////

bool FileSelection::runModal ( FX::FXWindow *owner )
{
  // Initialize.
  _filenames.clear();

  // Construct the pattern string.
  std::string patterns;
  this->_makePatterns ( patterns );

#ifdef _WIN32

  // The string that will hold the filenames. It is also used for the initial 
  // filename displayed.
  const unsigned int maxNumFilenames ( 1024 );
  const DWORD maxStringLength ( maxNumFilenames * MAX_PATH );
  char pathnames[maxStringLength];
  ::memset ( pathnames, '\0', maxStringLength );
  ::strcpy ( pathnames, _defaultFile.c_str() );

  // The struct we fill to define the dialog.
  OPENFILENAME dialog;
  dialog.lStructSize = sizeof (OPENFILENAME);
  dialog.hwndOwner = ( owner ) ? (HWND) owner->id() : ::GetForegroundWindow();
  dialog.hInstance = NULL;
  dialog.lpstrFilter = patterns.c_str();
  dialog.lpstrCustomFilter = NULL;
  dialog.nMaxCustFilter = 0;
  dialog.nFilterIndex = _filterIndex + 1;
  dialog.lpstrFile = pathnames;
  dialog.nMaxFile = maxStringLength;
  dialog.lpstrFileTitle = NULL;
  dialog.nMaxFileTitle = 0;
  dialog.lpstrInitialDir = NULL;
  dialog.lpstrTitle = _title.c_str();
  dialog.Flags = OFN_EXPLORER;
  dialog.nFileOffset = 0;
  dialog.nFileExtension = 0;
  dialog.lpstrDefExt = "*.*";
  dialog.lCustData = 0;
  dialog.lpfnHook = 0x0;
  dialog.lpTemplateName = NULL;
  #if (_WIN32_WINNT >= 0x0500)
  dialog.FlagsEx = 0;
  #endif

  // Append to the flags.
  dialog.Flags |= ( Usul::Bits::has ( _flags, FileSelection::CONFIRM_FILE_EXISTS  ) ) ? ( OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST ) : 0;
  dialog.Flags |= ( Usul::Bits::has ( _flags, FileSelection::HIDE_READ_ONLY       ) ) ?   OFN_READONLY                            : 0;
  dialog.Flags |= ( Usul::Bits::has ( _flags, FileSelection::ALLOW_MULTIPLE_FILES ) ) ?   OFN_ALLOWMULTISELECT                    : 0;
  dialog.Flags |= ( FileSelection::SAVE == _type )                                    ?   OFN_OVERWRITEPROMPT                     : 0;

  // Set the function to call.
  typedef BOOL APIENTRY Function ( LPOPENFILENAME lpofn );
  Function *function = ( FileSelection::OPEN == _type ) ? &::GetOpenFileName : &::GetSaveFileName;

  // Open the file selection dialog. This will return false if the user canceled.
  if ( FALSE == function ( &dialog ) )
  {
    // The user canceled so just return.
    return false;
  }

  // Assign the filter used. The dialog's filter index starts at 1. 
  // Ours starts at 0.
  _filterIndex = dialog.nFilterIndex - 1;

  // The filenames are null-separated with two nulls at the end. We need to 
  // loop through and grab all the filenames. Note: the pointer "filename" 
  // will not be null, but the value at that address will be null.
  {
    char *filename = pathnames;
    unsigned int count ( 0 );
    while ( ( count < maxStringLength ) && ( NULL != *filename ) )
    {
      // Grab this filename (it stops at the null character).
      std::string temp ( filename );

      // Push it into our list.
      _filenames.push_back ( temp );

      // Skip ahead to the start of the next filename.
      count += temp.size() + 1;
      filename = pathnames + count;
    }
  }

  // If there is more than one filename then the first "filename" is really 
  // the directory.
  if ( _filenames.size() > 1 )
  {
    // Grab the first one, it's the directory (make a hard copy).
    Filenames::iterator i = _filenames.begin();
    std::string dir ( *i );

    // Append the slash.
    dir += SLASH;

    // Pop this off the list.
    _filenames.erase ( _filenames.begin() );

    // Loop through all the rest of the filenames.
    for ( i = _filenames.begin(); i != _filenames.end(); ++i )
    {
      // Prepend the directory.
      (*i) = dir + (*i);
    }
  }

#else

  // Make a file-selection dialog on the stack.
  FX::FXFileDialog dialog ( ( ( owner ) ? owner : FoxTools::Functions::mainWindow() ), _title.c_str() );

  // Set the default filename.
  dialog.setFilename ( _defaultFile.c_str() );

  // Set the patterns.
  dialog.setPatternList ( patterns.c_str() );

  // Set initial pattern.
  dialog.setCurrentPattern ( _filterIndex );

  // Run the dialog in a modal loop.
  if ( dialog.execute ( _placement ) )
  {
    // Set the filter index.
    _filterIndex = dialog.getCurrentPattern();

    // Get the filenames.
    FX::FXString *path = dialog.getFilenames();
    while ( path && !path->empty() )
    {
      std::string temp ( path->text() );
      _filenames.push_back ( temp );
      path += 1;
    }
  }

#endif

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set multiple file selection.
//
///////////////////////////////////////////////////////////////////////////////

void FileSelection::multipleFiles ( bool state )
{
  _flags = ( state ) ? Usul::Bits::add<unsigned int, int>    ( _flags, FileSelection::ALLOW_MULTIPLE_FILES )
                     : Usul::Bits::remove<unsigned int, int> ( _flags, FileSelection::ALLOW_MULTIPLE_FILES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get multiple file selection.
//
///////////////////////////////////////////////////////////////////////////////

bool FileSelection::multipleFiles() const
{
  return Usul::Bits::has<unsigned int, int> ( _flags, FileSelection::ALLOW_MULTIPLE_FILES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set hiding of read-only files.
//
///////////////////////////////////////////////////////////////////////////////

void FileSelection::hideReadOnly ( bool state )
{
  _flags = ( state ) ? Usul::Bits::add<unsigned int, int>    ( _flags, FileSelection::HIDE_READ_ONLY )
                     : Usul::Bits::remove<unsigned int, int> ( _flags, FileSelection::HIDE_READ_ONLY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set hiding of read-only files.
//
///////////////////////////////////////////////////////////////////////////////

bool FileSelection::hideReadOnly() const
{
  return Usul::Bits::has<unsigned int, int> ( _flags, FileSelection::HIDE_READ_ONLY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set confirmation of filename existence.
//
///////////////////////////////////////////////////////////////////////////////

void FileSelection::confirmFileExists ( bool state )
{
  _flags = ( state ) ? Usul::Bits::add<unsigned int, int>    ( _flags, FileSelection::CONFIRM_FILE_EXISTS )
                     : Usul::Bits::remove<unsigned int, int> ( _flags, FileSelection::CONFIRM_FILE_EXISTS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get confirmation of filename existence.
//
///////////////////////////////////////////////////////////////////////////////

bool FileSelection::confirmFileExists() const
{
  return Usul::Bits::has<unsigned int, int> ( _flags, FileSelection::CONFIRM_FILE_EXISTS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the filter patterns.
//
///////////////////////////////////////////////////////////////////////////////

void FileSelection::_makePatterns ( std::string &patterns ) const
{
  // Initialize.
  patterns.clear();

  // Make a copy of the list.
  Filters filters ( _filters );

  // If we don't have any patterns then use a default.
  if ( 0 == filters.size() )
    filters.insert ( filters.end(), Filter ( "All Files (*.*)", "*.*" ) );

  // Loop through all the filters.
  for ( Filters::const_iterator i = filters.begin(); i != filters.end(); ++i )
  {
    // Get the name and pattern.
    const std::string &name    = i->first;
    const std::string &pattern = i->second;

    #ifdef _WIN32

      // Append the pattern.
      patterns += name;
      patterns.append ( 1, '\0' );
      patterns += pattern;
      patterns.append ( 1, '\0' );

    #else

      // Append the names.
      patterns += ( name + "\n" );

    #endif
  }

  #ifdef _WIN32

    // The very end is terminated by two null characters. Since the strings in 
    // the list are already terminated by a null, we just need one more.
    patterns += '\0';

  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the index of the filter selected when the dialog opens.
//
///////////////////////////////////////////////////////////////////////////////

void FileSelection::filterIndex ( unsigned int index )
{
  _filterIndex = std::min ( _filters.size() - 1, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ask the user for a list of file names.
//
///////////////////////////////////////////////////////////////////////////////

FileSelection::FilesResult FileSelection::askForFileNames ( const Type &type, const std::string &title, const Filters &filters, FX::FXWindow *owner )
{
  // Make main window owner if given null.
  if ( 0x0 == owner )
    owner = FoxTools::Functions::mainWindow();

  // Declare and configure a file dialog.
  FileSelection dialog ( type, title, filters );

  // Make the registry section.
  std::ostringstream section;
  std::string name ( title );
  std::transform ( name.begin(), name.end(), name.begin(), ::tolower );
  std::replace ( name.begin(), name.end(), ' ', '_' );
  section << "file_dialog_" << name;

  // Set the filter index.
  dialog.filterIndex ( ( title.empty() ) ? 0 : FoxTools::Functions::application()->reg().readUnsignedEntry ( section.str().c_str(), FILTER_INDEX, dialog.filterIndex() ) );

  // Run the dialog in a modal loop.
  if ( !dialog.runModal ( owner ) )
    return FilesResult();

  // Push the filter index back into the registry.
  FoxTools::Functions::application()->reg().writeUnsignedEntry ( section.str().c_str(), FILTER_INDEX, dialog.filterIndex() );

  // Return the file names.
  return FilesResult ( dialog.filenames(), dialog.filter ( dialog.filterIndex() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ask the user for a single file name.
//
///////////////////////////////////////////////////////////////////////////////

FileSelection::FileResult FileSelection::askForFileName ( const Type &type, const std::string &title, const Filters &filters, FX::FXWindow *owner )
{
  FilesResult result ( FileSelection::askForFileNames ( type, title, filters, owner ) );
  return ( ( result.first.empty() ) ? FileResult() : FileResult ( result.first.front(), result.second ) );
}
