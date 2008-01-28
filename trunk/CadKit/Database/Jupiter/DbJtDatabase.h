
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
#include "Database/Base/DbBasePrimitiveData.h"

#include "Standard/SlRefPtr.h"
#include "Standard/SlBitmask.h"
#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlMatrixStack.h"
#include "Standard/SlMatrix44.h"

#include "Interfaces/ILoadOptions.h"
#include "Interfaces/IEntityQuery.h"
#include "Interfaces/IShapeQuery.h"
#include "Interfaces/IPrimQuery.h"
#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryColors.h"
#include "Interfaces/IQueryTexCoords.h"
#include "Interfaces/IMessagePriority.h"
#include "Interfaces/ILodOption.h"
#include "Interfaces/IFileExtension.h"
#include "Interfaces/IDataRead.h"
#include "Interfaces/IUnits.h"
#include "Interfaces/IQueryPrimOrigins.h"
#include "Interfaces/IQueryPrimParams.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
#endif

#ifdef _CADKIT_USE_JTOPEN
class JtkAssembly;
class JtkHierarchy;
class JtkInstance;
class JtkPart;
class JtkShape;
class JtkPrim;
class JtkClientData;
#define eaiAssembly JtkAssembly
#define eaiPart JtkPart
#define eaiInstance JtkInstance
#define eaiShape JtkShape
#define eaiPrim JtkPrim
#define eaiHierarchy JtkHierarchy
#define eaiClientData JtkClientData
#else // _CADKIT_USE_JTOPEN
class eaiAssembly;
class eaiHierarchy;
class eaiInstance;
class eaiPart;
class eaiShape;
class eaiPrim;
class eaiClientData;
#endif // _CADKIT_USE_JTOPEN


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
                               public IPrimQueryFloat,
                               public ISetQuery,
                               public IQueryShapeVerticesVec3f,
                               public IQueryShapeNormalsVec3f,
                               public IQueryShapeColorsVec4f,
                               public IQueryShapeColorsVec3f,
                               public IQueryShapeTexCoordsVec2f,
                               public IMessagePriority,
                               public ILodOption,
                               public IFileExtension,
                               public IDataRead,
                               public IUnits,
                               public IQueryPrimOriginsVec3f,
                               public IQueryPrimParamsFloat,
                               public IQueryPrimColorsVec4f,
                               public IQueryPrimColorsVec3f
{

public:

  // Constructor. If you don't pass in a customer id then it will look for 
  // the environment variable "DMDTK_CUSTOMER_ID".
  DbJtDatabase ( const unsigned int &customerId = 0 );

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IFormatAttribute interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Does the format have the attribute?
  virtual bool            isAttributeSupported ( const FormatAttribute &attribute ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnits interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the linear units for this model.
  virtual std::string     getUnits ( ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ILoadOptions interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the assembly load option. Returns false if not supported.
  virtual bool            setAssemblyLoadOption ( const AssemblyLoadOption &option );

  // Set the brep load option. Returns false if not supported.
  virtual bool            setBrepLoadOption ( const BrepLoadOption &option );

  // Set the part load option. Returns false if not supported.
  virtual bool            setPartLoadOption ( const PartLoadOption &option );

  // Set the shape load option. Returns false if not supported.
  virtual bool            setShapeLoadOption ( const ShapeLoadOption &option );

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
  virtual bool            getTransform ( AssemblyHandle assembly, SlMatrix44f &matrix, bool tryParents ) const;

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
  virtual bool            getTransform ( PartHandle part, SlMatrix44f &matrix, bool tryParents ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IInstanceQueryFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the assembly that the given instance represents, or null.
  virtual AssemblyHandle  getCorrespondingAssembly ( InstanceHandle instance ) const;

  // Get the part that the given instance represents, or null.
  virtual PartHandle      getCorrespondingPart ( InstanceHandle instance ) const;

  // Get the name.
  virtual std::string     getName ( InstanceHandle instance ) const;

  // Get the material.
  virtual bool            getMaterial ( InstanceHandle instance, SlMaterialf &material, bool tryParents ) const;

  // Get the parent of the entity.
  virtual AssemblyHandle  getParent ( InstanceHandle instance ) const;

  // Get the transformation matrix.
  virtual bool            getTransform ( InstanceHandle instance, SlMatrix44f &matrix, bool tryParents ) const;

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
  //  IPrimQueryFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the material.
  virtual bool            getMaterial ( PrimHandle prim, SlMaterialf &material, bool tryParents ) const;

  // Get the parent of the entity.
  virtual LodHandle       getParent ( PrimHandle prim ) const;

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
  virtual bool            getVertexSetType ( ShapeHandle shape, VertexSetType &type ) const;

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
  //  IQueryPrimColorsVec3f and IQueryPrimColorsVec4f interfaces.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the color binding.
  virtual bool            getColorBinding ( PrimHandle prim, VertexBinding &binding ) const;

  // Get the normals.
  virtual bool            getColors ( PrimHandle prim, IQueryPrimColorsVec3f::ColorSetter &setter ) const;

  // Get the normals.
  virtual bool            getColors ( PrimHandle prim, IQueryPrimColorsVec4f::ColorSetter &setter ) const;

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
  virtual bool            setMessagePriorityLevel ( const MessageType &type, unsigned int priority );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ILodOption interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set option for processing the LODs.
  virtual void            setLodProcessOption ( const LodProcessOption &option );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IFileExtension interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the file extension.
  virtual std::string     getFileExtension() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataRead interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Read the data.
  virtual bool            readData ( const std::string &filename );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IQueryPrimOriginsVec3f interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the primitive type.
  virtual bool            getPrimType ( PrimHandle prim, PrimitiveType &type ) const;

  // Get the origin(s).
  virtual bool            getPrimOrigins ( PrimHandle prim, IQueryPrimOriginsVec3f::OriginSetter &setter ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IQueryPrimParamsFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the parameters.
  virtual bool            getPrimParams ( PrimHandle prim, IQueryPrimParamsFloat::ParamSetter &setter ) const;

protected:

  typedef std::list<eaiAssembly *> Assemblies;
  typedef DbBaseShapeData<SlVec3f,SlVec3f,SlVec3f,SlVec2f,ShapeHandle> ShapeData;
  typedef DbBasePrimitiveData<SlVec3f,SlVec3f,float,PrimHandle> PrimData;

  bool _initialized;
  AssemblyLoadOption _assemblyLoadOption;
  PartLoadOption _partLoadOption;
  BrepLoadOption _brepLoadOption;
  ShapeLoadOption _shapeLoadOption;
  std::auto_ptr<Assemblies> _assemblies;
  std::auto_ptr<DbJtTraversalState> _current;
  std::auto_ptr<ShapeData> _shapeData;
  std::auto_ptr<PrimData>  _primData;
  unsigned int _progressPriorityLevel;
  bool _result;
  LodProcessOption _lodOption;
  SlMatrixStack<SlMatrix44f> _matrixStack;
  std::string _units;

  virtual ~DbJtDatabase();

  bool                    _appendTriangles ( ShapeHandle shape, int whichSet );

  bool                    _endAssembly ( eaiAssembly *assembly );
  bool                    _endInstance ( eaiInstance *instance );
  bool                    _endPart     ( eaiPart *part );

  eaiAssembly *           _getCurrentAssembly() const;
  eaiInstance *           _getCurrentInstance() const;
  eaiPart *               _getCurrentPart() const;
  eaiShape *              _getShape ( ShapeHandle shape ) const;
  eaiShape *              _getShape ( eaiPart *part, int whichLod, int whichShape ) const;
  eaiPrim *               _getPrim ( PrimHandle shape ) const;
  eaiPrim *               _getPrim  ( eaiPart *part, int whichLod, int whichPrim ) const;
  eaiAssembly *           _getTopAssembly() const;

  static int              _preActionTraversalCallback  ( eaiHierarchy *hierarchy, int level, eaiClientData *data ); // DMDTk >= 5.2
  static int              _postActionTraversalCallback ( eaiHierarchy *hierarchy, int level, eaiClientData *data ); // DMDTk >= 5.2
  static int              _preActionTraversalCallback  ( eaiHierarchy *hierarchy, int level ); // DMDTk < 5.2
  static int              _postActionTraversalCallback ( eaiHierarchy *hierarchy, int level ); // DMDTk < 5.2
  bool                    _preActionTraversalNotify    ( eaiHierarchy *hierarchy, int level );
  bool                    _postActionTraversalNotify   ( eaiHierarchy *hierarchy, int level );

  bool                    _processLods  ( eaiPart *part );
  bool                    _processShapeLod   ( eaiPart *part,   int whichLod );
  bool                    _processPrimLod   ( eaiPart *part,   int whichLod );
  bool                    _processShape ( eaiPart *part,   int whichLod, int whichShape );
  bool                    _processPrim ( eaiPart *part,   int whichLod, int whichPrim );
  bool                    _processSet   ( eaiShape *shape, int whichShape, int whichSet );
  bool                    _processSet   ( eaiPrim *prim, int whichPrim, int whichSet );

  void                    _pushAssembly ( eaiAssembly *assembly );
  void                    _popAssembly();

  void                    _pushMatrix ( eaiHierarchy *hierarchy );
  void                    _popMatrix();

  void                    _replaceProperties ( eaiInstance *instance, eaiPart *part );
  void                    _resetStateIndices();

  void                    _setCurrentPart     ( eaiPart *part );
  void                    _setCurrentInstance ( eaiInstance *instance );
  bool                    _setShapeData ( ShapeHandle shapeHandle );
  bool                    _setShapeData ( eaiShape *shape );
  bool                    _setPrimData ( PrimHandle primHandle );
  bool                    _setPrimData ( eaiPrim *prim );


  bool                    _startAssembly ( eaiAssembly *assembly );
  bool                    _startInstance ( eaiInstance *instance );
  bool                    _startPart     ( eaiPart *part );

  bool                    _traverse ( const std::string &filename );

  SL_DECLARE_REFERENCE_POINTER ( DbJtDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbJtDatabase, 1032628516 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_DATABASE_H_
