
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IGeneralShader_H__
#define __USUL_INTERFACES_IGeneralShader_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct IGeneralShader : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGeneralShader );

  /// Id for this interface.
  enum { IID = 3744994418u };

  // Change the currently displayed model
  virtual void                setModel( const std::string &name ) = 0;
  virtual std::string         getCurrentModel() = 0;

  // Change the currently used shader
  virtual void                setShader( const std::string &name ) = 0;
  virtual std::string         getCurrentShader() = 0;

  // Read settings file
  virtual void                readSettings( const std::string &workingDir ) = 0;

  
}; //struct IGeneralShader



}
}

#endif // __USUL_INTERFACES_IGeneralShader_H__
