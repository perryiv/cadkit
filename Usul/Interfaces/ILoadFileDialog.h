
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


namespace Usul {
namespace Interfaces {


struct ILoadFileDialog : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILoadFileDialog );

  // Id for this interface.
  enum { IID = 1103131693u };

  // Get the name of the file to load from
  virtual std::string   getLoadFileName() = 0;

};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_LOAD_FILE_DIALOG_H_

