
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting a file name to save to
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SAVE_FILE_DIALOG_H_
#define _USUL_INTERFACE_SAVE_FILE_DIALOG_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>


namespace Usul {
namespace Interfaces {


struct ISaveFileDialog : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector<Filter> Filters;
  typedef std::string Filename;
  typedef std::pair<Filename,Filter> FileResult;
  typedef std::vector<Filename> Filenames;
  typedef std::pair<Filenames,Filter> FilesResult;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISaveFileDialog );

  // Id for this interface.
  enum { IID = 1102623452u };

  // Get the name of the file to save to
  virtual FileResult    getSaveFileName  ( const std::string &title = "Save", const Filters &filters = Filters() ) = 0;
  virtual FilesResult   getSaveFileNames ( const std::string &title = "Save", const Filters &filters = Filters() ) = 0;

};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SAVE_FILE_DIALOG_H_