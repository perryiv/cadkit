
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_FILE_DIALOG_H__
#define __QT_TOOLS_FILE_DIALOG_H__

#include "QtTools/Export.h"

#include <string>
#include <vector>

class QWidget;

namespace QtTools {

class QT_TOOLS_EXPORT FileDialog
{
public:
  // Typedefs.
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector < Filter > Filters;
  typedef std::string FileName;
  typedef std::vector < FileName > FileNames;
  typedef std::pair < FileName, Filter > FileResult;
  typedef std::pair < FileNames, Filter > FilesResult;
  
  // Get the name of the file to load from
  static FileResult    getLoadFileName  ( QWidget* parent, const std::string &title = "Load", const Filters &filters = Filters() );
  static FilesResult   getLoadFileNames ( QWidget* parent, const std::string &title = "Load", const Filters &filters = Filters() );
  
  // Get the name of the file to save to
  static FileResult    getSaveFileName  ( QWidget* parent, const std::string &title = "Save", const Filters &filters = Filters() );
  
private:
  static std::string   _formatFilters ( const Filters &filters );
  
  static std::string   _lastFileDialogDir ( const std::string &title );
  static void          _lastFileDialogDir ( const std::string &title, const std::string &dir );
  static std::string   _lastFileDialogFilter ( const std::string &title );
  static void          _lastFileDialogFilter ( const std::string &title, const std::string &filter );
  
  FileDialog();
  ~FileDialog();
  
};
  
}

#endif // __QT_TOOLS_FILE_DIALOG_H__
