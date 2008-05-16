
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtTraverser: Jupiter database parsing class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtTraverser.h"
#include "DbJtVisApiArray.h"
#include "DbJtVisApiHeaders.h"

#include "Standard/SlAssert.h"
#include "Standard/SlPrint.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlInline.h"

#include <iostream>

// This is the only way to get a pointer to the traverser from inside the 
// callback function.
namespace CadKit { DbJtTraverser *_currentTraverser = NULL; }

#define UNSET_CUSTOMER_NUMBER 0

using namespace CadKit;

// Macro for reporting progress.
#define NOTIFY if ( this->hasFlags ( VERBOSE ) ) std::cout

SL_IMPLEMENT_DYNAMIC_CLASS(DbJtTraverser,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  It appears that version 5.0 of DMDTk has renumbered the enumerations
//  for the entity types. So instead of typecasting I will assign the 
//  values that I use.
//
///////////////////////////////////////////////////////////////////////////////

const DbJtTraverser::EntityType DbJtTraverser::ASSEMBLY       ( eaiEntity::eaiASSEMBLY );
const DbJtTraverser::EntityType DbJtTraverser::PART           ( eaiEntity::eaiPART );
const DbJtTraverser::EntityType DbJtTraverser::INSTANCE       ( eaiEntity::eaiINSTANCE );
const DbJtTraverser::EntityType DbJtTraverser::LINE_STRIP_SET ( eaiEntity::eaiLINESTRIPSET );
const DbJtTraverser::EntityType DbJtTraverser::POINT_SET      ( eaiEntity::eaiPOINTSET );
const DbJtTraverser::EntityType DbJtTraverser::POLYGON_SET    ( eaiEntity::eaiPOLYGONSET );
const DbJtTraverser::EntityType DbJtTraverser::TRI_STRIP_SET  ( eaiEntity::eaiTRISTRIPSET );
//const DbJtTraverser::EntityType DbJtTraverser::TRI_FAN_SET    ( eaiEntity::eaiTRIFANSET );
const DbJtTraverser::EntityType DbJtTraverser::UNKNOWN        ( eaiEntity::eaiNONE );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraverser::DbJtTraverser ( const unsigned int &flags ) : SlRefBase ( 0 ),
  _flags ( flags ),
  _error ( "" ),
  _currentNode ( NULL ),
  _clientCallback ( NULL ),
  _clientData ( NULL ),
  _currentLevel ( 0 ),
  _customerNumber ( UNSET_CUSTOMER_NUMBER )
{
  SL_PRINT2 ( "In DbJtTraverser::DbJtTraverser(), this = %X\n", this );

#if ( DMDTK_MAJOR_VERSION >= 5 )

  // Initialize DMDTk. This used to be in _init() but since it doesn't 
  // return anything I moved it here.
  eaiEntityFactory::init();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraverser::~DbJtTraverser()
{
  SL_PRINT2 ( "In DbJtTraverser::~DbJtTraverser(), this = %X\n", this );

#if ( DMDTK_MAJOR_VERSION >= 5 )

  // Uninitialize DMDTk. Since it doesn't return anything and, according to 
  // eaiEntityFactory.h, has to be called, I put it here.
  eaiEntityFactory::fini();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::init()
{
  SL_PRINT2 ( "In DbJtTraverser::init(), this = %X\n", this );

  // Get the customer number.
  unsigned int customerNumber = this->getCustomerNumber();

  NOTIFY << "Attempting to register customer number " << customerNumber << "." << std::endl;

  // Register the customer.
  if ( eai_ERROR == eaiEntityFactory::registerCustomer ( customerNumber ) )
  {
    CadKit::format ( _error, "Failed to register customer number: %d.", customerNumber ); 
    NOTIFY << _error.c_str();
    return false;
  }

  NOTIFY << "Done registering customer number " << customerNumber << "." << std::endl;

  // We are now initialized.
  this->addFlags ( _INITIALIZED );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the client's callback.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtTraverser::setCallback ( DbJtTraverserCB *callback, const void *clientData )
{
  SL_PRINT2 ( "In DbJtTraverser::setCallback(), this = %X\n", this );
  _clientCallback = callback;
  _clientData = clientData;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the currently active traverser.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraverser *DbJtTraverser::_getCurrent()
{
  SL_PRINT2 ( "In DbJtTraverser::_getCurrent(), _currentTraverser = %X\n", _currentTraverser );
  return _currentTraverser;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the currently active traverser.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtTraverser::_setCurrent ( DbJtTraverser *traverser )
{
  SL_PRINT2 ( "In DbJtTraverser::_setCurrent(), traverser = %X\n", traverser );
  _currentTraverser = traverser;
}


///////////////////////////////////////////////////////////////////////////////
//
//  So class SlRefPtr works with class eaiEntity.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
void _incrementPointerReferenceCount ( eaiEntity *p )
{
  SL_PRINT2 ( "In _incrementPointerReferenceCount ( eaiEntity * ), p = %X\n", p );
  p->ref();
}
void _decrementPointerReferenceCount ( eaiEntity *p )
{
  SL_PRINT2 ( "In _decrementPointerReferenceCount ( eaiEntity * ), p = %X\n", p );
  p->unref();
}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the database.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::traverse ( const char *filename )
{
  SL_PRINT3 ( "In DbJtTraverser::traverse(), this = %X, filename = %s\n", this, filename );
  SL_ASSERT ( filename );

  try
  {
    return this->_traverse ( filename );
  }

  catch ( ... )
  {
    CadKit::format ( _error, "Exception generated when attempting to traverse." );
    NOTIFY << _error.c_str();
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the database.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::_traverse ( const char *filename )
{
  SL_PRINT3 ( "In DbJtTraverser::_traverse(), this = %X, filename = %s\n", this, filename );
  SL_ASSERT ( filename );

  // Initialize.
  _currentNode = NULL;
  _currentLevel = 0;

  NOTIFY << "Creating CAD importer." << std::endl;

  // Declare a CAD importer. The returned instance has a zero reference count.
  // Assigning it to the SlRefPtr will increment it to one. When the SlRefPtr 
  // goes out of scope the internal pointer will be deferenced (back to zero),
  // and the importer will get deleted.
  SlRefPtr<eaiCADImporter> importer = eaiEntityFactory::createCADImporter();
  if ( importer.isNull() )
  {
    CadKit::format ( _error, "Failed to create CAD importer. Is your license set up correctly?" );
    NOTIFY << _error.c_str();
    return false;
  }

  NOTIFY << "Done creating CAD importer." << std::endl;

  // We want all the levels of detail.
  importer->setShapeLoadOption ( eaiCADImporter::eaiALL_LODS );
  //importer->setShapeLoadOption ( eaiCADImporter::eaiHIGH_LOD );

  // We only want tessellations for now (default is eaiBREP_ONLY).
  importer->setBrepLoadOption ( eaiCADImporter::eaiTESS_ONLY );

  // We want to use instances (rather than explode and make separate parts).
  importer->setAssemblyOption ( eaiCADImporter::eaiINSTANCE_ASSEMBLY );

  NOTIFY << "Creating traverser." << std::endl;

  // Declare the traverser. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiTraverser> traverser = eaiEntityFactory::createTraverser();
  if ( traverser.isNull() )
  {
    CadKit::format ( _error, "Failed to create database traverser." );
    NOTIFY << _error.c_str();
    return false;
  }

  NOTIFY << "Done creating traverser." << std::endl;

  // Hook up the callback function.
  SL_VERIFY ( eai_OK == traverser->setupPreActionCallback ( &DbJtTraverser::_traverseCallback ) );

  // Notify the client.
  if ( false == this->_sendMessage ( IMPORT_START ) )
  {
    CadKit::format ( _error, "Failed to start importing database: %s", filename );
    NOTIFY << _error.c_str();
    return false;
  }

  NOTIFY << "Importing '" << filename << "'." << std::endl;

  // Import the database. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiHierarchy> root = importer->import ( filename );
  if ( root.isNull() )
  {
    CadKit::format ( _error, "Failed to import database: %s", filename );
    NOTIFY << _error.c_str();
    return false;
  }

  NOTIFY << "Done importing." << std::endl;

  // Notify the client.
  if ( false == this->_sendMessage ( IMPORT_FINISH ) )
  {
    CadKit::format ( _error, "Failed to finish importing database: %s", filename );
    NOTIFY << _error.c_str();
    return false;
  }

  // There doesn't appear to be a way to get a pointer to this instance inside 
  // the callback function. So I have to use this work-around.
  DbJtTraverser::_setCurrent ( this );

  // Notify the client.
  if ( false == this->_sendMessage ( TRAVERSAL_START ) )
  {
    CadKit::format ( _error, "Failed to start traversal." );
    NOTIFY << _error.c_str();

    return false;
  }

  NOTIFY << "Traversing '" << filename << "'" << std::endl;

  // Traverse the database.
  if ( eai_OK != traverser->traverseGraph ( root ) )
  {
    CadKit::format ( _error, "Failed to traverse database." );
    NOTIFY << _error.c_str();
    return false;
  }

  NOTIFY << "Done traversing '" << filename << "'" << std::endl;

  // Notify the client.
  if ( false == this->_sendMessage ( TRAVERSAL_FINISH ) )
  {
    CadKit::format ( _error, "Failed to finish traversal." );
    NOTIFY << _error.c_str();
    return false;
  }

  // TODO: It throws an exception when I allow this to be unref'd (and deleted).
  importer.makeNull();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send a message to the callback.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::_sendMessage ( const Message &message )
{
  SL_PRINT3 ( "In DbJtTraverser::_sendMessage(), this = %X, message = %d\n", this, message );

  // If there is a callback...
  if ( _clientCallback )
  {
    // Call it.
    return _clientCallback ( message, *this, _clientData );
  }

  // Since there is no callback function, we did our job.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse callback. For DMDTk >= 5.2.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtTraverser::_traverseCallback ( eaiHierarchy *hierarchy, int level, eaiClientData *data )
{
  SL_PRINT3 ( "In DbJtTraverser::_traverseCallback(), node = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( DbJtTraverser::_getCurrent() );

  // Call the other one.
  return DbJtTraverser::_getCurrent()->_traverseNotify ( hierarchy, level );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse callback. For DMDTk < 5.2.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtTraverser::_traverseCallback ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtTraverser::_traverseCallback(), node = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( DbJtTraverser::_getCurrent() );

  // Call the other one.
  return DbJtTraverser::_getCurrent()->_traverseNotify ( hierarchy, level );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse notify.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtTraverser::_traverseNotify ( eaiHierarchy *node, int level )
{
  SL_PRINT5 ( "In DbJtTraverser::_traverseNotify(), this = %X, node = %X, name = %s, level = %d\n", this, node, ( node->name() ) ? node->name() : "", level );
  SL_ASSERT ( this );

  // Set the current node.
  _currentNode = node;

  // If the level was pushed...
  if ( level > _currentLevel )
  {
    // Loop through all of the pushes. There should only be one but I'm doing 
    // it this way just in case.
    for ( int i = 0; i < level - _currentLevel; ++i )
    {
      // Notify the client.
      if ( false == this->_sendMessage ( LEVEL_PUSH ) )
      {
        CadKit::format ( _error, "Error processing node %X, type %d, name = %s.", node, node->typeID(), node->name() );
        NOTIFY << _error.c_str();
        return eai_ERROR;
      }
    }
  }

  // If the level was popped...
  else if ( level < _currentLevel )
  {
    // Unlike a push, where there is only one level change at a time, 
    // we could pop from level 10 back to 1, depending on how the tree 
    // is structured. Loop through all of the pops.
    for ( int i = 0; i < _currentLevel - level; ++i )
    {
      // Notify the client.
      if ( false == this->_sendMessage ( LEVEL_POP ) )
      {
        CadKit::format ( _error, "Error processing node %X, type %d, name = %s.", node, node->typeID(), node->name() );
        NOTIFY << _error.c_str();
        return eai_ERROR;
      }
    }
  }

  // Save the level.
  _currentLevel = level;

  // Notify the client.
  if ( false == this->_sendMessage ( ENTITY ) )
  {
    CadKit::format ( _error, "Error processing node %X, type %d, name = %s.", node, node->typeID(), node->name() );
    NOTIFY << _error.c_str();
    return eai_ERROR;
  }

  // It worked.
  return eai_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the given entity's type.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getType ( EntityHandle entity, DbJtTraverser::EntityType &type ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getType(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *node = (eaiHierarchy *) entity;

  // Set the type.
  type = (EntityType) node->typeID(); // TODO, bug with 5.0?

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the given entity's name.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbJtTraverser::getName ( EntityHandle entity ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getName(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Call the other one.
  std::string name;
  SL_VERIFY ( this->getName ( entity, name ) );
  return name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the given entity's name.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getName ( EntityHandle entity, std::string &name ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getName(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *node = (eaiHierarchy *) entity;

  // Set the name if it is valid.
  name = ( node->name() ) ? node->name() : "";

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the given entity's transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getTransform ( EntityHandle entity, SlMatrix44f &matrix ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getTransform(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *node = (eaiHierarchy *) entity;

  // Initialize.
  bool success ( false );

  // Ask for the transformation (there may not be one).
  eaiTransform *temp = NULL;
  node->getTransform ( temp );

  // Auto-release. This assignment will increment it from 0 -> 1.
  SlRefPtr<eaiTransform> transform ( temp );

  // If we got a transform...
  if ( transform.isValid() )
  {
    // Get the elements of the matrix.
    DbJtVisApiArray<float> elements;
    transform->getTElements ( elements.getReference() );

    // If we got any elements.
    if ( elements.getReference() )
    {
      // Fill in the given matrix. We assume that the array "elements" 
      // points to 16 valid numbers.
      matrix.setValue ( elements.getReference() );

      // It worked.
      success = true;
    }
  }

  // Did it work?
  return success;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material from the given entity, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

eaiMaterial *DbJtTraverser::_getMaterial ( EntityHandle theHandle ) const
{
  SL_PRINT3 ( "In DbJtTraverser::_getMaterial(), this = %X, entity = %X\n", this, theHandle );
  SL_ASSERT ( theHandle );

  // Initialize.
  eaiMaterial *material = NULL;
  int numMaterials = 0;

  // Get the base pointer.
  eaiEntity *entity = (eaiEntity *) theHandle;

  // Have to explicitly test every concrete type because the API does not 
  // offer a mechanism for checking abstract types, and the base classes do 
  // not have a virtual "getMaterial()".
  switch ( entity->typeID() )
  {
  case eaiEntity::eaiASSEMBLY:
  case eaiEntity::eaiPART:
  case eaiEntity::eaiINSTANCE:

#ifdef _CADKIT_USE_JTOPEN
    // Use new "getAttrib" style of command to get the last material
    // because that is what should be visible.
    numMaterials = ((eaiHierarchy *) entity)->numAttribs( JtkEntity::JtkMATERIAL );
    if(numMaterials > 0)
      ((eaiHierarchy *) entity)->getAttrib(numMaterials - 1, (JtkAttrib *&) material, JtkEntity::JtkMATERIAL);
#else
    ((eaiHierarchy *) entity)->getMaterial ( material );
#endif
    break;

  case eaiEntity::eaiLINESTRIPSET:

    ((eaiLineStripSet *) entity)->getMaterial ( material );
    break;

  case eaiEntity::eaiPOINTSET:

    ((eaiPointSet *) entity)->getMaterial ( material );
    break;

  case eaiEntity::eaiPOLYGONSET:

    ((eaiPolygonSet *) entity)->getMaterial ( material );
    break;

//  case eaiEntity::eaiTRIFANSET:
//
//    ((eaiTriFanSet *) entity)->getMaterial ( material );
//    break;

  case eaiEntity::eaiTRISTRIPSET:

    ((eaiTriStripSet *) entity)->getMaterial ( material );
    break;
  }

  // Return the material, which may still be null.
  return material;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material from the given entity, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getMaterial ( EntityHandle theHandle, 
                                  const unsigned int &whichLOD, 
                                  const unsigned int &whichShape, 
                                  SlMaterialf &material ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getMaterial(), this = %X, theHandle = %X\n", this, theHandle );
  SL_ASSERT ( theHandle );

  // The entity has to be a part.
  eaiEntity *entity = (eaiEntity *) theHandle;
  if ( eaiEntity::eaiPART != entity->typeID() )
    return false;

  // Get the part.
  eaiPart *part = (eaiPart *) entity;

  // Get the shape.
  SlRefPtr<eaiShape> shape = this->_getShape ( part, whichLOD, whichShape );
  if ( shape.isNull() )
    return false;

  // Now get the shape's material.
  return this->getMaterial ( shape, material );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getMaterial ( EntityHandle entity, SlMaterialf &mat ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getMaterial(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Initialize.
  bool success ( false );
  mat.setValid ( 0 );

  // Ask for the material (there may not be one). 
  // This assignment will increment it from 0 -> 1.
  SlRefPtr<eaiMaterial> material = this->_getMaterial ( entity );
  if ( material.isNull() )
    return false;

  // Get the components of the material.
  const float BAD_SHININESS ( -999.0f ); // From DirectModel toolkit's example5.
  DbJtVisApiArray<float> a, d, sp, e;
  float sh ( BAD_SHININESS );

  // Ambient.
  material->getAmbientColor ( a.getReference() );
  if ( a.getReference() )
  {
    mat.setAmbient ( SlVec4f ( a[0], a[1], a[2], a[3] ) );
    success = true;
  }

  // Diffuse.
  material->getDiffuseColor ( d.getReference() );
  if ( d.getReference() )
  {
    mat.setDiffuse ( SlVec4f ( d[0], d[1], d[2], d[3] ) );
    success = true;
  }

  // Specular.
  material->getSpecularColor ( sp.getReference() );
  if ( sp.getReference() )
  {
    mat.setSpecular ( SlVec4f ( sp[0], sp[1], sp[2], sp[3] ) );
    success = true;
  }

  // Emissive.
  material->getEmissionColor ( e.getReference() );
  if ( e.getReference() )
  {
    mat.setEmissive ( SlVec4f ( e[0], e[1], e[2], e[3] ) );
    success = true;
  }

  // Shininess.
  material->getShininess ( sh );
  if ( BAD_SHININESS != sh )
  {
    mat.setShininess ( sh );
    success = true;
  }

  // Did it work?
  return success;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of LODs.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getNumLODs ( EntityHandle entity, unsigned int &numLODs ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getNumLODs(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *hierarchy = (eaiHierarchy *) entity;

  // The hierarchy has to be a part.
  if ( 0 == hierarchy->isOfType ( eaiEntity::eaiPART ) )
    return false;

  // Get the pointer to the part.
  eaiPart *part = (eaiPart *) hierarchy;

  // Get the number of LODs.
  numLODs = part->numPolyLODs();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of shapes for the given LOD.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getNumShapes ( EntityHandle entity, const unsigned int &whichLOD, unsigned int &numShapes ) const
{
  SL_PRINT4 ( "In DbJtTraverser::getNumShapes(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *hierarchy = (eaiHierarchy *) entity;

  // The hierarchy has to be a part.
  if ( 0 == hierarchy->isOfType ( eaiEntity::eaiPART ) )
    return false;

  // Get the pointer to the part.
  eaiPart *part = (eaiPart *) hierarchy;

  // Should be true, but won't crash if it's false.
  SL_ASSERT ( whichLOD < (unsigned int) part->numPolyLODs() );

  // Get the number of shapes.
  numShapes = part->numPolyShapes ( whichLOD );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shape. The returned pointer will have a reference count of zero. 
//  The caller has to reference it (and ultimately dereference it).
//
///////////////////////////////////////////////////////////////////////////////

eaiShape *DbJtTraverser::_getShape ( DbJtTraverser::EntityHandle entity, 
                                     const unsigned int &whichLOD, 
                                     const unsigned int &whichShape ) const
{
  SL_PRINT5 ( "In DbJtTraverser::_getShape(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *hierarchy = (eaiHierarchy *) entity;

  // The hierarchy has to be a part.
  if ( 0 == hierarchy->isOfType ( eaiEntity::eaiPART ) )
    return NULL;

  // Get the pointer to the part.
  eaiPart *part = (eaiPart *) hierarchy;

  // Should be true, but won't crash if it's false.
  SL_ASSERT ( whichLOD < (unsigned int) part->numPolyLODs() );
  SL_ASSERT ( whichShape < (unsigned int) part->numPolyShapes ( whichLOD ) );

  // Ask for the shape (there may not be one).
  eaiShape *shape = NULL;
  part->getPolyShape ( shape, whichLOD, whichShape );

  // Return the shape (which may still be null).
  return shape;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of sets for the given shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getNumShapeSets ( EntityHandle entity, 
                                      const unsigned int &whichLOD, 
                                      const unsigned int &whichShape, 
                                      unsigned int &numSets ) const
{
  SL_PRINT4 ( "In DbJtTraverser::getNumShapeSets(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );
  SL_ASSERT ( entity );

  // Ask for the shape (there may not be one). Catching with SlRefPtr will 
  // increment it from 0 -> 1.
  SlRefPtr<eaiShape> shape = this->_getShape ( entity, whichLOD, whichShape );

  // If we got a shape...
  if ( shape.isValid() )
  {
    // Assign the number of sets.
    numSets = shape->numOfSets();

    // It worked.
    return true;
  }

  // It didn't work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the arrays for the given LOD, shapen and set id.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getShapeSet ( EntityHandle entity, 
                                  const unsigned int &whichLOD, 
                                  const unsigned int &whichShape, 
                                  const unsigned int &whichSet, 
                                  std::vector<float> &vertices, 
                                  std::vector<float> &normals, 
                                  std::vector<float> &colors, 
                                  std::vector<float> &texture, 
                                  unsigned int &valid ) const
{
  SL_PRINT5 ( "In DbJtTraverser::getShapeSet(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( entity );

  // Call the other one.
  std::vector<SlVec3f> v, n, c;
  std::vector<SlVec2f> t;
  if ( false == this->getShapeSet ( entity, whichLOD, whichShape, whichSet, v, n, c, t, valid ) )
    return false;

  // Append to the given vectors. We can ignore the "valid" flag for this.
  CadKit::append3D ( v, vertices );
  CadKit::append3D ( n, normals );
  CadKit::append3D ( c, colors );
  CadKit::append2D ( t, texture );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the arrays for the given LOD, shapen and set id.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getShapeSet ( EntityHandle entity, 
                                  const unsigned int &whichLOD, 
                                  const unsigned int &whichShape, 
                                  const unsigned int &whichSet, 
                                  std::vector<SlVec3f> &vertices, 
                                  std::vector<SlVec3f> &normals, 
                                  std::vector<SlVec3f> &colors, 
                                  std::vector<SlVec2f> &texture, 
                                  unsigned int &valid ) const
{
  SL_PRINT5 ( "In DbJtTraverser::getShapeSet(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( entity );

  // Initialize.
  bool success ( false );
  valid = 0;

  // Ask for the shape (there may not be one). Catching with SlRefPtr will 
  // increment it from 0 -> 1.
  SlRefPtr<eaiShape> shape = this->_getShape ( entity, whichLOD, whichShape );
  if ( shape.isNull() )
    return false;

  // Should be true, but won't crash if it's false.
  SL_ASSERT ( whichSet < (unsigned int) shape->numOfSets() );

  // Needed below. Note: the "vertexCount" is not the number of vertices, 
  // but rather the number of elements in the vertex array. Same for the 
  // other arrays.
  DbJtVisApiArray<float> v, n, c, t;
  int	vertexCount = -1, normalCount = -1, colorCount = -1, textureCount = -1;

  // Get the arrays for this set of the shape.
  shape->getInternal ( v.getReference(), vertexCount, n.getReference(), normalCount, c.getReference(), colorCount, t.getReference(), textureCount, whichSet );

  // Note: we append the arrays to the given std::vectors. This way the 
  // client can repeatedly call this function with the same vectors.

  // Vertices.
  if ( v.getReference() && vertexCount > 0 )
  {
    CadKit::append3D ( vertexCount, v.getReference(), vertices );
    valid |= SHAPE_ARRAY_VERTICES;
    success = true;
  }

  // Normals.
  if ( n.getReference() && normalCount > 0 )
  {
    CadKit::append3D ( normalCount, n.getReference(), normals );
    valid |= SHAPE_ARRAY_NORMALS;
    success = true;
  }

  // Colors. Note: they are just rgb, not rgba.
  if ( c.getReference() && colorCount > 0 )
  {
    CadKit::append3D ( colorCount, c.getReference(), colors );
    valid |= SHAPE_ARRAY_COLORS;
    success = true;
  }

  // Texture.
  if ( t.getReference() && textureCount > 0 )
  {
    CadKit::append2D ( textureCount, t.getReference(), texture );
    valid |= SHAPE_ARRAY_TEXTURE;
    success = true;
  }

  // Did it work?
  return success;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the type of the given entity's shape (if it has one).
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getShapeType ( EntityHandle entity, 
                                   const unsigned int &whichLOD, 
                                   const unsigned int &whichShape, 
                                   DbJtTraverser::EntityType &type ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getShapeType(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Initialize.
  type = UNKNOWN;

  // Ask for the shape (there may not be one). Catching with SlRefPtr will 
  // increment it from 0 -> 1.
  SlRefPtr<eaiShape> shape = this->_getShape ( entity, whichLOD, whichShape );
  if ( shape.isNull() )
    return false;

  // Set the type.
  type = (EntityType) shape->typeID(); // TODO, bug with 5.0?

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the original part or assembly associated with the given instance.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraverser::EntityHandle DbJtTraverser::getOriginal ( EntityHandle entity ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getOriginal(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // It has to be an instance.
  if ( eaiEntity::eaiINSTANCE != ((eaiEntity *) entity)->typeID() )
    return NULL;

  // Return a pointer to the original part or assembly (which may be null).
  return ((eaiInstance *) entity)->original();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the customer number.
//
///////////////////////////////////////////////////////////////////////////////

const unsigned int &DbJtTraverser::getCustomerNumber()
{
  SL_PRINT2 ( "In DbJtTraverser::getCustomerNumber(), this = %X\n", this );

  // Was it set by the client?
  if ( UNSET_CUSTOMER_NUMBER != _customerNumber )
    return _customerNumber;

  // Try to get it from the environment.
#ifdef _CADKIT_USE_JTOPEN
#if defined _WIN32 && _MSC_VER >= 1400
  char *temp;
  size_t sz;
  if( _dupenv_s( &temp, &sz, "JTTK_CUSTOMER_ID" ) )
  {
    _customerNumber = 0;
    return _customerNumber;
  }
#else
  const char *temp = ::getenv ( "JTTK_CUSTOMER_ID" );
#endif
#else // _CADKIT_USE_JTOPEN
  const char *temp = ::getenv ( "DMDTK_CUSTOMER_ID" );
#endif // _CADKIT_USE_JTOPEN
  if ( temp ) 
    _customerNumber = ::atoi ( temp );

#if defined _WIN32 && _MSC_VER >= 1400
  free( temp );
#endif

  // Return what we have.
  return _customerNumber;
}

