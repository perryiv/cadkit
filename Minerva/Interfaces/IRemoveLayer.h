
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IREMOVE_LAYER_H__
#define __MINERVA_INTERFACES_IREMOVE_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {

struct IRemoveLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRemoveLayer );

  /// Id for this interface.
  enum { IID = 3509478175u };

  /// Remove a layer.
  virtual void                             removeLayer ( Usul::Interfaces::IUnknown * layer ) = 0;

}; // struct IAnimationControl

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IREMOVE_LAYER_H__ 
