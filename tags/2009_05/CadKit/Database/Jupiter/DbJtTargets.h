
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtTargets: Proxy class. This class serves as the implementation of
//  all known "terget" interfaces and simply tries to delegate to all of 
//  its clients.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_

#include "DbJtApi.h"

#include "Interfaces/IInterfaceGroup.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlRefPtr.h"
#include "Standard/SlBitmask.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
# include <memory>
#endif

namespace CadKit
{
class DB_JT_API DbJtTargets : public SlRefBase, 
                              public IInterfaceGroup
{
public:

  DbJtTargets();

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IInterfaceGroup interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Add the interface.
  virtual void            addInterface ( IUnknown *unknown );

  // Get the number of unknowns.
  virtual unsigned int    getNumInterfaces() const;

  // Remove the interface. Returns the number of occurances removed.
  virtual unsigned int    removeInterface ( IUnknown *unknown );

  // Remove all interfaces.
  virtual void            removeAllInterfaces();

protected:

  typedef std::vector<SlRefPtr<IUnknown> > Interfaces;
  std::auto_ptr<Interfaces> _interfaces;

  virtual ~DbJtTargets();

  SL_DECLARE_REFERENCE_POINTER ( DbJtTargets );
  SL_DECLARE_DYNAMIC_CLASS ( DbJtTargets, 1032637889 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
