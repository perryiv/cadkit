
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for parsing the restart file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_PARSE_RESTART_FILE_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_PARSE_RESTART_FILE_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"

#include <string>


namespace VRV {
namespace Interfaces {


struct IParseRestart : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IParseRestart );

  // Id for this interface.
  enum { IID = 2556785532u };

  // Parse the restart-file. The caller should support IRequestRead.
  virtual void parseRestartFile ( const std::string &filename, Unknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_PARSE_RESTART_FILE_H_
