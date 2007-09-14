
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ILAYER_LIST_H__
#define __MINERVA_INTERFACES_ILAYER_LIST_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Interfaces { struct ILayer; } }

namespace Minerva {
namespace Interfaces {

struct ILayerList : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILayerList );

  /// Id for this interface.
  enum { IID = 3025074776u };

  /// Get the number of layers.
  virtual unsigned int                  numberLayers () const = 0;

  /// Get the layer at position i.
  virtual Usul::Interfaces::ILayer*     layer ( unsigned int i ) = 0;

}; // struct ILayerList

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_ILAYER_LIST_H__ 
