
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

#include "Database/Base/DbBaseSource.h"
#include "Database/Base/DbBaseShapeData.h"

#include "Standard/SlRefPtr.h"
#include "Standard/SlBitmask.h"
#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"

#include "Interfaces/ILoadOptions.h"
#include "Interfaces/IEntityQuery.h"
#include "Interfaces/IShapeQuery.h"
#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryColors.h"
#include "Interfaces/IQueryTexCoords.h"
#include "Interfaces/IMessagePriority.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
#endif

class eaiAssembly;
class eaiHierarchy;
class eaiInstance;
class eaiPart;
class eaiShape;


namespace CadKit
{
class DbJtTraversalState;

class DB_JT_API DbJtDatabase : public DbBaseSource, 
                               public ILoadOptions,
                               public IAssemblyQueryFloat,
                               public IPartQueryFloat,
                               public IInstanceQueryFloat,
                               public ILodQuery,
                               public IShapeQueryFloatUchar,
                               public ISetQuery,
                               public IQueryShapeVerticesVec3f,
                               public IQueryShapeNormalsVec3f,
                               public IQueryShapeColorsVec4f,
                               public IQueryShapeColorsVec3f,
                               public IQueryShapeTexCoordsVec2f,
                               public IMessagePriority
{
public:

  // Constructor. If you don't pass in a customer id then it will look for 
  // the environment variable "DMDTK_CUSTOMER_ID".
  DbJtDatabase ( const unsigned int &customerId = 0 );

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataSource interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Load the data.
  virtual bool            loadData ( const std::string &filename );

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

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IAssemblyQueryFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the name.
  virtual std::string     getName ( AssemblyHandle assembly ) const;

  // Get the material.
  virtual bool            getMaterial ( AssemblyHandle assembly, SlMaterialf &material, bool tryParents ) const;

  // Get the parent of the entity.
  virtual AssemblyHandle  getParent ( AssemblyHandle assembly ) const;

  // Get the transformation matrix.
  virtual bool            getTransform ( AssemblyHandle assembly, SlMatrix4f &matrix, bool tryParents ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IPartQueryFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the name.
  virtual std::string     getName ( PartHandle part ) const;

  // Get the material.
  virtual bool            getMaterial ( PartHandle part, SlMaterialf &material, bool tryParents ) const;

  // Get the parent of the entity.
  virtual AssemblyHandle  getParent ( PartHandle part ) const;

  // Get the transformation matrix.
  virtual bool            getTransform ( PartHandle part, SlMatrix4f &matrix, bool tryParents ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IInstanceQueryFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the corresponding part.
  virtual HierarchyHandle getCorresponding ( InstanceHandle instance ) const;

  // Get the name.
  virtual std::string     getName ( InstanceHandle instance ) const;

  // Get the material.
  virtual bool            getMaterial ( InstanceHandle instance, SlMaterialf &material, bool tryParents ) const;

  // Get the parent of the entity.
  virtual AssemblyHandle  getParent ( InstanceHandle instance ) const;

  // Get the transformation matrix.
  virtual bool            getTransform ( InstanceHandle instance, SlMatrix4f &matrix, bool tryParents ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ILodQuery interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the parent of the entity.
  virtual PartHandle      getParent ( LodHandle lod ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IShapeQueryFloatUchar interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the material.
  virtual bool            getMaterial ( ShapeHandle shape, SlMaterialf &material, bool tryParents ) const;

  // Get the parent of the entity.
  virtual LodHandle       getParent ( ShapeHandle shape ) const;

  // Get the texture.
  virtual bool            getTexture ( ShapeHandle shape, std::vector<unsigned char> &texture, bool tryParents ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ISetQuery interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the parent of the entity.
  virtual ShapeHandle     getParent ( SetHandle set ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IQueryShapeVerticesVec3f interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the set type.
  virtual bool            getVertexSetType ( ShapeHandle shape, IQueryShapeVerticesVec3f::Type &type ) const;

  // Get the vertices.
  virtual bool            getVertices ( ShapeHandle shape, IQueryShapeVerticesVec3f::VertexSetter &setter ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IQueryShapeNormalsVec3f interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the normal binding.
  virtual bool            getNormalBinding ( ShapeHandle shape, VertexBinding &binding ) const;

  // Get the normals.
  virtual bool            getNormals ( ShapeHandle shape, IQueryShapeNormalsVec3f::NormalSetter &setter ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IQueryShapeColorsVec3f and IQueryShapeColorsVec4f interfaces.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the color binding.
  virtual bool            getColorBinding ( ShapeHandle shape, VertexBinding &binding ) const;

  // Get the normals.
  virtual bool            getColors ( ShapeHandle shape, IQueryShapeColorsVec3f::ColorSetter &setter ) const;

  // Get the normals.
  virtual bool            getColors ( ShapeHandle shape, IQueryShapeColorsVec4f::ColorSetter &setter ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IQueryShapeTexCoordsVec2f interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the texture coordinates.
  virtual bool            getTextureCoords ( ShapeHandle shape, IQueryShapeTexCoordsVec2f::TexCoordSetter &setter ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IMessagePriority interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the message priority level.
  virtual bool            setMessagePriorityLevel ( const MessageType &type, const unsigned int &priority );

protected:

  typedef std::list<eaiAssembly *> Assemblies;
  typedef DbBaseShapeData<SlVec3f,SlVec3f,SlVec3f,SlVec2f,ShapeHandle> ShapeData;

  unsigned int _customerId;
  bool _initialized;
  AssemblyLoadOption _assemblyLoadOption;
  BrepLoadOption _brepLoadOption;
  ShapeLoadOption _shapeLoadOption;
  std::auto_ptr<Assemblies> _assemblies;
  std::auto_ptr<DbJtTraversalState> _current;
  std::auto_ptr<ShapeData> _shapeData;
  unsigned int _progressPriorityLevel;

  virtual ~DbJtDatabase();

  bool                    _endAssembly ( eaiAssembly *assembly );
  bool                    _endInstance ( eaiInstance *instance );
  bool                    _endPart     ( eaiPart *part );

  eaiAssembly *           _getCurrentAssembly() const;
  const unsigned int &    _getCustomerId();
  eaiShape *              _getShape ( ShapeHandle shape ) const;
  eaiShape *              _getShape ( eaiPart *part, const int &whichLod, const int &whichShape ) const;
  eaiAssembly *           _getTopAssembly() const;

  bool                    _init();

  static int              _preActionTraversalCallback  ( eaiHierarchy *hierarchy, int level );
  static int              _postActionTraversalCallback ( eaiHierarchy *hierarchy, int level );
  bool                    _preActionTraversalNotify    ( eaiHierarchy *hierarchy, int level );
  bool                    _postActionTraversalNotify   ( eaiHierarchy *hierarchy, int level );

  bool                    _processLods  ( eaiPart *part );
  bool                    _processLod   ( eaiPart *part,   const int &whichLod );
  bool                    _processShape ( eaiPart *part,   const int &whichLod, const int &whichShape );
  bool                    _processSet   ( eaiShape *shape, const int &whichSet );

  void                    _pushAssembly ( eaiAssembly *assembly );
  void                    _popAssembly();

  void                    _resetStateIndices();

  void                    _setCurrentPart     ( eaiPart *part );
  void                    _setCurrentInstance ( eaiInstance *instance );
  bool                    _setShapeData ( ShapeHandle shapeHandle );
  bool                    _setShapeData ( eaiShape *shape );

  bool                    _startAssembly ( eaiAssembly *assembly );
  bool                    _startInstance ( eaiInstance *instance );
  bool                    _startPart     ( eaiPart *part );

  bool                    _traverse ( const std::string &filename );

  SL_DECLARE_REFERENCE_POINTER ( DbJtDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbJtDatabase, 1032628516 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
