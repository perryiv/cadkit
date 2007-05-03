
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDATA_OBJECTS_H__
#define __USUL_INTERFACES_IDATA_OBJECTS_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva { namespace Core { namespace DataObjects { class DataObject; } } }

namespace Usul {
namespace Interfaces {


struct IDataObjects : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDataObjects );

  /// Id for this interface.
  enum { IID = 2184274320u };

  /// Get the number of data objects.
  virtual unsigned int                                     numberDataObjects() const = 0;

  /// Get the data object.
  virtual Minerva::Core::DataObjects::DataObject *         dataObject( unsigned int i ) = 0;
  virtual const Minerva::Core::DataObjects::DataObject *   dataObject( unsigned int i ) const = 0;

}; // struct IDataObjects


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IDATA_OBJECTS_H__ */

