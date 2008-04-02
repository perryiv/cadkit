
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting a file name to load from
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_LOAD_FILE_DIALOG_H_
#define _USUL_INTERFACE_LOAD_FILE_DIALOG_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>


namespace Usul {
namespace Interfaces {


struct ILoadFileDialog : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector < Filter > Filters;
  typedef std::string FileName;
  typedef std::vector < FileName > FileNames;
  typedef std::pair < FileName, Filter > FileResult;
  typedef std::pair < FileNames, Filter > FilesResult;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILoadFileDialog );

  // Id for this interface.
  enum { IID = 1103131693u };

  // Get the name of the file to load from
  virtual FileResult    getLoadFileName  ( const std::string &title = "Load", const Filters &filters = Filters() ) = 0;
  virtual FilesResult   getLoadFileNames ( const std::string &title = "Load", const Filters &filters = Filters() ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_LOAD_FILE_DIALOG_H_

