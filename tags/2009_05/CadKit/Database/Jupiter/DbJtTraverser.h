

///////////////////////////////////////////////////////////////////////////////

//

//  Copyright (c) 2002, Perry L. Miller IV

//  All rights reserved.

//  BSD License: http://www.opensource.org/licenses/bsd-license.html

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

#include "Standard/SlMatrix44.h"

#include "Standard/SlMaterial.h"

#include "Standard/SlVec4.h"

#include "Standard/SlVec3.h"

#include "Standard/SlVec2.h"

#include "Standard/SlBitmask.h"



#ifndef _CADKIT_USE_PRECOMPILED_HEADERS

# include <vector>

#endif

#ifdef _CADKIT_USE_JTOPEN

class JtkHierarchy;

class JtkMaterial;

class JtkShape;

class JtkClientData;

#define eaiHierarchy JtkHierarchy
#define eaiMaterial JtkMaterial
#define eaiTexImage JtkTexImage
#define eaiShape JtkShape
#define eaiClientData JtkClientData

#else // _CADKIT_USE_JTOPEN

class eaiHierarchy;

class eaiMaterial;

class eaiShape;

class eaiClientData;

#endif // _CADKIT_USE_JTOPEN



namespace CadKit

{

class DB_JT_API DbJtTraverser : public SlRefBase

{

public:



  // Possible flags.

  enum

  {

    VERBOSE = 1 // Print progress and errors to stdout. Default is off.

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

  typedef unsigned int EntityType;

  static const EntityType ASSEMBLY;

  static const EntityType PART;

  static const EntityType INSTANCE;

  static const EntityType LINE_STRIP_SET;

  static const EntityType POINT_SET;

  static const EntityType POLYGON_SET;

  static const EntityType TRI_STRIP_SET;

//  static const EntityType TRI_FAN_SET;

  static const EntityType UNKNOWN;



  // Shape components.

  enum

  {

    SHAPE_ARRAY_VERTICES = 1,

    SHAPE_ARRAY_NORMALS  = 2,

    SHAPE_ARRAY_COLORS   = 4,

    SHAPE_ARRAY_TEXTURE  = 8

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

  bool                    getTransform    ( EntityHandle entity, SlMatrix44f &transform ) const;

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

  static int              _traverseCallback ( eaiHierarchy *hierarchy, int level, eaiClientData *data ); // DMDTk >= 5.2

  static int              _traverseCallback ( eaiHierarchy *hierarchy, int level ); // DMDTk < 5.2

  int                     _traverseNotify ( eaiHierarchy *node, int level );



  SL_DECLARE_REFERENCE_POINTER ( DbJtTraverser );

  SL_DECLARE_DYNAMIC_CLASS ( DbJtTraverser, 1032912016 );

  SL_DECLARE_BITMASK_FUNCTIONS ( Flags, unsigned int, _flags );

};



}; // namespace CadKit



#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_TRAVERSER_H_

