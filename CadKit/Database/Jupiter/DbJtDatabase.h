
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
//  DbJtDatabase: Jupiter database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_

#include "DbJtApi.h"

#include "Interfaces/IDataSource.h"
#include "Interfaces/IController.h"
#include "Interfaces/ILoadOptions.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlRefPtr.h"
#include "Standard/SlBitmask.h"

#include <vector>


namespace CadKit
{
class DB_JT_API DbJtDatabase : public SlRefBase, 
                               public IDataSource,
                               public IController,
                               public ILoadOptions
{
public:

  // Constructor. If you don't pass in a customer id then it will look for 
  // the environment variable "DMDTK_CUSTOMER_ID".
  DbJtDatabase ( const unsigned int &customerId = 0 );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnknown interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the reference count.
  virtual unsigned long   getRefCount() const;

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( const unsigned long &iid );

  // Reference/unreference this instance.
  virtual void            ref();
  virtual void            unref();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataSource interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Load the data.
  virtual bool            loadData ( const char *filename );

  // Set the data target.
  virtual void            setDataTarget ( IUnknown *target );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IController interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the controller.
  virtual void            setController ( IUnknown *controller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ILoadOptions interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the assembly load option.
  virtual void            setAssemblyLoadOption ( const AssemblyLoadOption &option );

  // Set the brep load option.
  virtual void            setBrepLoadOption ( const BrepLoadOption &option );

  // Set the shape load option.
  virtual void            setShapeLoadOption ( const ShapeLoadOption &option );

protected:

  typedef std::vector<SlRefPtr<IUnknown> > Clients;

  unsigned int _flags;
  std::auto_ptr<Clients> _clients;
  SlRefPtr<IUnknown> _target;
  SlRefPtr<IUnknown> _controller;
  unsigned int _customerId;
  bool _initialized;
  AssemblyLoadOption _assemblyLoadOption;
  BrepLoadOption _brepLoadOption;
  ShapeLoadOption _shapeLoadOption;

  virtual ~DbJtDatabase();

  const unsigned int &    _getCustomerId();

  bool                    _init();

  bool                    _notifyError    ( const std::string &message, const unsigned long &id );
  bool                    _notifyProgress ( const std::string &message );
  bool                    _notifyWarning  ( const std::string &message, const unsigned long &id );

  bool                    _traverse ( const char *filename );
  static int              _preActionTraversalCallback ( eaiHierarchy *node, int level );
  static int              _postActionTraversalCallback ( eaiHierarchy *node, int level );
  bool                    _preActionTraversalNotify ( eaiHierarchy *node, int level );
  bool                    _postActionTraversalNotify ( eaiHierarchy *node, int level );

  SL_DECLARE_REFCOUNT_TYPE ( DbJtDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbJtDatabase, 1032628516 );
  SL_DECLARE_BITMASK_FUNCTIONS ( Flags, unsigned int, _flags );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
