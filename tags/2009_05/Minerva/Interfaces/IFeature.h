
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IFEATURE_H__
#define __MINERVA_INTERFACES_IFEATURE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva { namespace Core { namespace Data { class Feature; } } }


namespace Minerva {
namespace Interfaces {

struct IFeature : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFeature );
  
  /// Id for this interface.
  enum { IID = 3210853490u };
  
  /// Get as a feature.
  virtual Minerva::Core::Data::Feature*   feature() = 0;
  
}; // struct IFeature

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IFEATURE_H__ 
