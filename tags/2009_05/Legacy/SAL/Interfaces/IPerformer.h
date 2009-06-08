
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for direct access to Performer base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_PEFORMER_H_
#define _SAL_INTERFACE_PEFORMER_H_

#include "Usul/Interfaces/IUnknown.h"

class pfMemory;


namespace SAL {
namespace Interfaces {


struct IPerformer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPerformer );

  /// Id for this interface.
  enum { IID = 3405106785 };

  /// Get the pointer to the base class for all Performer objects.
  virtual const ::pfMemory *            getPfMemory() const = 0;
  virtual ::pfMemory *                  getPfMemory() = 0;
};


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_PEFORMER_H_
