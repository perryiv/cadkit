
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_NODE_H_
#define _SAL_INTERFACE_NODE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace SAL {
namespace Interfaces {


struct INode : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INode );

  /// Id for this interface.
  enum { IID = 2506418282u };

  // Clone the node.
  virtual INode *     cloneNode() = 0;

  /// Get/set the name.
  virtual void        getNodeName ( std::string &name ) const = 0;
  virtual void        setNodeName ( const std::string &name ) = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_NODE_H_
