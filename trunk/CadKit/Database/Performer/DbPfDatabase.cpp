
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfDatabase: Performer database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbPfPrecompiled.h"
#include "DbPfDatabase.h"
#include "DbPfFunctions.h"
#include "DbPfInline.h"
#include "DbPfArraySetters.h"
#include "DbPfPerformer.h"

#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IShapeQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryTexCoords.h"
#include "Interfaces/IClientData.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

#define LAST_LOD_RANGE            1e7
#define MAX_LOD_DISTANCE_FACTOR   30

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbPfDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbPfDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbPfDatabase::DbPfDatabase() : DbBaseTarget(),
  _groupStack ( new GroupStack ),
  _outputAttribute ( FORMAT_ATTRIBUTE_BINARY )
{
  SL_PRINT2 ( "In DbPfDatabase::DbPfDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groupStack.get() );

  // Push a new group onto stack.
  SlRefPtr<pfGroup> root ( new pfGroup );
  root->setName ( "root" );
  this->_pushGroup ( root );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbPfDatabase::~DbPfDatabase()
{
  SL_PRINT2 ( "In DbPfDatabase::~DbPfDatabase(), this = %X\n", this );

  // The stack should just be holding the root.
  SL_ASSERT ( 1 == _groupStack->size() );

  // The client may have referenced the root.
  SL_ASSERT ( this->_getRoot()->getRef() >= 1 );

  // Pop the root off the stack.
  this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is about to receive data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbPfDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );

  // Clear the group stack.
  this->_clearGroupStack();

  // Push a group onto the stack.
  SlRefPtr<pfGroup> root ( new pfGroup );
  root->setName ( "root" );
  this->_pushGroup ( root );

  // Clear the client-data maps.
  this->_clearClientDataMaps();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is done receiving data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::dataTransferEnd ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbPfDatabase::dataTransferEnd(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );
  SL_ASSERT ( this->_getRoot()->getRef() >= 1 );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the group stack.
//
///////////////////////////////////////////////////////////////////////////////

void DbPfDatabase::_clearGroupStack()
{
  SL_PRINT2 ( "In DbPfDatabase::_clearGroupStack(), this = %X\n", this );

  // Pop all the groups off the stack.
  while ( _groupStack->size() )
    this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbPfDatabase::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbPfDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IAssemblyNotify::IID:
    return static_cast<IAssemblyNotify *>(this);
  case IPartNotify::IID:
    return static_cast<IPartNotify *>(this);
  case IInstanceNotify::IID:
    return static_cast<IInstanceNotify *>(this);
  case ILodNotify::IID:
    return static_cast<ILodNotify *>(this);
  case IShapeNotify::IID:
    return static_cast<IShapeNotify *>(this);
  case ISetNotify::IID:
    return static_cast<ISetNotify *>(this);
  case IFileExtension::IID:
    return static_cast<IFileExtension *>(this);
  case IDataWrite::IID:
    return static_cast<IDataWrite *>(this);
  case IOutputAttribute::IID:
    return static_cast<IOutputAttribute *>(this);
  default:
    return DbBaseTarget::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file extension.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbPfDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbPfDatabase::getFileExtension(), this = %X\n", this );

  switch ( _outputAttribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return "pfb";
  case FORMAT_ATTRIBUTE_ASCII:
    return "pfa";
  default:
    SL_ASSERT ( 0 ); // What attribute is this?
    return "pfb";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the format have the attribute?
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::isAttributeSupported ( const FormatAttribute &attribute ) const
{
  SL_PRINT3 ( "In DbPfDatabase::isAttributeSupported(), this = %X, attribute = %d\n", this, attribute );

  switch ( attribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return true;
  case FORMAT_ATTRIBUTE_ASCII:
    return true;
  default:
    SL_ASSERT ( 0 ); // What attribute is this?
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the output attribute.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::setOutputAttribute ( const FormatAttribute &attribute )
{
  SL_PRINT3 ( "In DbPfDatabase::setOutputAttribute(), this = %X, attribute = %d\n", this, attribute );

  // Is it supported?
  if ( false == this->isAttributeSupported ( attribute ) )
    return false;

  // Set the attribute.
  _outputAttribute = attribute;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::writeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbPfDatabase::writeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );
  SL_ASSERT ( NULL != this->_getRoot() );

#if 0
#ifdef _DEBUG
  std::string dump ( filename );
  dump.append ( ".out" );
  FILE *out = ::fopen ( dump.c_str(), "w" );
  ::printf ( "Dumping debug file: %s\n", dump.c_str() );
  ::pfPrint ( this->_getRoot(), PFTRAV_SELF | PFTRAV_DESCEND, PFPRINT_VB_DEBUG, out );
  ::fclose ( out );
#endif
#endif

  // Write the root to file. Note: Performer will write binary or ascii 
  // depending on the extension.
  return ( 0 != ::pfdStoreFile ( this->_getRoot(), filename.c_str() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a DCS).
  SlRefPtr<pfGroup> mt = CadKit::createGroup ( assembly, query.getValue(), (pfGroup *) NULL );
  if ( mt.isNull() )
    return ERROR ( "Failed to create pfDCS for assembly.", CadKit::FAILED );

  // Add this group to the scene.
  _groupStack->top()->addChild ( mt );

  // Make it the new current group.
  this->_pushGroup ( mt );

  // Set the client data for this assembly.
  this->setClientData ( assembly, mt.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );

  // Done with this group.
  this->_popGroup();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a DCS).
  SlRefPtr<pfGroup> mt = CadKit::createGroup ( part, query.getValue(), (pfGroup *) NULL );
  if ( mt.isNull() )
    return ERROR ( "Failed to create pfDCS for assembly.", CadKit::FAILED );

  // Create a LOD.
  SlRefPtr<pfLOD> lod ( new pfLOD );
  if ( lod.isNull() )
    return ERROR ( "Failed to create pfLOD for given lod handle.", CadKit::FAILED );

  // Give it a name.
  lod->setName ( mt->getName() );

  // Add this DCS to the scene.
  _groupStack->top()->addChild ( mt );

  // Add this LOD to the DCS.
  mt->addChild ( lod );

  // Set the client data for this part.
  this->setClientData ( part, mt.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );

  // Get the pfGroup associated with the part.
  SlRefPtr<pfObject> object ( (pfObject *) ( this->getClientData ( part ) ) );
  SlRefPtr<pfGroup> group ( dynamic_cast<pfGroup *> ( object.getValue() ) );
  if ( group.isNull() )
    return ERROR ( "Failed to find group for part.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( 1 == group->getNumChildren() );

  // Get the lod from the group.
  SlRefPtr<pfLOD> lod ( dynamic_cast<pfLOD *> ( group->getChild ( 0 ) ) );
  if ( lod.isNull() )
    return ERROR ( "Failed to find lod to set ranges for.", CadKit::FAILED );

  // Set the lod center and ranges.
  this->_setLodParameters ( lod );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod center and ranges.
//
///////////////////////////////////////////////////////////////////////////////

void DbPfDatabase::_setLodParameters ( pfLOD *lod ) const
{
  SL_PRINT3 ( "In DbPfDatabase::_setLodParameters(), this = %X, lod = %X\n", this, lod );

  // If there are no children then just return. Do not assert, this has 
  // happened with some big files.
  if ( 0 == lod->getNumChildren() )
    return;

  // Get the first child.
  pfNode *child = lod->getChild ( 0 );

  // Get the bounding sphere for the first child.
  pfSphere boundingSphere;
  child->getBound ( &boundingSphere ); // Returns mode, not error.
  SL_VERIFY ( boundingSphere.radius > 0.0f );

  // The maximum distance for the lod ranges.
  float maxDist = MAX_LOD_DISTANCE_FACTOR * boundingSphere.radius;

  // Get the center of the bounding sphere.
  pfVec3 &center = boundingSphere.center;

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  // Set the first range.
  lod->setRange ( 0, 0.0f );

  // Loop through all of the children, starting at the second one.
  unsigned int numChildren = lod->getNumChildren();
  for ( unsigned int i = 1; i < numChildren; ++i )
  {
    // Set the range.
    float dist = ( ( (float) i ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    lod->setRange ( i, dist );
  }

  // There should be one more range value than the number of children.
  lod->setRange ( numChildren, LAST_LOD_RANGE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Find the group associated with the corresponding part or assembly.
  pfGroup *group = this->_findGroup ( instance, query );
  if ( NULL == group )
    return ERROR ( "Failed to find group associated with the corresponding part or assembly.", CadKit::FAILED );

  // Create a group (really a DCS) by making a shallow copy of the given 
  // group. The new group contains all the same children as the given group.
  SlRefPtr<pfGroup> mt = CadKit::createGroup ( instance, query.getValue(), group );
  if ( mt.isNull() )
    return ERROR ( "Failed to create pfDCS for instance.", CadKit::FAILED );

  // Add this DCS to the scene.
  _groupStack->top()->addChild ( mt );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::startEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::startEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a Geode.
  SlRefPtr<pfGeode> geode ( new pfGeode );
  if ( geode.isNull() )
    return ERROR ( "Failed to create pfGeode for given lod handle.", CadKit::FAILED );

  // Get the pfGroup associated with the lod's parent part.
  SlRefPtr<pfObject> object ( (pfObject *) ( this->getClientData ( query->getParent ( lod ) ) ) );
  SlRefPtr<pfGroup> group ( dynamic_cast<pfGroup *> ( object.getValue() ) );
  if ( group.isNull() )
    return ERROR ( "Failed to find lod to add geode to.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( 1 == group->getNumChildren() );

  // Give the geode a name.
  geode->setName ( FORMAT ( "Part '%s', LOD %d", group->getName(), lod ).c_str() );

  // The lod should be the only child of the group.
  SlRefPtr<pfLOD> pfLod ( dynamic_cast<pfLOD *> ( group->getChild ( 0 ) ) );
  if ( pfLod.isNull() )
    return ERROR ( "Failed to find lod to add geode to.", CadKit::FAILED );

  // Add this Geode to the LOD.
  pfLod->addChild ( geode );

  // Set the client data for this lod.
  this->setClientData ( lod, geode.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::startEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::startEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the pfGeode we need.
  SlRefPtr<pfObject> object ( (pfObject *) ( this->getClientData ( query->getParent ( shape ) ) ) );
  SlRefPtr<pfGeode> geode ( dynamic_cast<pfGeode *> ( object.getValue() ) );
  if ( geode.isNull() )
    return ERROR ( "Failed to find geode to add shape geometry to.", CadKit::FAILED );

  // Create a GeoSet.
  SlRefPtr<pfGeoSet> gset ( new pfGeoSet );
  if ( gset.isNull() )
    return ERROR ( "Failed to create pfGeoSet for given shape handle.", CadKit::FAILED );

  // Create a StateSet.
  SlRefPtr<pfGeoState> state ( new pfGeoState );
  if ( state.isNull() )
    return ERROR ( "Failed to create pfGeoState for given shape handle.", CadKit::FAILED );

  // Add the vertices, normals, etc. We have to call this before we add the 
  // attributes (like material) because in Performer, materials take precedence.
  if ( false == this->_addDataSets ( caller, shape, gset ) )
    return ERROR ( "Failed to add shape sets for given shape.", CadKit::FAILED );

  // Add the material, texture, etc.
  if ( false == this->_addAttributes ( caller, shape, state ) )
    return ERROR ( "Failed to add shape sets for given shape.", CadKit::FAILED );

  // Set the state of the GeoSet.
  gset->setGState ( state );

  // Add the GeoSet to the Geode.
  geode->addGSet ( gset );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's attributes to the given state.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, pfGeoState *state )
{
  SL_PRINT5 ( "In DbPfDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
  SL_ASSERT ( caller );
  SL_ASSERT ( state );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // If there are no color attributes then look for a material.
  // We have to do this for OSG because it uses materials first (even if 
  // there are colors). However, do we have to do this for Performer?
  if ( false == this->_hasColorAttribute ( caller, shape ) )
  {
    // Get the material from the shape, part, or assembly.
    SlMaterialf material;
    if ( true == query->getMaterial ( shape, material, true ) )
      CadKit::setMaterial ( material, state );
  }

  // TODO, texture.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices, normals, etc, to the given GeoSet.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_addDataSets ( IUnknown *caller, ShapeHandle shape, pfGeoSet *gset )
{
  SL_PRINT5 ( "In DbPfDatabase::_addDataSets(), this = %X, caller = %X, shape = %d, gset = %X\n", this, caller, shape, gset );
  SL_ASSERT ( caller );
  SL_ASSERT ( gset );
  
  // Add the vertices. If we don't add any then we failed.
  if ( false == this->_addVertices ( caller, shape, gset ) )
    return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), CadKit::FAILED );

  // Add the normals, colors, and texture coordinates, if there are any.
  // It is ok if these fail.
  this->_addNormals   ( caller, shape, gset );
  this->_addColors    ( caller, shape, gset );
  this->_addTexCoords ( caller, shape, gset );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_addVertices ( IUnknown *caller, ShapeHandle shape, pfGeoSet *gset )
{
  SL_PRINT5 ( "In DbPfDatabase::_addVertices(), this = %X, caller = %X, shape = %d, gset = %X\n", this, caller, shape, gset );
  SL_ASSERT ( caller );
  SL_ASSERT ( gset );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeVerticesVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the primitive type.
  VertexSetType type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN != type );

  // Get the vertices.
  DbPfVertexSetter setter ( type );
  if ( query->getVertices ( shape, setter ) )
  {
    // Get the primitive lengths.
    int *lengths = setter.getPrimitiveLengths();
    if ( NULL == lengths )
      return ERROR ( "Failed to determine lengths of vertex-set primitives.", CadKit::FAILED );

    // Set the vertices, the primitive lengths, and the primitive type.
    gset->setPrimType ( setter.getPrimitiveType() );
    gset->setNumPrims ( setter.getNumPrimitives() );
    gset->setPrimLengths ( lengths );
    gset->setAttr ( PFGS_COORD3, PFGS_PER_VERTEX, setter.getVertices(), NULL );

    // It worked.
    return true;
  }

  // It didn't work.
  return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), CadKit::FAILED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's normals.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_addNormals ( IUnknown *caller, ShapeHandle shape, pfGeoSet *gset )
{
  SL_PRINT5 ( "In DbPfDatabase::_addNormals(), this = %X, caller = %X, shape = %d, gset = %X\n", this, caller, shape, gset );
  SL_ASSERT ( caller );
  SL_ASSERT ( gset );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeNormalsVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the normal binding.
  VertexBinding binding;
  if ( false == query->getNormalBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain normal binding.", CadKit::FAILED );

  // Get the normals.
  DbPfNormalSetter setter ( binding );
  if ( query->getNormals ( shape, setter ) )
  {
    // Set the normals and the binding.
    gset->setAttr ( PFGS_NORMAL3, setter.getBinding(), setter.getNormals(), NULL );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's colors.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_addColors ( IUnknown *caller, ShapeHandle shape, pfGeoSet *gset )
{
  SL_PRINT5 ( "In DbPfDatabase::_addColors(), this = %X, caller = %X, shape = %d, gset = %X\n", this, caller, shape, gset );
  SL_ASSERT ( caller );
  SL_ASSERT ( gset );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeColorsVec4f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain color binding.", CadKit::FAILED );

  // Get the colors.
  DbPfColorSetter setter ( binding );
  if ( query->getColors ( shape, setter ) )
  {
    // Set the colors and the binding.
    gset->setAttr ( PFGS_COLOR4, setter.getBinding(), setter.getColors(), NULL );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_addTexCoords ( IUnknown *caller, ShapeHandle shape, pfGeoSet *gset )
{
  SL_PRINT5 ( "In DbPfDatabase::_addTexCoords(), this = %X, caller = %X, shape = %d, gset = %X\n", this, caller, shape, gset );
  SL_ASSERT ( caller );
  SL_ASSERT ( gset );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeTexCoordsVec2f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the texture coordinates.
  DbPfTextureCoordSetter setter;
  if ( query->getTextureCoords ( shape, setter ) )
  {
    // Set the texture coordinates.
    gset->setAttr ( PFGS_TEXCOORD2, setter.getBinding(), setter.getTextureCoords(), NULL );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given shape has any color attributes.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::_hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const
{
  SL_PRINT4 ( "In DbPfDatabase::_hasColorAttribute(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeColorsVec4f> query ( caller );
  if ( query.isNull() )
    return false;

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return false;

  // Does it have colors?
  return BINDING_OFF != binding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::endEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::endEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::startEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::startEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // We ignore this because we build the geometry in the shape function above.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfDatabase::endEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbPfDatabase::endEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbPfDatabase::_pushGroup ( pfGroup *group )
{
  SL_PRINT3 ( "In DbPfDatabase::_pushGroup(), this = %X, group = %X\n", this, group );
  SL_ASSERT ( group );

  // Push it onto our stack.
  _groupStack->push ( group );

  // Reference it.
  group->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

pfGroup *DbPfDatabase::_getRoot() const
{
  SL_PRINT2 ( "In DbPfDatabase::_getRoot(), this = %X\n", this );

  // If the stack is empty then the root is null.
  return ( _groupStack->size() ) ? _groupStack->top() : NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbPfDatabase::_popGroup()
{
  SL_PRINT2 ( "In DbPfDatabase::_popGroup(), this = %X\n", this );
  SL_ASSERT ( false == _groupStack->empty() );

  // Unreference the top one.
  _groupStack->top()->unref();

  // Pop it off our stack.
  _groupStack->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the group associated with the instance's part or assembly.
//
///////////////////////////////////////////////////////////////////////////////

pfGroup *DbPfDatabase::_findGroup ( InstanceHandle instance, IInstanceQueryFloat *query ) const
{
  SL_PRINT4 ( "In DbPfDatabase::_findGroup(), this = %X, instance = %X, query = %X\n", this, instance, query );
  SL_ASSERT ( query );

  // See if this instance is a part.
  PartHandle part = query->getCorrespondingPart ( instance );
  if ( NULL != part )
    return dynamic_cast<pfGroup *> ( (pfObject *) ( this->getClientData ( part ) ) );

  // See if this instance is an assembly.
  AssemblyHandle assembly = query->getCorrespondingAssembly ( instance );
  if ( NULL != assembly )
    return dynamic_cast<pfGroup *> ( (pfObject *) ( this->getClientData ( assembly ) ) );

  // It didn't work.
  return NULL;
}
