
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
# include "Database/XML/DbXmlGroup.h"
# include "Database/XML/DbXmlLeaf.h"
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
# include "Standard/SlBitmask.h"
# include "Standard/SlStringFunctions.h"
# include <iostream>
# include <iomanip>
#endif

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TrJt2Xml::TrJt2Xml() :
  _verbose ( false ),
  _jtTraverser ( NULL ),
  _error ( "" )
{
  SL_PRINT2 ( "In TrJt2Xml::TrJt2Xml(), this = %X,\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TrJt2Xml::~TrJt2Xml()
{
  SL_PRINT2 ( "In TrJt2Xml::~TrJt2Xml(), this = %X,\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::init()
{
  SL_PRINT2 ( "In TrJt2Xml::init(), this = %X,\n", this );

  // Allocate.
  _jtTraverser = new DbJtTraverser ( DbJtTraverser::VERBOSE );
  if ( _jtTraverser.isNull() )
  {
    _error = "Failed to allocate memory.";
    return false;
  }

  // Initialize.
  if ( false == _jtTraverser->init() )
  {
    _error = "Failed to initialize jt traverser.";
    return false;
  }

  // Clear the error string.
  _error = "";

  // Set the callback function.
  _jtTraverser->setCallback ( &TrJt2Xml::_traverseCallback, this );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate. Note: we return a bool instead of a DbXmlGroup pointer because, 
//  this way if the translation fails part way through, the client can decide 
//  whether or not to use the partially formed XML tree. If we didn't return
//  a bool they would get all or nothing (a complete tree or NULL).
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::translate ( const char *filename, DbXmlGroup &root )
{
  SL_PRINT3 ( "In TrJt2Xml::translate(), this = %X, filename = %s\n", this, filename );
  SL_ASSERT ( _jtTraverser.isValid() );

  // Make sure we have just one group on the stack.
  _groupStack.clear();
  _groupStack.push_back ( &root );

  // Tell the traverser to traverse the database.
  if ( false == _jtTraverser->traverse ( filename ) )
  {
    _error = _jtTraverser->getLastError();
    return false;
  }

  // The database traversal stops when the last child is parsed, which may be 
  // deep in the heiarchy.
  SL_ASSERT ( _groupStack.size() >= 1 );
  SL_ASSERT ( &root == _groupStack.front() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse callback.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_traverseCallback ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData )
{
  SL_PRINT3 ( "In TrJt2Xml::_traverseCallback(), message = %d, clientData = %X\n", message, clientData );
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
  SL_PRINT3 ( "In TrJt2Xml::_traverseNotify(), message = %d, this = %X\n", message, this );
  SL_ASSERT ( this );

  // See what kind of message it is.
  switch ( message )
  {
  case DbJtTraverser::IMPORT_START:

    if ( _verbose )
      std::cout << "Importing... " << std::flush;
    break;

  case DbJtTraverser::IMPORT_FINISH:

    if ( _verbose )
      std::cout << "done" << std::endl;
    break;

  case DbJtTraverser::TRAVERSAL_START:

    if ( _verbose )
      std::cout << "Starting the traversal" << std::endl;
    break;

  case DbJtTraverser::TRAVERSAL_FINISH:

    if ( _verbose )
      std::cout << "Done traversing" << std::endl;
    break;

  case DbJtTraverser::LEVEL_PUSH:

    // Do nothing.
    break;

  case DbJtTraverser::LEVEL_POP:

    if ( _verbose )
      std::cout << "Done with assembly" << std::endl;

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
  SL_PRINT3 ( "In TrJt2Xml::_processEntity(), this = %X, entity = %X\n", this, entity );

  // Get the current entity type.
  DbJtTraverser::EntityType type;
  SL_VERIFY ( _jtTraverser->getType ( entity, type ) );

  // Process the entity types that we are interested in.
  if ( DbJtTraverser::PART == type )
  {
    // Add the part to the Performer tree.
    return this->_addPart ( entity );
  }
  
  else if ( DbJtTraverser::INSTANCE == type )
  {
    // Add the instance to the XML tree.
    return this->_addInstance ( entity );
  }

  else if ( DbJtTraverser::ASSEMBLY == type )
  {
    // Add the part to the Performer scene.
    return this->_assemblyStart ( entity );
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
  SL_PRINT3 ( "In TrJt2Xml::_addName(), this = %X, entity = %X\n", this, entity );
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
  SL_PRINT3 ( "In TrJt2Xml::_addTransform(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query the transformation, there may not be one.
  SlMatrix44f transform;
  if ( false == _jtTraverser->getTransform ( entity, transform ) )
    return false;

  // Write it on a single line, one row after the other.
  SlAString tempString;
  CadKit::format ( 
    tempString, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
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
  SL_PRINT3 ( "In TrJt2Xml::_addMaterial(), this = %X, entity = %X\n", this, entity );
  SL_ASSERT ( entity );

  // Query the material, there may not be one.
  SlMaterialf material;
  if ( false == _jtTraverser->getMaterial ( entity, material ) )
    return false;

  // Call the other one.
  return this->_addMaterial ( material, group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the material, if is is valid.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_addMaterial ( const SlMaterialf &m, DbXmlGroup &group )
{
  SL_PRINT3 ( "In TrJt2Xml::_addMaterial(), this = %X, m.getValid() = %d\n", this, m.getValid() );

  // Make a group for the material.
  DbXmlGroup::Ptr material = new DbXmlGroup ( "material" );
  if ( material.isNull() )
    return false;

  // Add the colors.
  this->_addColor ( m.getValid(), SlMaterialf::AMBIENT,  m.getAmbient(),  "ambient",  *material );
  this->_addColor ( m.getValid(), SlMaterialf::DIFFUSE,  m.getDiffuse(),  "diffuse",  *material );
  this->_addColor ( m.getValid(), SlMaterialf::SPECULAR, m.getSpecular(), "specular", *material );
  this->_addColor ( m.getValid(), SlMaterialf::EMISSIVE, m.getEmissive(), "emissive", *material );

  // Add the shininess.
  if ( m.isValid ( SlMaterialf::SHININESS ) )
  {
    // Add a leaf for the shininess.
    SlAString tempString;
    CadKit::format ( tempString, "%f", m.getShininess() );
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
  SL_PRINT5 ( "In TrJt2Xml::_addColor(), this = %X, valid = %d, which = %d, colorName = %s\n", this, valid, which, colorName );
  SL_ASSERT ( colorName );

  // See if the color is valid.
  if ( CadKit::hasBits ( valid, which ) )
  {
    // Add a leaf for the color.
    SlAString tempString;
    CadKit::format ( tempString, "%f %f %f %f", color[0], color[1], color[2], color[3] );
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

void TrJt2Xml::_printGroup ( DbXmlGroup &group )
{
  SL_PRINT2 ( "In TrJt2Xml::_printGroup(), this = %X\n", this );

  // Return if we aren't supposed to print anything.
  if ( false == _verbose )
    return;

  // Get the name of the group, if there is one. This is not the XML name 
  // from the name-value pair, this is the first child of <name>.
  std::string name;
  if ( group.getNumChildren() > 0 )
  {
    const DbXmlNode *node = group.getChild ( 0 );
    if ( node && node->isOfType ( DbXmlLeaf::getClassType() ) )
    {
      name = ((const DbXmlLeaf *) node)->getValue();
    }
  }

  // Print the info, even if "name" is blank.
  std::cout << std::setw ( 8 ) << group.getName() << ": " << name << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create an XML group node and fill in the common properties.
//
///////////////////////////////////////////////////////////////////////////////

DbXmlGroup *TrJt2Xml::_createGroup ( const char *groupName, DbJtTraverser::EntityHandle entity )
{
  SL_PRINT4 ( "In TrJt2Xml::_createGroup(), this = %X, groupName = %s, entity = %X\n", this, groupName, entity );
  SL_ASSERT ( groupName );

  // Make a new group.
  DbXmlGroup *group = new DbXmlGroup ( groupName );
  if ( NULL == group )
    return NULL;

  // Add some properties to the group.
  this->_addName      ( entity, *group );
  this->_addTransform ( entity, *group );
  this->_addMaterial  ( entity, *group );

  // Let the user know.
  this->_printGroup ( *group );

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
  SL_PRINT3 ( "In TrJt2Xml::_assemblyStart(), this = %X, entity = %X\n", this, entity );

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
  SL_PRINT3 ( "In TrJt2Xml::_addPart(), this = %X, entity = %X\n", this, entity );

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
//  has one or more LODs.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_addLODs ( DbJtTraverser::EntityHandle entity, DbXmlGroup &part )
{
  SL_PRINT3 ( "In TrJt2Xml::_addLODs(), this = %X, entity = %X\n", this, entity );

  // Get the number of shape LODs.
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
//  Add a LOD group (holding shapes) to the part.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_addLOD ( DbJtTraverser::EntityHandle entity, 
                         const unsigned int &whichLOD, 
                         DbXmlGroup &part )
{
  SL_PRINT4 ( "In TrJt2Xml::_addLOD(), this = %X, entity = %X, whichLOD = %d\n", this, entity, whichLOD );

  // Get the number of shapes for this LOD.
  unsigned int numShapes ( 0 );
  if ( false == _jtTraverser->getNumShapes ( entity, whichLOD, numShapes ) )
    return false;

  // Should be true.
  SL_ASSERT ( numShapes > 0 );

  // Make a new group for the LOD.
  DbXmlGroup::Ptr lod = new DbXmlGroup ( "lod" );
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
    part.addChild ( lod );
    
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
  SL_PRINT5 ( "In TrJt2Xml::_addShape(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );

  // Get the number of sets for this shape.
  unsigned int numSets ( 0 );
  if ( false == _jtTraverser->getNumShapeSets ( entity, whichLOD, whichShape, numSets ) )
    return false;

  // Should be true.
  SL_ASSERT ( numSets > 0 );

  // Make a new group for the shape.
  DbXmlGroup::Ptr shape = new DbXmlGroup ( "shape" );
  if ( shape.isNull() )
    return false;

  // Add the shape's material, if there is one.
  SlMaterialf material;
  if ( _jtTraverser->getMaterial ( entity, whichLOD, whichShape, material ) )
    this->_addMaterial ( material, *shape );

  // Determine the type of shape. Note: we don't want the given entity's type 
  // (it's most likely a part), we want the type of its shape.
  DbJtTraverser::EntityType type;
  if ( false == _jtTraverser->getShapeType ( entity, whichLOD, whichShape, type ) )
    return false;

  // Determine a proper name for the set.
  std::string name;
  if ( DbJtTraverser::TRI_STRIP_SET == type )
    name = "tri_strip";
  else if ( DbJtTraverser::POLYGON_SET == type )
    name = "polygon";
  else if ( DbJtTraverser::LINE_STRIP_SET == type )
    name = "line_strip";
  else if ( DbJtTraverser::POINT_SET == type )
    name = "point_set";
//  else if ( DbJtTraverser::TRI_FAN_SET == type )
//    name = "tri_fan";
  else
  {
    SL_ASSERT ( 0 ); // Heads up.
    CadKit::format ( _error, "Unknown shape type '%d' for entity = %X, name = %s, LOD = %d, shape = %d", type, entity, _jtTraverser->getName ( entity ).c_str(), whichLOD, whichShape );
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
  SL_PRINT5 ( "In TrJt2Xml::_addSet(), this = %X, entity = %X, whichLOD = %d, whichShape = %d\n", this, entity, whichLOD, whichShape );
  SL_ASSERT ( false == name.empty() );

  // Get the shape.
  std::vector<float> vertices, normals, colors, texture;
  unsigned int valid ( 0 );
  if ( false == _jtTraverser->getShapeSet ( entity, whichLOD, whichShape, whichSet, vertices, normals, colors, texture, valid ) )
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
  SL_PRINT6 ( "In TrJt2Xml::_addArray(), this = %X, valid = %d, which = %d, array.size() = %d, arrayName = %s\n", this, valid, which, array.size(), arrayName );
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
  SL_PRINT3 ( "In TrJt2Xml::_setArrayString(), this = %X, array.size() = %d\n", this, array.size() );
  SL_ASSERT ( false == array.empty() );

  // Make sure it's empty.
  s.clear();

  // Loop through the array (except the last one) and append to the string.
  SlAString temp;
  unsigned int stop ( array.size() - 1 ), i;
  for ( i = 0; i < stop; ++i )
  {
    // Format the temporary string.
    CadKit::format ( temp, "%f ", array[i] );

    // Append it to the "array string".
    s.append ( temp );
  }

  // Now append the last element (the format is slightly different).
  CadKit::format ( temp, "%f", array[i] );
  s.append ( temp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool TrJt2Xml::_addInstance ( DbJtTraverser::EntityHandle entity )
{
  SL_PRINT3 ( "In TrJt2Xml::_addInstance(), this = %X, entity = %X\n", this, entity );

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
  SL_PRINT2 ( "In TrJt2Xml::_endCurrentGroup(), this = %X\n", this );
  SL_ASSERT ( _groupStack.size() >= 2 );

  // Pop the group.
  _groupStack.pop_back();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the verbose flag. If true it will print progress notifications 
//  to stdout. Default is false.
//
///////////////////////////////////////////////////////////////////////////////

void TrJt2Xml::setVerbose ( bool verbose )
{
  SL_PRINT2 ( "In TrJt2Xml::setVerbose(), this = %X\n", this );
  SL_ASSERT ( _jtTraverser.isValid() );

  _jtTraverser->addFlags ( DbJtTraverser::VERBOSE );
  _verbose = true;
}

/*
TODO
- Figure out how to delete those arrays.
- Figure out when to return true and false. 
  Look at TrJt2Xml::_addArray().
- There are more properties at the LOD and shape level, add them.
*/
