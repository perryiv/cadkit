
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

#ifndef _FOX_TOOLS_FILE_SELECTION_DIALOG_H_
#define _FOX_TOOLS_FILE_SELECTION_DIALOG_H_

#include "FoxTools/Export/Export.h"

#include <string>
#include <vector>

namespace FX { class FXWindow; };


namespace FoxTools {
namespace Dialogs {


class FOX_TOOLS_EXPORT FileDialog
{
public:

  // Useful typedefs.
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector<Filter> Filters;
  typedef std::string Filename;
  typedef std::vector<Filename> Filenames;
  typedef std::pair<Filename,Filter> FileResult;
  typedef std::pair<Filenames,Filter> FilesResult;

  // The type of dialog.
  enum Type { OPEN, SAVE };

  // Constructor/destructor.
  FileDialog();
  FileDialog ( const Type &type, 
               const std::string &title, 
               const Filters &filters = Filters(), 
               unsigned int initialFilter = 0 );
  virtual ~FileDialog();

  // Ask the user for file names.
  static FilesResult            askForFileNames ( FX::FXWindow *owner, const Type &type, const std::string &title, const Filters &filters = Filters() );
  static FileResult             askForFileName  ( FX::FXWindow *owner, const Type &type, const std::string &title, const Filters &filters = Filters() );

  // Get the i'th filename selected. Throws if not available.
  const Filename &              filename ( unsigned int i ) const { return _filenames.at ( i ); }

  // Direct access to the filenames.
  const Filenames &             filenames() const { return _filenames; }

  // Get the i'th filter. Throws if not available.
  const Filter &                filter ( unsigned int i ) const { return _filters.at ( i ); }

  // Direct access to the filters.
  const Filters &               filters() const { return _filters; }
  Filters &                     filters()       { return _filters; }
  void                          filters ( const Filters &f ) { _filters = f; }

  // Set/get the index of the filter when the dialog opens/closes.
  unsigned int                  filterIndex() const { return _filterIndex; }
  void                          filterIndex ( unsigned int index );

  // Get the number of filenames selected.
  unsigned int                  numFilenames() const { return _filenames.size(); }

  // Get the number of filter patterns added.
  unsigned int                  numFilters() const { return _filters.size(); }

  // Set/get the default filename.
  void                          defaultFilename ( const std::string &f ) { _defaultFile = f; }
  const std::string &           defaultFilename() const { return _defaultFile; }

  // Turn on/off confirmation of filename existence.
  void                          confirmFileExists ( bool state );
  bool                          confirmFileExists() const;

  // Turn on/off hiding of read-only files.
  void                          hideReadOnly ( bool state );
  bool                          hideReadOnly() const;

  // Turn on/off multiple filename selection.
  void                          multipleFiles ( bool state );
  bool                          multipleFiles() const;

  // Set/get the placement. TODO, currently ignored.
  unsigned int                  placement() const { return _placement; }
  void                          placement ( unsigned int p ) { _placement = p; }

  // Set/get the title.
  const std::string &           title() const { return _title; }
  void                          title ( const std::string &t ) { _title = t; }

  // Set/get the type of dialog.
  Type                          type() const { return _type; }
  void                          type ( Type t ) { _type = t; }

  // Run the dialog in a modal loop.
  bool                          runModal ( FX::FXWindow *owner );

protected:

  void                          _makePatterns ( std::string & ) const;

private:

  // Possible flags.
  enum
  {
    CONFIRM_FILE_EXISTS  = 0x00000001,
    HIDE_READ_ONLY       = 0x00000002,
    ALLOW_MULTIPLE_FILES = 0x00000004,
  };

  std::string _title;
  std::string _defaultFile;
  unsigned int _placement;
  unsigned int _filterIndex;
  unsigned int _flags;
  Filters _filters;
  Filenames _filenames;
  Type _type;
};


}; // namespace Dialogs
}; // namespace FoxTools


#endif // _FOX_TOOLS_FILE_SELECTION_DIALOG_H_
