
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
# include "Performer/pf/pfLOD.h"
# include "Performer/pf/pfGeode.h"
# include "Performer/pr/pfGeoSet.h"
# include "Performer/pr/pfGeoState.h"
# include "Performer/pr/pfLinMath.h"
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
//  So class SlRefPtr works with class pfMemory.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
void _incrementPointerReferenceCount ( pfMemory *p )
{
  SL_PRINT3 ( "In _incrementPointerReferenceCount ( pfMemory * ), p = %X, ref-count = %d\n", p, p->getRef() );
  p->ref();
}
void _decrementPointerReferenceCount ( pfMemory *p )
{
  SL_PRINT3 ( "In _decrementPointerReferenceCount ( pfMemory * ), p = %X, ref-count = %d\n", p, p->getRef() );
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
    //return this->_addInstance ( entity );

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


///////////////////////////////////////////////////////////////////////////////
//
//  Add the material, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addMaterial ( DbJtTraverser::EntityHandle entity, TrJt2Pf::Group &group )
{
  SL_PRINT3 ( "In TrJt2Pf::_addMaterial(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Get the material, if there is one.
  SlMaterialf material;
  if ( _jtTraverser->getMaterial ( entity, material ) )
  {
    // Jupiter assemblies and parts can contain materials. 
    // However, there is no corresponding association in Performer. 
    // Only pfGeoSets, through their pfGeoStates, can have materials. 
    // And pfGeoSets are collections of primitives (i.e., eaiShapes). 
    // So, we save the material along side of the group that represents
    // the assembly, part, or instance.
    group.setMaterial ( material );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a Performer group node and fill in the common properties.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_createGroup ( DbJtTraverser::EntityHandle entity, TrJt2Pf::Group &group )
{
  SL_PRINT3 ( "In TrJt2Pf::_createGroup(), this = %X, entity = %X\n", this, entity );
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
  this->_addMaterial  ( entity, group );

  // It worked.
  return true;
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
  Group assembly;
  if ( false == this->_createGroup ( entity, assembly ) )
    return false;

  // Add the new assembly group to the Performer scene.
  _groupStack.back().getGroup()->addChild ( assembly.getGroup() );

  // Make the new assembly the current one.
  _groupStack.push_back ( assembly );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the part.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addPart ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT3 ( "In TrJt2Pf::_addPart(), this = %X, entity = %X\n", this, entity );

  // Make a new group for the part.
  Group part;
  if ( false == this->_createGroup ( entity, part ) )
    return false;

  // Add the LOD groups.
  this->_addLODs ( entity, part );

  // Add the new part to the tree.
  _groupStack.back().getGroup()->addChild ( part.getGroup() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the LOD groups. A Level-of-Detail (LOD) is a group, and each part 
//  has one or more LODs.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addLODs ( DbJtTraverser::EntityHandle entity, TrJt2Pf::Group &part )
{
  SL_PRINT3 ( "In TrJt2Pf::_addLODs(), this = %X, entity = %X\n", this, entity );

  // Get the number of LODs.
  unsigned int numLODs ( 0 );
  if ( false == _jtTraverser->getNumLODs ( entity, numLODs ) )
    return false;

  // Should be true.
  SL_ASSERT ( numLODs > 0 );

  // Loop through the LODs.
  for ( unsigned int i = 0; i < numLODs; ++i )
  {
    // Add the LOD.
    this->_addLOD ( entity, i, part );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the LOD group (holding shapes) to the part.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addLOD ( DbJtTraverser::EntityHandle entity, 
                        const unsigned int &whichLOD, 
                        TrJt2Pf::Group &part )
{
  SL_PRINT4 ( "In TrJt2Pf::_addLOD(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );

  // Get the number of shapes for this LOD.
  unsigned int numShapes ( 0 );
  if ( false == _jtTraverser->getNumShapes ( entity, whichLOD, numShapes ) )
    return false;

  // Should be true.
  SL_ASSERT ( numShapes > 0 );

  // Make a new LOD.
  SlRefPtr<pfLOD> lod = new pfLOD;
  if ( lod.isNull() )
    return false;

  // Make a new Geode.
  SlRefPtr<pfGeode> geode = new pfGeode;
  if ( geode.isNull() )
    return false;

  // Add the geode to the LOD.
  lod->addChild ( geode );

  // Loop through the shapes.
  for ( unsigned int i = 0; i < numShapes; ++i )
  {
    // Add the shape.
    this->_addShape ( entity, whichLOD, i, part, *geode );
  }

  // If we added any children...
  if ( lod->getNumChildren() > 0 )
  {
    // Add the LOD to the part.
    part.getGroup()->addChild ( lod );
    
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
                          const TrJt2Pf::Group &part,
                          pfGeode &geode )
{
  SL_PRINT5 ( "In TrJt2Pf::_addShape(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );

  // Get the number of sets for this shape.
  unsigned int numSets ( 0 );
  if ( false == _jtTraverser->getNumShapeSets ( entity, whichLOD, whichShape, numSets ) )
    return false;

  // Should be true.
  SL_ASSERT ( numSets > 0 );

  // Make a new GeoSet for the shape.
  SlRefPtr<pfGeoSet> gset = new pfGeoSet;
  if ( gset.isNull() )
    return false;

  // Set the GeoSet's number of primitives.
  gset->setNumPrims ( numSets );

  // Make a GeoState to hold this GeoSet's state.
  SlRefPtr<pfGeoState> state = new pfGeoState;

  // Look for (and add) the material. It could either belong to this shape, 
  // the parent part or to one of its parent assemblies.
  this->_addMaterial ( entity, whichLOD, whichShape, part, *state );

  // TODO. Make this work.
  //this->_addTexture ( part, state );

  // Determine the type of shape. Note: we don't want the given entity's type 
  // (it's most likely a part), we want the type of its shape.
  DbJtTraverser::EntityType type;
  if ( false == _jtTraverser->getShapeType ( entity, whichLOD, whichShape, type ) )
    return false;

  // Set the GeoSet's primitive type.
  switch ( type )
  {
  case DbJtTraverser::LINE_STRIP_SET: gset->setPrimType ( PFGS_LINESTRIPS ); break;
  case DbJtTraverser::POINT_SET:      gset->setPrimType ( PFGS_POINTS );     break;
  case DbJtTraverser::POLYGON_SET:    gset->setPrimType ( PFGS_POLYS );      break;
  case DbJtTraverser::TRI_FAN_SET:    gset->setPrimType ( PFGS_TRIFANS );    break;
  case DbJtTraverser::TRI_STRIP_SET:  gset->setPrimType ( PFGS_TRISTRIPS );  break;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    CadKit::format ( _error, "Unknown shape type '%d' for entity = %X, name = %s, LOD = %d, shape = %d", type, entity, _jtTraverser->getName ( entity ).c_str(), whichLOD, whichShape );
    return false;
  }

  // Declare the vectors here and keep appending to them.
  std::vector<float> vertices, normals, colors, texture;

  You might want to think making a pfGeoSet for each set, because, is there any guarantee that each set in the shaps will have the same arrays? For example, will set 1 have colors, and set 2 not? I don't think you can jam that into one pfGeoSet.

  // Loop through the sets.
  for ( unsigned int i = 0; i < numSets; ++i )
  {
    // Add the shape.
    this->_addSet ( entity, whichLOD, whichShape, i, *gset );
  }

  SL_ASSERT ( 0 ); // TODO, Need to call pfGeoSet::setPrimLengths().

  // Add the GeoSet to the group.
  group.addChild ( gset );

  // It work ed.
  return true;
}


//TODO
//1. Call pfGeoSet::setPrimLengths() above.
//2. Have to make one big array to pass to pfGeoSet::setAttr(), not many 
//   small ones like you are now. Use the same vector over and over and just 
//   append to the end of it.


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's array set.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Pf::_addSet ( DbJtTraverser::EntityHandle entity, 
                        const unsigned int &whichLOD, 
                        const unsigned int &whichShape, 
                        const unsigned int &whichSet, 
                        std::vector<float> vertices, 
                        std::vector<float> normals, 
                        std::vector<float> colors,
                        std::vector<float> texture )
{
  SL_PRINT5 ( "In TrJt2Pf::_addSet(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );

  // Get the shape.
  std::vector<float> vertices, normals, colors, texture;
  unsigned int valid ( 0 );
  if ( false == _jtTraverser->getShape ( entity, whichLOD, whichShape, whichSet, vertices, normals, colors, texture, valid ) )
    return false;

  // Add the vertices.
  if ( CadKit::hasBits ( valid, DbJtTraverser::SHAPE_ARRAY_VERTICES ) )
    gset->setAttr ( PFGS_COORD3, PFGS_PER_VERTEX, this->_getVec3Array ( vertices ), NULL );

  // Add the normals.
  if ( CadKit::hasBits ( valid, DbJtTraverser::SHAPE_ARRAY_NORMALS ) )
    gset->setAttr ( PFGS_NORMAL3, PFGS_PER_VERTEX, this->_getVec3Array ( normals ), NULL );

  // TODO.
  // Add the colors.
  //if ( CadKit::hasBits ( valid, DbJtTraverser::SHAPE_ARRAY_COLORS ) )
  //  gset->setAttr ( PFGS_COLOR4, PFGS_PER_VERTEX, this->_getVec4Array ( colors ), NULL );

  // Add the texture coordinates.
  //if ( CadKit::hasBits ( valid, DbJtTraverser::SHAPE_ARRAY_TEXTURE ) )
  //  gset->setAttr ( PFGS_TEXCOORD2, PFGS_PER_VERTEX, this->_getVec2Array ( texture ), NULL );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the instance.
//
///////////////////////////////////////////////////////////////////////////////
/*
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
