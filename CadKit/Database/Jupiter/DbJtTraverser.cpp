
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

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtTraverser: Jupiter database parsing class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtTraverser.h"
#include "DbJtVisApiArray.h"

// This hack is because DMDataTk/eaiStandard.h (which is indirectly included 
// below) includes <iostream.h>. This causes VC++ error C2874 with STLport.
#ifdef _WIN32
#define EAI_STANDARD_HXX // From DMDataTk/eaiStandard.h
#define EAI_TOOLKIT_API __declspec(dllimport)
enum { eai_ERROR = 0, eai_OK = 1 };
enum Units { UNKNOWN=0, MICROMETERS, MILLIMETERS, CENTIMETERS, DECIMETERS, 
             METERS, KILOMETERS, INCHES, FEET, YARDS, MILES, MILS };
#endif // _WIN32

#include "DMDataTk/eaiEntityFactory.h" // Doesn't compile in DbJtPrecompiled.h
#include "DMDataTk/eaiCADImporter.h"
#include "DMDataTk/eaiTraverser.h"
#include "DMDataTk/eaiAttrib.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
# include "Standard/SlStringFunctions.h"
# include "Standard/SlInline.h"
#endif

// This is the only way to get a pointer to the traverser from inside the 
// callback function.
namespace CadKit { DbJtTraverser *_currentTraverser = NULL; }

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS(DbJtTraverser,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraverser::DbJtTraverser() : SlRefBase ( 0 ),
  _error ( "" ),
  _currentNode ( NULL ),
  _clientCallback ( NULL ),
  _clientData ( NULL ),
  _currentLevel ( 0 )
{
  SL_PRINT2 ( "In DbJtTraverser::DbJtTraverser(), this = %X\n", this );

  // Should be true for a one-to-one mapping between the entity types.
  SL_ASSERT ( (unsigned int) eaiEntity::eaiNONE == (unsigned int) NONE );
  SL_ASSERT ( (unsigned int) eaiEntity::eaiTRIPRISMSET == (unsigned int) TRI_PRISM_SET );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtTraverser::~DbJtTraverser()
{
  SL_PRINT2 ( "In DbJtTraverser::~DbJtTraverser(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the client's callback.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtTraverser::setCallback ( DbJtTraverserCB *callback, const void *clientData )
{
  SL_PRINT2 ( "In DbJtTraverser::~DbJtTraverser(), this = %X\n", this );
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
  SL_PRINT2 ( "In DbJtTraverser::_getCurrent(), traverser = %X\n", traverser );
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

  // Declare a CAD importer. The returned instance has a zero reference count.
  // Assigning it to the SlRefPtr will increment it to one. When the SlRefPtr 
  // goes out of scope the internal pointer will be deferenced (back to zero),
  // and the importer will get deleted.
  SlRefPtr<eaiCADImporter> importer = eaiEntityFactory::createCADImporter();
  if ( importer.isNull() )
  {
    CadKit::format ( _error, "Failed to create CAD importer." );
    return false;
  }

  // We want all the levels of detail.
  importer->setShapeLoadOption ( eaiCADImporter::eaiALL_LODS );

  // We only want tessellations for now (default is eaiBREP_ONLY).
  importer->setBrepLoadOption ( eaiCADImporter::eaiTESS_ONLY );

  // We want to use instances (rather than explode and make separate parts).
  importer->setAssemblyOption ( eaiCADImporter::eaiINSTANCE_ASSEMBLY );

  // Declare the traverser. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiTraverser> traverser = eaiEntityFactory::createTraverser();
  if ( traverser.isNull() )
  {
    CadKit::format ( _error, "Failed to create database traverser." );
    return false;
  }

  // Hook up the callback function.
  SL_VERIFY ( eai_OK == traverser->setupPreActionCallback ( &DbJtTraverser::_traverseCallback ) );

  // Notify the client.
  if ( false == this->_sendMessage ( IMPORT_START ) )
  {
    CadKit::format ( _error, "Failed to start importing database: %s", filename );
    return false;
  }

  // Import the database. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiHierarchy> root = importer->import ( filename );
  if ( root.isNull() )
  {
    CadKit::format ( _error, "Failed to import database: %s", filename );
    return false;
  }

  // Notify the client.
  if ( false == this->_sendMessage ( IMPORT_FINISH ) )
  {
    CadKit::format ( _error, "Failed to finish importing database: %s", filename );
    return false;
  }

  // There doesn't appear to be a way to get a pointer to this instance inside 
  // the callback function. So I have to use this work-around.
  DbJtTraverser::_setCurrent ( this );

  // Notify the client.
  if ( false == this->_sendMessage ( TRAVERSAL_START ) )
  {
    CadKit::format ( _error, "Failed to start traversal." );
    return false;
  }

  // Traverse the database.
  if ( eai_OK != traverser->traverseGraph ( root ) )
  {
    CadKit::format ( _error, "Failed to traverse database." );
    return false;
  }

  // Notify the client.
  if ( false == this->_sendMessage ( TRAVERSAL_FINISH ) )
  {
    CadKit::format ( _error, "Failed to finish traversal." );
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
//  Traverse callback.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtTraverser::_traverseCallback ( eaiHierarchy *node, int level )
{
  SL_PRINT3 ( "In DbJtTraverser::_traverseCallback(), node = %X, level = %d\n", node, level );
  SL_ASSERT ( DbJtTraverser::_getCurrent() );

  // Call the other one.
  return DbJtTraverser::_getCurrent()->_traverseNotify ( node, level );
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
  type = (EntityType) node->typeID();

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

bool DbJtTraverser::getTransform ( EntityHandle entity, SlMatrix4f &matrix ) const
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
//  Get the current material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getMaterial ( EntityHandle entity, SlMaterialf &mat ) const
{
  SL_PRINT3 ( "In DbJtTraverser::getMaterial(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Get the pointer we need.
  eaiHierarchy *node = (eaiHierarchy *) entity;

  // Initialize.
  bool success ( false );
  mat.setValid ( 0 );

  // Ask for the material (there may not be one).
  eaiMaterial *temp = NULL;
  node->getMaterial ( temp );

  // Auto-release. This assignment will increment it from 0 -> 1.
  SlRefPtr<eaiMaterial> material ( temp );
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
//  Get the shape. Not a member function on purpose. The returned pointer 
//  will have a reference count of zero. The caller has to reference it 
//  (and ultimately dereference it).
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
eaiShape *_getShape ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape )
{
  SL_PRINT4 ( "In DbJtTraverser::_getShape(), entity = %X, whichLOD = %d, whichShape = %d\n", entity, whichLOD, whichShape );
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
}; // namespace CadKit.


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of sets for the given shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getNumSets ( EntityHandle entity, 
                                 const unsigned int &whichLOD, 
                                 const unsigned int &whichShape, 
                                 unsigned int &numSets ) const
{
  SL_PRINT4 ( "In DbJtTraverser::getNumShapeSets(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );
  SL_ASSERT ( entity );

  // Ask for the shape (there may not be one). Catching with SlRefPtr will 
  // increment it from 0 -> 1.
  SlRefPtr<eaiShape> shape = CadKit::_getShape ( entity, whichLOD, whichShape );

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
//  Get the shape arrays for the given LOD and shape id.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtTraverser::getShape ( EntityHandle entity, 
                               const unsigned int &whichLOD, 
                               const unsigned int &whichShape, 
                               const unsigned int &whichSet, 
                               std::vector<float> &vertices, 
                               std::vector<float> &normals, 
                               std::vector<float> &colors, 
                               std::vector<float> &texture, 
                               unsigned int &valid ) const
{
  SL_PRINT5 ( "In DbJtTraverser::getShape(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( entity );

  // Initialize.
  bool success ( false );
  valid = 0;

  // Ask for the shape (there may not be one). Catching with SlRefPtr will 
  // increment it from 0 -> 1.
  SlRefPtr<eaiShape> shape = CadKit::_getShape ( entity, whichLOD, whichShape );
  if ( shape.isNull() )
    return false;

  // Should be true, but won't crash if it's false.
  SL_ASSERT ( whichSet < (unsigned int) shape->numOfSets() );

  // Needed below. Note: the "vertexCount" is not the number of vertices, 
  // but rather the number of elements in the vertext array. Same for the 
  // other arrays.
  DbJtVisApiArray<float> v, n, c, t;
  int	vertexCount = -1, normalCount = -1, colorCount = -1, textureCount = -1;

  // Get the arrays for this set of the shape.
  shape->getInternal ( v.getReference(), vertexCount, n.getReference(), normalCount, c.getReference(), colorCount, t.getReference(), textureCount, whichSet );

  // Vertices.
  if ( v.getReference() && vertexCount > 0 )
  {
    CadKit::copy ( vertexCount * 3, v.getReference(), vertices );
    valid |= SHAPE_ARRAY_VERTICES;
    success = true;
  }

  // Normals.
  if ( n.getReference() && normalCount > 0 )
  {
    CadKit::copy ( normalCount * 3, n.getReference(), normals );
    valid |= SHAPE_ARRAY_NORMALS;
    success = true;
  }

  // Colors.
  if ( c.getReference() && colorCount > 0 )
  {
    CadKit::copy ( colorCount * 3, c.getReference(), colors );
    valid |= SHAPE_ARRAY_COLORS;
    success = true;
  }

  // Texture.
  if ( t.getReference() && textureCount > 0 )
  {
    CadKit::copy ( textureCount, t.getReference(), texture ); // Note: no "* 3" here.
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
  type = NONE;

  // Ask for the shape (there may not be one). Catching with SlRefPtr will 
  // increment it from 0 -> 1.
  SlRefPtr<eaiShape> shape = CadKit::_getShape ( entity, whichLOD, whichShape );
  if ( shape.isNull() )
    return false;

  // Set the type.
  type = (EntityType) shape->typeID();

  // It worked.
  return true;
}
