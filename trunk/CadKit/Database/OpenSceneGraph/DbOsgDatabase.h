
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

#include "Interfaces/IEntityNotify.h"

#include "Standard/SlStack.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif

namespace osg { class Group; };


namespace CadKit
{
class DB_OSG_API DbOsgDatabase : public DbBaseTarget,
                                 public IAssemblyNotify,
                                 public IPartNotify,
                                 public IInstanceNotify
{
public:

  DbOsgDatabase();

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

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
  virtual bool            endEntity ( AssemblyHandle assembly, IUnknown *caller );

  // Start the assembly.
  virtual bool            startEntity ( AssemblyHandle assembly, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IPartNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the part.
  virtual bool            endEntity ( PartHandle assembly, IUnknown *caller );

  // Start the part.
  virtual bool            startEntity ( PartHandle assembly, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IInstanceNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the instance.
  virtual bool            endEntity ( InstanceHandle assembly, IUnknown *caller );

  // Start the instance.
  virtual bool            startEntity ( InstanceHandle assembly, IUnknown *caller );

protected:

  typedef SlStack<osg::Group *> GroupStack;

  osg::Group *_root;
  std::auto_ptr<GroupStack> _groups;

  virtual ~DbOsgDatabase();

  void                    _pushGroup ( osg::Group *group );
  void                    _popGroup();

  SL_DECLARE_REFERENCE_POINTER ( DbOsgDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbOsgDatabase, 1032736074 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_DATABASE_H_
