
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

/////////////////////////////////////////////////////////////////////////////
//
//  DbJtTraverser: Jupiter database parsing class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSER_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSER_H_

#include "DbJtApi.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlString.h"
#include "Standard/SlMatrix4.h"
#include "Standard/SlMaterial.h"
#include "Standard/SlVec4.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec2.h"
#include "Standard/SlBitmask.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif

class eaiHierarchy;
class eaiMaterial;
class eaiShape;


namespace CadKit
{
class DB_JT_API DbJtTraverser : public SlRefBase
{
public:

  // Possible flags.
  enum
  {
    VERBOSE = (1<<0L) // Print progress and errors to stdout. Default is off.
  };

  // Possible messages.
  enum Message
  {
    IMPORT_START,
    IMPORT_FINISH,
    TRAVERSAL_START,
    TRAVERSAL_FINISH,
    LEVEL_PUSH,
    LEVEL_POP,
    ENTITY,
  };

  // Possible entity types.
  enum EntityType
  {
    NONE = 0, BREP, REGION, SHELL, FACE, LOOP, COEDGE, EDGE, VERTEX, 
    NURBS_SURFACE, UV_CURVE, XYZ_CURVE, TRI_STRIP_SET, TRI_FAN_SET, POINT_SET, 
    LINE_STRIP_SET, POLYGON_SET, POINT, LIGHT_SET, MATERIAL, TRANSFORM, 
    TEX_IMAGE, POINT_LIGHT, INFINITE_LIGHT, PROPERTY, ASSEMBLY, PART, INSTANCE, 
    PLACE_HOLDER, MOTION_PART, CLIENT_DATA, WIRE, CAD_EXPORTER, CAD_IMPORTER,
    ENTITY_STACK, STACK, ENTITY_TABLE, MOTION_EXPORTER, PART_TABLE, 
    PROPERTY_FILTER, TRAVERSER, WREP, HARNESS, WREP_WIRE, BUNDLE, SEGMENT, 
    BOX_SET, CYLINDER_SET, PYRAMID_SET, SPHERE_SET, TRI_PRISM_SET
  };

  // Shape components.
  enum
  {
    SHAPE_ARRAY_VERTICES = (1<<0L),
    SHAPE_ARRAY_NORMALS  = (1<<1L),
    SHAPE_ARRAY_COLORS   = (1<<2L),
    SHAPE_ARRAY_TEXTURE  = (1<<3L)
  };

  typedef bool DbJtTraverserCB ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData );

  DbJtTraverser ( const unsigned int &flags = 0 );

  // Handle to an entity.
  typedef void * EntityHandle;

  // Get the current entity.
  EntityHandle            getCurrentEntity() const { return _currentNode; }

  // Get the DMDTk customer number (needed for versions >= 5).
  const unsigned int &    getCustomerNumber();

  // Get the current entity's property.
  bool                    getMaterial     ( EntityHandle entity, SlMaterialf &material ) const;
  bool                    getMaterial     ( EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, SlMaterialf &material ) const;
  bool                    getName         ( EntityHandle entity, std::string &name ) const;
  std::string             getName         ( EntityHandle entity ) const;
  bool                    getNumLODs      ( EntityHandle entity, unsigned int &numLODs ) const;
  bool                    getNumShapes    ( EntityHandle entity, const unsigned int &whichLOD, unsigned int &numShapes ) const;
  bool                    getNumShapeSets ( EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, unsigned int &numSets ) const;
  EntityHandle            getOriginal     ( EntityHandle instance ) const;
  bool                    getShapeSet     ( EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const unsigned int &whichSet, std::vector<SlVec3f> &vertices, std::vector<SlVec3f> &normals, std::vector<SlVec3f> &colors, std::vector<SlVec2f> &texture, unsigned int &valid ) const;
  bool                    getShapeSet     ( EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const unsigned int &whichSet, std::vector<float> &vertices, std::vector<float> &normals, std::vector<float> &colors, std::vector<float> &texture, unsigned int &valid ) const;
  bool                    getShapeType    ( EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, DbJtTraverser::EntityType &type ) const;
  bool                    getTransform    ( EntityHandle entity, SlMatrix4f &transform ) const;
  bool                    getType         ( EntityHandle entity, EntityType &type ) const;

  // Get the last error.
  const std::string &     getLastError() const { return _error; }

  // Initialize.
  bool                    init();

  // Set the client's callback.
  void                    setCallback ( DbJtTraverserCB *callback, const void *clientData );

  // Set the DMDTk customer number (needed for versions >= 5).
  void                    setCustomerNumber ( const unsigned int &customerNumber ) { _customerNumber = customerNumber; }

  // Traverse the database.
  bool                    traverse ( const char *filename );

  // Reference/unreference this instance.
  void                    ref()   { this->_incrementReferenceCount(); }
  void                    unref() { this->_decrementReferenceCount(); }

protected:

  // Internal flags.
  enum
  {
    _INITIALIZED = (1<<10L)
  };

  unsigned int _flags;
  std::string _error;
  eaiHierarchy *_currentNode;
  DbJtTraverserCB *_clientCallback;
  const void *_clientData;
  int _currentLevel;
  unsigned int _customerNumber;

  virtual ~DbJtTraverser();

  static DbJtTraverser *  _getCurrent();
  eaiMaterial *           _getMaterial ( EntityHandle entity ) const;
  eaiShape *              _getShape ( EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape ) const;

  bool                    _sendMessage ( const Message &message );
  static void             _setCurrent ( DbJtTraverser *traverser );

  bool                    _traverse ( const char *filename );
  static int              _traverseCallback ( eaiHierarchy *node, int level );
  int                     _traverseNotify ( eaiHierarchy *node, int level );

  SL_DECLARE_REFCOUNT_TYPE ( DbJtTraverser );
  SL_DECLARE_DYNAMIC_CLASS ( DbJtTraverser, 0x00001054 );
  SL_DECLARE_BITMASK_FUNCTIONS ( Flags, unsigned int, _flags );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSER_H_
