
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
# include <map>
#endif

namespace osg 
{
  class Group;
  class Geometry;
  class StateSet;
};


namespace CadKit
{
class DB_OSG_API DbOsgDatabase : public DbBaseTarget,
                                 public IAssemblyNotify,
                                 public IPartNotify,
                                 public IInstanceNotify,
                                 public ILodNotify,
                                 public IShapeNotify,
                                 public ISetNotify
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

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ILodNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the lod.
  virtual bool            endEntity ( LodHandle lod, IUnknown *caller );

  // Start the lod.
  virtual bool            startEntity ( LodHandle lod, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IShapeNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the shape.
  virtual bool            endEntity ( ShapeHandle shape, IUnknown *caller );

  // Start the shape.
  virtual bool            startEntity ( ShapeHandle shape, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ISetNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the set.
  virtual bool            endEntity ( SetHandle set, IUnknown *caller );

  // Start the set.
  virtual bool            startEntity ( SetHandle set, IUnknown *caller );

protected:

  typedef SlStack<osg::Group *> GroupStack;
  typedef std::map<const void *, osg::Group *> GroupMap;

  osg::Group *_root;
  std::auto_ptr<GroupStack> _groupStack;
  std::auto_ptr<GroupMap> _groupMap;

  virtual ~DbOsgDatabase();

  bool                    _addAttributes ( IUnknown *caller, ShapeHandle shape, osg::StateSet *state );
  bool                    _addDataSets   ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );
  bool                    _addVertices   ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );
  bool                    _addNormals    ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );
  bool                    _addColors     ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );
  bool                    _addTexCoords  ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );

  osg::Group *            _findGroup ( const void *key ) const;

  bool                    _hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const;

  void                    _pushGroup ( osg::Group *group );
  void                    _popGroup();

  SL_DECLARE_REFERENCE_POINTER ( DbOsgDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbOsgDatabase, 1032736074 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_DATABASE_H_
