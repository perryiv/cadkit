
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IDATABASE_CONNECTION_H__
#define __MINERVA_INTERFACES_IDATABASE_CONNECTION_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Minerva {
namespace Interfaces {


struct IDatabaseConnection : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDatabaseConnection );

  /// Id for this interface.
  enum { IID = 3540370007u };

  /// Get/Set the hostname.
  virtual void                         hostname ( const std::string& ) = 0;
  virtual const std::string&           hostname () const = 0;

  /// Get/Set the database name.
  virtual void                         database ( const std::string& ) = 0;
  virtual const std::string&           database () const = 0;

  /// Get/Set the username
  virtual void                         username ( const std::string& ) = 0;
  virtual const std::string&           username () const = 0;

  /// Get/Set the password.
  virtual void                         password ( const std::string& ) = 0;
  virtual const std::string&           password () const = 0;

  /// Connect.
  virtual void                         connect () = 0;
  
  /// Disconnect.
  virtual void                         disconnect () = 0;

}; // struct IDatabaseConnection


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_IDATABASE_CONNECTION_H__ */
