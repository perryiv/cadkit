
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
#include "Interfaces/IControlled.h"
#include "Interfaces/ILoadOptions.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlRefPtr.h"
#include "Standard/SlBitmask.h"
#include "Standard/SlStack.h"

#include <vector>

class eaiAssembly;
class eaiHierarchy;
class eaiInstance;
class eaiPart;


namespace CadKit
{
class DB_JT_API DbJtDatabase : public SlRefBase, 
                               public IDataSource,
                               public IControlled,
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
  virtual bool            loadData ( const std::string &filename );

  // Set the data target.
  virtual void            setDataTarget ( IUnknown *target );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IControlled interface.
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

  typedef SlStack<eaiAssembly *> Assemblies;

  SlRefPtr<IUnknown> _target;
  SlRefPtr<IUnknown> _controller;
  unsigned int _customerId;
  bool _initialized;
  AssemblyLoadOption _assemblyLoadOption;
  BrepLoadOption _brepLoadOption;
  ShapeLoadOption _shapeLoadOption;
  std::auto_ptr<Assemblies> _assemblies;
  eaiPart *_currentPart;
  eaiInstance *_currentInstance;

  virtual ~DbJtDatabase();

  bool                    _endAssembly ( const unsigned int &level, eaiAssembly *assembly );
  bool                    _endInstance ( const unsigned int &level, eaiInstance *instance );
  bool                    _endPart     ( const unsigned int &level, eaiPart *part );

  const eaiAssembly *     _getCurrentAssembly() const;
  eaiAssembly *           _getCurrentAssembly();
  const unsigned int &    _getCustomerId();

  bool                    _init();

  bool                    _notifyError    ( const std::string &message, const unsigned long &id );
  bool                    _notifyProgress ( const std::string &message );
  bool                    _notifyWarning  ( const std::string &message, const unsigned long &id );

  static int              _preActionTraversalCallback ( eaiHierarchy *hierarchy, int level );
  static int              _postActionTraversalCallback ( eaiHierarchy *hierarchy, int level );
  bool                    _preActionTraversalNotify ( eaiHierarchy *hierarchy, int level );
  bool                    _postActionTraversalNotify ( eaiHierarchy *hierarchy, int level );

  void                    _pushAssembly ( eaiAssembly *assembly );
  void                    _popAssembly();

  void                    _setCurrentPart ( eaiPart *part );
  void                    _setCurrentInstance ( eaiInstance *instance );

  bool                    _startAssembly ( const unsigned int &level, eaiAssembly *assembly );
  bool                    _startInstance ( const unsigned int &level, eaiInstance *instance );
  bool                    _startPart     ( const unsigned int &level, eaiPart *part );

  bool                    _traverse ( const std::string &filename );

  SL_DECLARE_REFCOUNT_TYPE ( DbJtDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbJtDatabase, 1032628516 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
