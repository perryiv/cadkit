
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlDatabase: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_DATABASE_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_DATABASE_H_

#include "DbStlApi.h"

#include "Database/Base/DbBaseTarget.h"

#include "Interfaces/IEntityNotify.h"
#include "Interfaces/IInstanceQuery.h"

#include "Standard/SlStack.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
# include <map>
#endif

namespace osg 
{
  class Group;
  class LOD;
  class Geometry;
  class StateSet;
};


namespace CadKit
{
class DB_STL_API DbStlDatabase : public DbBaseTarget,
                                 public IAssemblyNotify,
                                 public IPartNotify,
                                 public IInstanceNotify,
                                 public ILodNotify,
                                 public IShapeNotify,
                                 public ISetNotify
{
public:

  DbStlDatabase();

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataTarget interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Tell the target it is about to receive data.
  virtual bool            dataTransferStart ( IUnknown *caller );

  // Tell the target it is done receiving data.
  virtual bool            dataTransferEnd ( IUnknown *caller );

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

//  typedef SlStack<osg::Group *> GroupStack;
//  std::auto_ptr<GroupStack> _groupStack;

  virtual ~DbStlDatabase();

  bool                    _addDataSets   ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );
  bool                    _addVertices   ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );
  bool                    _addNormals    ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry );

  void                    _pushGroup ( osg::Group *group );
  void                    _popGroup();

  void                    _setLodParameters ( osg::LOD *lod ) const;

  SL_DECLARE_REFERENCE_POINTER ( DbStlDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbStlDatabase, 1034649260 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_DATABASE_H_
