
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
//  TrJt2Pf: The Jupiter to Performer translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "TrJt2PfPrecompiled.h"
#include "TrJt2Pf.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
# include "Standard/SlPathname.h"
# include "Standard/SlBitmask.h"
# include "Performer/pf/pfDCS.h"
# include "Performer/pr/pfGeoSet.h"
# include "Performer/pr/pfGeoState.h"
#endif

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TrJt2Pf::TrJt2Pf() :
  _jtTraverser ( NULL ),
  _error ( "" )
{
  SL_PRINT2 ( "In TrJt2Pf::TrJt2Pf(), this = %X,\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TrJt2Pf::~TrJt2Pf()
{
  SL_PRINT2 ( "In TrJt2Pf::~TrJt2Pf(), this = %X,\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  So class SlRefPtr works with class pfObject.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
void _incrementPointerReferenceCount ( pfObject *p )
{
  SL_PRINT2 ( "In _incrementPointerReferenceCount ( pfObject * ), p = %X, ref-count = %d\n", p, p->getRef() );
  p->ref();
}
void _decrementPointerReferenceCount ( pfObject *p )
{
  SL_PRINT2 ( "In _decrementPointerReferenceCount ( pfObject * ), p = %X, ref-count = %d\n", p, p->getRef() );
  p->unref();
}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::init()
{
  SL_PRINT2 ( "In TrJt2Pf::init(), this = %X,\n", this );

  // Allocate.
  _jtTraverser = new DbJtTraverser;
  if ( _jtTraverser.isNull() )
  {
    _error = "Failed to allocate memory.";
    return false;
  }

  // Clear the error string.
  _error = "";

  // Set the callback function.
  _jtTraverser->setCallback ( &TrJt2Pf::_traverseCallback, this );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate. Note: we return a bool instead of a pfGroup pointer because, 
//  this way if the translation fails part way through, the client can decide 
//  whether or not to use the partially formed scene. If we didn't return
//  a bool they would get all or nothing (a complete scene or NULL).
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::translate ( const char *filename, pfGroup &root )
{
  SL_PRINT3 ( "In TrJt2Pf::translate(), this = %X, filename = %s\n", this, filename );
  SL_ASSERT ( _jtTraverser.isValid() );

  // Make sure we have just one group on the stack.
  _groupStack.clear();
  _groupStack.push_back ( Group ( &root, SlMaterialf() ) );

  // Tell the traverser to traverse the database.
  if ( false == _jtTraverser->traverse ( filename ) )
  {
    _error = _jtTraverser->getLastError();
    return false;
  }

  // The database traversal stops when the last child is parsed, which may be 
  // deep in the heiarchy.
  SL_ASSERT ( _groupStack.size() >= 1 );
  SL_ASSERT ( &root == _groupStack.front().getGroup() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse callback.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_traverseCallback ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData )
{
  SL_PRINT3 ( "In TrJt2Pf::_traverseCallback(), message = %d, clientData = %X\n", message, clientData );
  SL_ASSERT ( clientData );

  // Call the other one.
  return ((TrJt2Pf *) clientData)->_traverseNotify ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse notify.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_traverseNotify ( const DbJtTraverser::Message &message )
{
  SL_PRINT3 ( "In TrJt2Pf::_traverseNotify(), message = %d, this = %X\n", message, this );
  SL_ASSERT ( this );

  // See what kind of message it is.
  switch ( message )
  {
  case DbJtTraverser::IMPORT_START:
  case DbJtTraverser::IMPORT_FINISH:
  case DbJtTraverser::TRAVERSAL_START:
  case DbJtTraverser::TRAVERSAL_FINISH:
  case DbJtTraverser::LEVEL_PUSH:

    // Do nothing.
    break;

  case DbJtTraverser::LEVEL_POP:

    // Pop the Performer scene tree level.
    return this->_endCurrentGroup();

  case DbJtTraverser::ENTITY:

    // We have an entity.
    return this->_processEntity ( _jtTraverser->getCurrentEntity() );

  default:

    SL_ASSERT ( 0 ); // What message is this?
    break;           // Keep going.
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the entity.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_processEntity ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT3 ( "In TrJt2Pf::_processEntity(), this = %X, entity = %X\n", this, entity );

  // Get the current entity type.
  DbJtTraverser::EntityType type;
  SL_VERIFY ( _jtTraverser->getType ( entity, type ) );

  // Process the entity types that we are interested in.
  switch ( type )
  {
  case DbJtTraverser::ASSEMBLY:

    // Add the part to the Performer scene.
    return this->_assemblyStart ( entity );
    
  case DbJtTraverser::PART:

    // Add the part to the Performer tree.
    return this->_addPart ( entity );
    
  case DbJtTraverser::INSTANCE:

    // Add the instance to the XML tree.
    return this->_addInstance ( entity );

  default:

    // Do nothing.
    break;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the name, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addName ( DbJtTraverser::EntityHandle entity, pfNode &node )
{
  SL_PRINT3 ( "In TrJt2Pf::_addName(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query name of the entity, there may not be one.
  std::string name;
  if ( false == _jtTraverser->getName ( entity, name ) )
    return false;

  // Add the name.
  node.setName ( name.c_str() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the transformation matrix, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addTransform ( DbJtTraverser::EntityHandle entity, pfDCS &dcs )
{
  SL_PRINT3 ( "In TrJt2Pf::_addTransform(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query the transformation, there may not be one.
  SlMatrix4f T;
  if ( false == _jtTraverser->getTransform ( entity, T ) )
    return false;

  // Put the SlMatrix into a pfMatrix.
  pfMatrix M ( T(0,0), T(0,1), T(0,2), T(0,3),
               T(1,0), T(1,1), T(1,2), T(1,3),
               T(2,0), T(2,1), T(2,2), T(2,3),
               T(3,0), T(3,1), T(3,2), T(3,3) );

  // Add the transformation to the coordinate system.
  dcs.setMat ( M );

  // It worked.
  return true;
}

/*
///////////////////////////////////////////////////////////////////////////////
//
//  Add the material, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addMaterial ( DbJtTraverser::EntityHandle entity, pfGroup &group )
{
  SL_PRINT3 ( "In TrJt2Pf::_addMaterial(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query the material, there may not be one.
  SlVec4f ambient, diffuse, specular, emissive;
  float shininess;
  unsigned int valid ( 0 );
  if ( false == _jtTraverser->getMaterial ( entity, ambient, diffuse, specular, emissive, shininess, valid ) )
    return false;

  // Make a group for the material.
  pfGroup::Ptr material = (use pfMalloc) pfGroup ( "material" );
  if ( material.isNull() )
    return false;

  // Add the colors.
  this->_addColor ( valid, DbJtTraverser::MATERIAL_COLOR_AMBIENT,  ambient, "ambient",  *material );
  this->_addColor ( valid, DbJtTraverser::MATERIAL_COLOR_DIFFUSE,  ambient, "diffuse",  *material );
  this->_addColor ( valid, DbJtTraverser::MATERIAL_COLOR_SPECULAR, ambient, "specular", *material );
  this->_addColor ( valid, DbJtTraverser::MATERIAL_COLOR_EMISSIVE, ambient, "emissive", *material );

  // Add the shininess.
  if ( CadKit::hasBits ( valid, DbJtTraverser::MATERIAL_COLOR_SHININESS ) )
  {
    // Add a leaf for the shininess.
    SlAString tempString;
    tempString.format ( "%f", shininess );
    material->addChild ( new DbXmlLeaf ( "shininess", tempString.c_str() ) );
  }

  // If we added any children...
  if ( material->getNumChildren() > 0 )
  {
    // Add the material to the group.
    group.addChild ( material );

    // It worked.
    return true;
  }

  // It didn't work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the material color, if it is valid.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addColor ( const unsigned int &valid, 
                           const unsigned int &which, 
                           const SlVec4f &color, 
                           const char *colorName, 
                           pfGroup &material )
{
  SL_PRINT5 ( "In TrJt2Pf::_addColor(), this = %X, valid = %d, which = %d, colorName = %s\n", this, valid, which, colorName );
  SL_ASSERT ( colorName );

  // See if the color is valid.
  if ( CadKit::hasBits ( valid, which ) )
  {
    // Add a leaf for the color.
    SlAString tempString;
    tempString.format ( "%f %f %f %f", color[0], color[1], color[2], color[3] );
    material.addChild ( new DbXmlLeaf ( colorName, tempString.c_str() ) );
    return true;
  }

  // No color added.
  return false;
}
*/

///////////////////////////////////////////////////////////////////////////////
//
//  Create a Performer group node and fill in the common properties.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_createGroup ( DbJtTraverser::EntityHandle entity, TrJt2Pf::Group &group )
{
  SL_PRINT4 ( "In TrJt2Pf::_createGroup(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Make a new dynamic coordinate system. We make a dynamic coordinate 
  // system and leave it up to the client to decide to optimize (by 
  // converting to static).
  pfDCS *dcs = new pfDCS;
  if ( NULL == dcs )
    return false;

  // Add the coordinate system to the given group.
  group.setGroup ( dcs );

  // Add some properties to the group.
  this->_addName      ( entity, *dcs );
  this->_addTransform ( entity, *dcs );

  // Get the material, if there is one.
  SlMaterialf material;
  if ( _jtTraverser->getMaterial ( entity, material ) )
  {
    // Jupiter assemblies can contain materials. However, there is no 
    // corresponding association in Performer. Only pfGeoSets, through 
    // their pfGeoStates, can have materials. And pfGeoSets are collections
    // of primitives (i.e., eaiShapes). So, we save the material along 
    // side of the group that represents the assembly.
HERE
Need to rethink the performer structure.
  }



  //this->_addMaterial  ( entity, *group );

  // Return the new group.
  return group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_assemblyStart ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT3 ( "In TrJt2Pf::_assemblyStart(), this = %X, entity = %X\n", this, entity );

  // Make a new assembly.
  SlRefPtr<pfGroup> assembly = this->_createGroup ( entity );
  if ( assembly.isNull() )
    return false;

  // Add the new assembly to the tree.
  _groupStack.back().getGroup()->addChild ( assembly );

  // Make the new assembly the current one.
  _groupStack.push_back ( assembly );

  // It worked.
  return true;
}

/*
///////////////////////////////////////////////////////////////////////////////
//
//  Add the part.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addPart ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT3 ( "In TrJt2Pf::_addPart(), this = %X, entity = %X\n", this, entity );

  // Make a new group for the part.
  pfGroup::Ptr part = this->_createGroup ( "part", entity );
  if ( part.isNull() )
    return false;

  // Add the LOD groups.
  this->_addLODs ( entity, *part );

  // Add the new part to the tree.
  _groupStack.back()->addChild ( part );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the LOD groups. A Level-of-Detail (LOD) is a group, and each part 
//  has a group of LODs.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addLODs ( DbJtTraverser::EntityHandle entity, pfGroup &part )
{
  SL_PRINT3 ( "In TrJt2Pf::_addLODs(), this = %X, entity = %X\n", this, entity );

  // Get the number of shape LODs.
  unsigned int numLODs ( 0 );
  if ( false == _jtTraverser->getNumLODs ( entity, numLODs ) )
    return false;

  // Should be true.
  SL_ASSERT ( numLODs > 0 );

  // Make a new group for the LODs.
  pfGroup::Ptr lods = new pfGroup ( "lods" );
  if ( lods.isNull() )
    return false;

  // Loop through the LODs.
  for ( unsigned int i = 0; i < numLODs; ++i )
  {
    // Add the LOD.
    this->_addLOD ( entity, i, *lods );
  }

  // If we added any children...
  if ( lods->getNumChildren() > 0 )
  {
    // Add the LODs group to the part group.
    part.addChild ( lods );

    // It worked.
    return true;
  }

  // It didn't work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a LOD group (holding shapes) to the group of LODs.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addLOD ( DbJtTraverser::EntityHandle entity, 
                         const unsigned int &whichLOD, 
                         pfGroup &lods )
{
  SL_PRINT4 ( "In TrJt2Pf::_addLOD(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );

  // Get the number of shapes for this LOD.
  unsigned int numShapes ( 0 );
  if ( false == _jtTraverser->getNumShapes ( entity, whichLOD, numShapes ) )
    return false;

  // Should be true.
  SL_ASSERT ( numShapes > 0 );

  // Make a new group for the LOD.
  pfGroup::Ptr lod = new pfGroup ( "lod" ); // Notice it's not "lods".
  if ( lod.isNull() )
    return false;

  // Loop through the shapes.
  for ( unsigned int i = 0; i < numShapes; ++i )
  {
    // Add the shape.
    this->_addShape ( entity, whichLOD, i, *lod );
  }

  // If we added any children...
  if ( lod->getNumChildren() > 0 )
  {
    // Add the LOD to the group of LODs.
    lods.addChild ( lod );
    
    // It worked.
    return true;
  }

  // It didn't work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a shape.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addShape ( DbJtTraverser::EntityHandle entity, 
                           const unsigned int &whichLOD, 
                           const unsigned int &whichShape, 
                           pfGroup &lod )
{
  SL_PRINT5 ( "In TrJt2Pf::_addShape(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );

  // Get the number of sets for this shape.
  unsigned int numSets ( 0 );
  if ( false == _jtTraverser->getNumSets ( entity, whichLOD, whichShape, numSets ) )
    return false;

  // Should be true.
  SL_ASSERT ( numSets > 0 );

  // Make a new group for the shape.
  pfGroup::Ptr shape = new pfGroup ( "shape" );
  if ( shape.isNull() )
    return false;

  // Determine the type of shape. Note: we don't want the given entity's type 
  // (it's most likely a part), we want the type of its shape.
  DbJtTraverser::EntityType type;
  if ( false == _jtTraverser->getShapeType ( entity, whichLOD, whichShape, type ) )
    return false;

  // Determine a proper name for the set.
  std::string name;
  switch ( type )
  {
  case DbJtTraverser::LINE_STRIP_SET: name = "line_strip"; break;
  case DbJtTraverser::POINT_SET:      name = "point_set";  break;
  case DbJtTraverser::POLYGON_SET:    name = "polygon";    break;
  case DbJtTraverser::TRI_FAN_SET:    name = "tri_fan";    break;
  case DbJtTraverser::TRI_STRIP_SET:  name = "tri_strip";  break;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    _jtTraverser->getName ( entity, name );
    CadKit::format ( _error, "Unknown shape type for entity = %X, name = %s, LOD = %d, shape = %d", entity, name.c_str(), whichLOD, whichShape );
    return false;
  }

  // Loop through the sets.
  for ( unsigned int i = 0; i < numSets; ++i )
  {
    // Add the shape.
    this->_addSet ( entity, whichLOD, whichShape, i, name, *shape );
  }

  // If we added any children...
  if ( shape->getNumChildren() > 0 )
  {
    // Add the shape to the lod.
    lod.addChild ( shape );
    return true;
  }

  // It didn't work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's array set.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addSet ( DbJtTraverser::EntityHandle entity, 
                         const unsigned int &whichLOD, 
                         const unsigned int &whichShape, 
                         const unsigned int &whichSet, 
                         const std::string &name,
                         pfGroup &shape )
{
  SL_PRINT5 ( "In TrJt2Pf::_addSet(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( false == name.empty() );

  // Get the shape.
  std::vector<float> vertices, normals, colors, texture;
  unsigned int valid ( 0 );
  if ( false == _jtTraverser->getShape ( entity, whichLOD, whichShape, whichSet, vertices, normals, colors, texture, valid ) )
    return false;

  // Make a group for the set.
  pfGroup::Ptr set = new pfGroup ( name.c_str() );
  if ( set.isNull() )
    return false;

  // Add the colors.
  this->_addArray ( valid, DbJtTraverser::SHAPE_ARRAY_VERTICES, vertices, "vertices", *set );
  this->_addArray ( valid, DbJtTraverser::SHAPE_ARRAY_NORMALS,  normals,  "normals",  *set );
  this->_addArray ( valid, DbJtTraverser::SHAPE_ARRAY_COLORS,   colors,   "colors",   *set );
  this->_addArray ( valid, DbJtTraverser::SHAPE_ARRAY_TEXTURE,  texture,  "texture",  *set );

  // If we added any children...
  if ( set->getNumChildren() > 0 )
  {
    // Add the set to the shape.
    shape.addChild ( set );

    // It worked.
    return true;
  }

  // It didn't work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the array (if the flag says to).
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addArray ( const unsigned int &valid, 
                          const unsigned int &which, 
                          const std::vector<float> &array, 
                          const char *arrayName, 
                          pfGroup &set )
{
  SL_PRINT6 ( "In TrJt2Pf::_addArray(), this = %X, valid = %d, which = %d, array.size() = %d, arrayName = %s\n", this, valid, which, array.size(), arrayName );
  SL_ASSERT ( arrayName );

  // See if the array is valid.
  if ( CadKit::hasBits ( valid, which ) )
  {
    // Should be true.
    SL_ASSERT ( false == array.empty() );

    // Copy the array to the string.
    SlAString tempString;
    this->_setArrayString ( array, tempString );

    // Add a leaf for the array.
    set.addChild ( new DbXmlLeaf ( arrayName, tempString.c_str() ) );
  }

  // If we get to here then we did our job.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the string to hold the given array.
//
///////////////////////////////////////////////////////////////////////////////

void TrJt2Pf::_setArrayString ( const std::vector<float> &array, SlAString &s )
{
  SL_PRINT3 ( "In TrJt2Pf::_setArrayString(), this = %X, array.size() = %d\n", this, array.size() );
  SL_ASSERT ( false == array.empty() );

  // Make sure it's empty.
  s.clear();

  // Loop through the array (except the last one) and append to the string.
  SlAString temp;
  unsigned int stop ( array.size() - 1 );
  for ( unsigned int i = 0; i < stop; ++i )
  {
    // Format the temporary string.
    temp.format ( "%f ", array[i] );

    // Append it to the "array string".
    s.append ( temp );
  }

  // Now append the last element (the format is slightly different).
  temp.format ( "%f", array[i] );
  s.append ( temp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addInstance ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT3 ( "In TrJt2Pf::_addInstance(), this = %X, entity = %X\n", this, entity );

  // Make a new group for the part.
  pfGroup::Ptr instance = this->_createGroup ( "instance", entity );
  if ( instance.isNull() )
    return false;

  // Add the new part to the tree.
  _groupStack.back()->addChild ( instance );

  // It worked.
  return true;
}
*/

///////////////////////////////////////////////////////////////////////////////
//
//  Pop the Performer scene tree level.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_endCurrentGroup()
{
  SL_PRINT2 ( "In TrJt2Pf::_endCurrentGroup(), this = %X\n", this );
  SL_ASSERT ( _groupStack.size() >= 2 );

  // Pop the group.
  _groupStack.pop_back();

  // It worked.
  return true;
}
