
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IVECTOR_LAYER_H__
#define __MINERVA_INTERFACES_IVECTOR_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Minerva {
namespace Interfaces {


struct IVectorLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVectorLayer );

  /// Id for this interface.
  enum { IID = 2501255927u };

  /// Build the data objects.
  virtual void                buildVectorData  ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 ) = 0;

  /// Modify data objects
  virtual void                modifyVectorData ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 ) = 0;

}; // struct IVectorLayer


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_IVECTOR_LAYER_H__ */

