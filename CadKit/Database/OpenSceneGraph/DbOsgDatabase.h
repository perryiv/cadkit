
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgDatabase: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_DATABASE_H_
#define _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_DATABASE_H_

#include "DbOsgApi.h"

#include "Database/Base/DbBaseTarget.h"

#include "Interfaces/IAssemblyNotify.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class DB_OSG_API DbOsgDatabase : public DbBaseTarget,
                                 public IAssemblyNotify
{
public:

  DbOsgDatabase();

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
  //  IDataTarget interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the file extension.
  virtual std::string     getFileExtension() const;

  // Store the data.
  virtual bool            storeData ( const std::string &filename );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IAssemblyNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the assembly.
  virtual bool            endAssembly ( IUnknown *caller );

  // Start the assembly.
  virtual bool            startAssembly ( IUnknown *caller );

protected:

  virtual ~DbOsgDatabase();

  SL_DECLARE_REFCOUNT_TYPE ( DbOsgDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbOsgDatabase, 1032736074 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_DATABASE_H_
