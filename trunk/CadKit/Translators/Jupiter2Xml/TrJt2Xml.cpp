
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
//  TrJt2Xml: The Jupiter to XML translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "TrJt2XmlPrecompiled.h"
#include "TrJt2Xml.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Database/XML/DbXmlLeaf.h"
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
# include "Standard/SlPathname.h"
# include "Standard/SlBitmask.h"
# include <fstream>
#endif

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TrJt2Xml::TrJt2Xml() :
  _jtTraverser ( NULL ),
  _xmlWrite ( NULL ),
  _xmlRoot ( NULL ),
  _error ( "" )
{
  SL_PRINT ( "In TrJt2Xml::TrJt2Xml(), this = %X,\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TrJt2Xml::~TrJt2Xml()
{
  SL_PRINT ( "In TrJt2Xml::~TrJt2Xml(), this = %X,\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::init()
{
  SL_PRINT ( "In TrJt2Xml::init(), this = %X,\n", this );

  // Allocate.
  _jtTraverser = new DbJtTraverser;
  _xmlWrite = new DbXmlWrite;
  _xmlRoot = new DbXmlGroup;
  if ( _jtTraverser.isNull() || _xmlWrite.isNull() || _xmlRoot.isNull() )
  {
    _error.format ( "Failed to allocate memory." );
    return false;
  }

  // Clear the error string.
  _error = "";

  // Set the callback function.
  _jtTraverser->setCallback ( &TrJt2Xml::_traverseCallback, this );

  // Name the root node.
  _xmlRoot->setName ( "jt2xml" );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::translate ( const char *filename )
{
  SL_PRINT ( "In TrJt2Xml::translate(), this = %X, filename = %s\n", this, filename );
  SL_ASSERT ( _jtTraverser.isValid() && _xmlWrite.isValid() && _xmlRoot.isValid() );

  // Make sure the XML tree is empty.
  _xmlRoot->removeAllChildren();

  // Make sure we have just one group on the stack.
  _groupStack.clear();
  _groupStack.push_back ( _xmlRoot );

  // Tell the traverser to traverse the database.
  if ( false == _jtTraverser->traverse ( filename ) )
  {
    _error = _jtTraverser->getLastError();
    return false;
  }

  // The database traversal stops when the last child is parsed, which may be 
  // deep in the heiarchy.
  SL_ASSERT ( _groupStack.size() >= 1 );
  SL_ASSERT ( _xmlRoot == _groupStack.front() );

  // Write the file.
  SlAPathname out ( filename );
  return this->_write ( out.getDrive() + out.getDirectory() + out.getFilename() + ".xml" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse callback.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_traverseCallback ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData )
{
  SL_PRINT ( "In TrJt2Xml::_traverseCallback(), message = %d, clientData = %X\n", message, clientData );
  SL_ASSERT ( clientData );

  // Call the other one.
  return ((TrJt2Xml *) clientData)->_traverseNotify ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse notify.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_traverseNotify ( const DbJtTraverser::Message &message )
{
  SL_PRINT ( "In TrJt2Xml::_traverseNotify(), message = %d, this = %X\n", message, this );
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

    // Pop the XML tree level.
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

bool TrJt2Xml::_processEntity ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT ( "In TrJt2Xml::_processEntity(), this = %X, entity = %X\n", this, entity );

  // Get the current entity type.
  DbJtTraverser::EntityType type;
  SL_VERIFY ( _jtTraverser->getType ( entity, type ) );

  // Process the entity types that we are interested in.
  switch ( type )
  {
  case DbJtTraverser::ASSEMBLY:

    // Add the part to the XML tree.
    return this->_assemblyStart ( entity );
    
  case DbJtTraverser::PART:

    // Add the part to the XML tree.
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

bool TrJt2Xml::_addName ( DbJtTraverser::EntityHandle entity, DbXmlGroup &group )
{
  SL_PRINT ( "In TrJt2Xml::_addName(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query name of the group, there may not be one.
  std::string name;
  if ( false == _jtTraverser->getName ( entity, name ) )
    return false;

  // Add the name.
  group.addChild ( new DbXmlLeaf ( "name", name.c_str() ) );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the transformation matrix, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_addTransform ( DbJtTraverser::EntityHandle entity, DbXmlGroup &group )
{
  SL_PRINT ( "In TrJt2Xml::_addTransform(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query the transformation, there may not be one.
  SlMatrix4f transform;
  if ( false == _jtTraverser->getTransform ( entity, transform ) )
    return false;

  // Write it on a single line, one row after the other.
  SlAString tempString;
  tempString.format ( 
    "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
    transform[0],  transform[1],  transform[2],  transform[3], 
    transform[4],  transform[5],  transform[6],  transform[7], 
    transform[8],  transform[9],  transform[10], transform[11], 
    transform[12], transform[13], transform[14], transform[15]
    );

  // Add the name-value pair for the transformation.
  group.addChild ( new DbXmlLeaf ( "transform", tempString.c_str() ) );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the material, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_addMaterial ( DbJtTraverser::EntityHandle entity, DbXmlGroup &group )
{
  SL_PRINT ( "In TrJt2Xml::_addMaterial(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query the material, there may not be one.
  SlVec4f ambient, diffuse, specular, emissive;
  float shininess;
  unsigned int valid ( 0 );
  if ( false == _jtTraverser->getMaterial ( entity, ambient, diffuse, specular, emissive, shininess, valid ) )
    return false;

  // Make a group for the material.
  DbXmlGroup::Ptr material = new DbXmlGroup ( "material" );
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

bool TrJt2Xml::_addColor ( const unsigned int &valid, 
                           const unsigned int &which, 
                           const SlVec4f &color, 
                           const char *colorName, 
                           DbXmlGroup &material )
{
  SL_PRINT ( "In TrJt2Xml::_addColor(), this = %X, valid = %d, which = %d, colorName = %s\n", this, valid, which, colorName );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Create an XML group node and fill in the common properties.
//
///////////////////////////////////////////////////////////////////////////////

DbXmlGroup::Ptr TrJt2Xml::_createGroup ( const char *groupName, DbJtTraverser::EntityHandle entity )
{
  SL_PRINT ( "In TrJt2Xml::_createGroup(), this = %X, groupName = %s, entity = %X\n", this, groupName, entity );
  SL_ASSERT ( groupName );

  // Make a new group.
  DbXmlGroup::Ptr group = new DbXmlGroup ( groupName );
  if ( group.isNull() )
    return NULL;

  // Add some properties to the group.
  this->_addName      ( entity, *group );
  this->_addTransform ( entity, *group );
  this->_addMaterial  ( entity, *group );

  // Return the new group.
  return group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_assemblyStart ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT ( "In TrJt2Xml::_assemblyStart(), this = %X, entity = %X\n", this, entity );

  // Make a new assembly.
  DbXmlGroup::Ptr assembly = this->_createGroup ( "assembly", entity );
  if ( assembly.isNull() )
    return false;

  // Add the new assembly to the tree.
  _groupStack.back()->addChild ( assembly );

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

bool TrJt2Xml::_addPart ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT ( "In TrJt2Xml::_addPart(), this = %X, entity = %X\n", this, entity );

  // Make a new group for the part.
  DbXmlGroup::Ptr part = this->_createGroup ( "part", entity );
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

bool TrJt2Xml::_addLODs ( DbJtTraverser::EntityHandle entity, DbXmlGroup &part )
{
  SL_PRINT ( "In TrJt2Xml::_addLODs(), this = %X, entity = %X\n", this, entity );

  // Get the number of shape LODs.
  unsigned int numLODs ( 0 );
  if ( false == _jtTraverser->getNumLODs ( entity, numLODs ) )
    return false;

  // Should be true.
  SL_ASSERT ( numLODs > 0 );

  // Make a new group for the LODs.
  DbXmlGroup::Ptr lods = new DbXmlGroup ( "lods" );
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

bool TrJt2Xml::_addLOD ( DbJtTraverser::EntityHandle entity, 
                         const unsigned int &whichLOD, 
                         DbXmlGroup &lods )
{
  SL_PRINT ( "In TrJt2Xml::_addLOD(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );

  // Get the number of shapes for this LOD.
  unsigned int numShapes ( 0 );
  if ( false == _jtTraverser->getNumShapes ( entity, whichLOD, numShapes ) )
    return false;

  // Should be true.
  SL_ASSERT ( numShapes > 0 );

  // Make a new group for the LOD.
  DbXmlGroup::Ptr lod = new DbXmlGroup ( "lod" ); // Notice it's not "lods".
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

bool TrJt2Xml::_addShape ( DbJtTraverser::EntityHandle entity, 
                           const unsigned int &whichLOD, 
                           const unsigned int &whichShape, 
                           DbXmlGroup &lod )
{
  SL_PRINT ( "In TrJt2Xml::_addShape(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );

  // Get the number of sets for this shape.
  unsigned int numSets ( 0 );
  if ( false == _jtTraverser->getNumSets ( entity, whichLOD, whichShape, numSets ) )
    return false;

  // Should be true.
  SL_ASSERT ( numSets > 0 );

  // Make a new group for the shape.
  DbXmlGroup::Ptr shape = new DbXmlGroup ( "shape" );
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
    _error.format ( "Unknown shape type for entity = %X, name = %s, LOD = %d, shape = %d", entity, name.c_str(), whichLOD, whichShape );
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

bool TrJt2Xml::_addSet ( DbJtTraverser::EntityHandle entity, 
                         const unsigned int &whichLOD, 
                         const unsigned int &whichShape, 
                         const unsigned int &whichSet, 
                         const std::string &name,
                         DbXmlGroup &shape )
{
  SL_PRINT ( "In TrJt2Xml::_addSet(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( false == name.empty() );

  // Get the shape.
  std::vector<float> vertices, normals, colors, texture;
  unsigned int valid ( 0 );
  if ( false == _jtTraverser->getShape ( entity, whichLOD, whichShape, whichSet, vertices, normals, colors, texture, valid ) )
    return false;

  // Make a group for the set.
  DbXmlGroup::Ptr set = new DbXmlGroup ( name.c_str() );
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

bool TrJt2Xml::_addArray ( const unsigned int &valid, 
                           const unsigned int &which, 
                           const std::vector<float> &array, 
                           const char *arrayName, 
                           DbXmlGroup &set )
{
  SL_PRINT ( "In TrJt2Xml::_addArray(), this = %X, valid = %d, which = %d, array.size() = %d, arrayName = %s\n", this, valid, which, array.size(), arrayName );
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

void TrJt2Xml::_setArrayString ( const std::vector<float> &array, SlAString &s )
{
  SL_PRINT ( "In TrJt2Xml::_setArrayString(), this = %X, array.size() = %d\n", this, array.size() );
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

bool TrJt2Xml::_addInstance ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT ( "In TrJt2Xml::_addInstance(), this = %X, entity = %X\n", this, entity );

  // Make a new group for the part.
  DbXmlGroup::Ptr instance = this->_createGroup ( "instance", entity );
  if ( instance.isNull() )
    return false;

  // Add the new part to the tree.
  _groupStack.back()->addChild ( instance );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the XML tree level.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_endCurrentGroup()
{
  SL_PRINT ( "In TrJt2Xml::_endCurrentGroup(), this = %X\n", this );
  SL_ASSERT ( _groupStack.size() >= 2 );

  // Pop the group.
  _groupStack.pop_back();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the XML tree to file.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_write ( std::string &filename )
{
  SL_PRINT ( "In TrJt2Xml::_write(), this = %X, filename = %s\n", this, filename.c_str() );

  // Open the file.
  std::ofstream out ( filename.c_str() );
  if ( false == out.is_open() )
  {
    _error.format ( "Failed to open file '%s' for writing.", filename.c_str() );
    return false;
  }

//#ifdef _DEBUG
//  // Write only the names (no values) to the file.
//  _xmlWrite->setMode ( DbXmlWrite::WRITE_NAME_ONLY );
//#endif

  // Write the file.
  return _xmlWrite->write ( *_xmlRoot, out );
}

/*
TODO
- Figure out how to delete those arrays.
- Figure out when to return true and false. 
  Look at TrJt2Xml::_addArray().
- There are more properties at the LOD and shape level, add them.
*/
