
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the document
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_H_
#define _USUL_INTERFACE_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <utility>

namespace Usul {
namespace Interfaces {


struct IDocument : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef std::pair<std::string,bool> Filename;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocument );

  /// Id for this interface.
  enum { IID = 3762815634u };

  virtual void                         modified ( bool m ) = 0;
  virtual bool                         modified() const = 0;

  virtual const Filename &             filename() const = 0;
  virtual void                         filename ( const std::string &f, bool valid = true ) = 0;
  
}; // class IDocument


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_H_
