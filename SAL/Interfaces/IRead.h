
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_READ_H_
#define _SAL_INTERFACE_READ_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace SAL {
namespace Interfaces {

struct INode;

struct IRead : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRead );

  /// Id for this interface.
  enum { IID = 2584851569 };

  /// Read the scene to file.
  virtual INode * readNodeFile ( const std::string &filename ) const = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_READ_H_
