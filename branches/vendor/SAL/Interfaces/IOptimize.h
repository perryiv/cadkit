
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for optimizing a scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_OPTIMIZE_H_
#define _SAL_INTERFACE_OPTIMIZE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace SAL {
namespace Interfaces {


struct IOptimize : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOptimize );

  /// Id for this interface.
  enum { IID = 1642133382 };

  /// Remove redundant groups.
  virtual void removeRedundantGroups ( Unknown *unknown, bool traverse );
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_OPTIMIZE_H_
