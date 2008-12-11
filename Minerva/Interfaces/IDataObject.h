
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IDATA_OBJECT_H__
#define __MINERVA_INTERFACES_IDATA_OBJECT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva { namespace Core { namespace Data { class DataObject; } } }


namespace Minerva {
namespace Interfaces {
    
struct IDataObject : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDataObject );
  
  /// Id for this interface.
  enum { IID = 2511972422u };
  
  /// Get as a DataObect.
  virtual Minerva::Core::Data::DataObject*   dataObject() = 0;
  
}; // struct IDataObject

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IDATA_OBJECT_H__ 
