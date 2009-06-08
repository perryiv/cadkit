
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_INTERFACES_CONTAINER_H_
#define _MINERVA_INTERFACES_CONTAINER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva { namespace Core { namespace Data { class Container; } } }


namespace Minerva {
namespace Interfaces {


struct IContainer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IContainer );
  
  /// Id for this interface.
  enum { IID = 2532462292u };
  
  /// Get the container.
  virtual Minerva::Core::Data::Container *      container() = 0;
};


} // end namespace Interfaces
} // end namespace Minerva


#endif // _MINERVA_INTERFACES_CONTAINER_H_
