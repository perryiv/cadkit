
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for writing a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_WRITE_H_
#define _SAL_INTERFACE_WRITE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace SAL {
namespace Interfaces {

struct INode;

struct IWrite : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWrite );

  /// Id for this interface.
  enum { IID = 1177035122 };

  /// Write the scene to file.
  virtual void    writeNodeFile ( const std::string &filename, INode *node ) const = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_WRITE_H_
