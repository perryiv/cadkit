
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the number of primitives in the model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDECIMATEMODEL_H__
#define __USUL_INTERFACES_IDECIMATEMODEL_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IDecimateModel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDecimateModel );

  /// Id for this interface.
  enum { IID = 2172000000u };

  virtual void decimateModel () = 0;

}; // struct IDecimateModel


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IDECIMATEMODEL_H__ */
