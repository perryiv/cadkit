
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISUBDIVIDEMODEL_H__
#define __USUL_INTERFACES_ISUBDIVIDEMODEL_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISubdivideModel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISubdivideModel );

  /// Id for this interface.
  enum { IID = 1136402875u };

  virtual void subdivideModel ( unsigned int numSubdivisions ) = 0;

}; // struct ISubdivideModel


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISUBDIVIDEMODEL_H__ */
