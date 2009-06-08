
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a group.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_GROUP_H_
#define _SAL_INTERFACE_GROUP_H_

#include "Usul/Interfaces/IUnknown.h"


namespace SAL {
namespace Interfaces {

struct INode;

struct IGroup : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGroup );

  /// Id for this interface.
  enum { IID = 1467355212 };

  /// Append the node to list of children.
  virtual void            addChild ( INode * ) = 0;

  /// Get the i'th child.
  virtual const INode *   childNode ( unsigned long i ) const = 0;
  virtual INode *         childNode ( unsigned long i ) = 0;

  /// Get the number of children.
  virtual unsigned long   numChildren() const = 0;

  /// Remove all children.
  virtual void            removeAllChildren() = 0;

  /// Remove all occurances of the node from the list of children.
  virtual void            removeAllOccurances ( INode * ) = 0;

  /// Remove the first occurance of the node from the list of children.
  virtual void            removeFirstOccurance ( INode * ) = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_GROUP_H_
