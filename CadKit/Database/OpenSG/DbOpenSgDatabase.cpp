#include "DbOpenSgPrecompiled.h"
#include "DbOpenSgArraySetters.h"
#include "DbOpenSgDatabase.h"
#include "DbOpenSgFunctions.h"
#include "DbOpenSgInline.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IShapeQuery.h"

#include "Standard/SlAssert.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlPrint.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlStringFunctions.h"

#include <OpenSG/OSGDistanceLOD.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGTransform.h>

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

#define LAST_LOD_RANGE            1e7
#define MAX_LOD_DISTANCE_FACTOR   30

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbOpenSgDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbOpenSgDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOpenSgDatabase::DbOpenSgDatabase()
{
  SL_PRINT2 ( "In DbOpenSgDatabase::DbOpenSgDatabase(), this = %X\n", this );
  
  _groupStack.reset(new GroupStack);
  SL_ASSERT ( NULL != _groupStack.get() );
  
  osg::TransformPtr core = osg::Transform::create();
  SL_ASSERT ( osg::NullFC != core );    

  osg::NodePtr node = osg::Node::create();
  SL_ASSERT ( osg::NullFC != node );
  
  node->setCore(core);
  
  // Push a new group onto stack.  
  this->_pushGroup( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOpenSgDatabase::~DbOpenSgDatabase()
{
  SL_PRINT2 ( "In DbOpenSgDatabase::~DbOpenSgDatabase(), this = %X\n", this );

  // The stack should just be holding the root.
  SL_ASSERT ( 1 == _groupStack->size() );

  // The client may have referenced the root.
  //SL_ASSERT ( this->_getRoot()->referenceCount() >= 1 );

  // Pop the root off the stack.
  this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is about to receive data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOpenSgDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );

  // Clear the group stack.
  this->_clearGroupStack();

  osg::TransformPtr core = osg::Transform::create();
  if ( !core )
    return ERROR( "Failed to create osg::Transform for root.", CadKit::FAILED );
  
  osg::NodePtr node = osg::Node::create ();
  if ( !node )
    return ERROR( "Failed to create osg::Node for root.", CadKit::FAILED );
  
  node->setCore(core);
  
  // Push a group onto the stack.
  this->_pushGroup ( node );

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

bool DbOpenSgDatabase::dataTransferEnd ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOpenSgDatabase::dataTransferEnd(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );
  //SL_ASSERT ( this->_getRoot()->referenceCount() >= 1 );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the group stack.
//
///////////////////////////////////////////////////////////////////////////////

void DbOpenSgDatabase::_clearGroupStack()
{
  SL_PRINT2 ( "In DbOpenSgDatabase::_clearGroupStack(), this = %X\n", this );

  // Pop all the groups off the stack.
  while ( _groupStack->size() )
    this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbOpenSgDatabase::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbOpenSgDatabase::queryInterface(), this = %X\n", this );

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

std::string DbOpenSgDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbOpenSgDatabase::getFileExtension(), this = %X\n", this );
  return "osg";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the format have the attribute?
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::isAttributeSupported ( const FormatAttribute &attribute ) const
{
  SL_PRINT3 ( "In DbOpenSgDatabase::isAttributeSupported(), this = %X, attribute = %d\n", this, attribute );

  switch ( attribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return false;
  case FORMAT_ATTRIBUTE_ASCII:
    return true;
  default:
    SL_ASSERT ( 0 ); // What format is this?
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the output attribute.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::setOutputAttribute ( const FormatAttribute &attribute )
{
  SL_PRINT3 ( "In DbOpenSgDatabase::setOutputAttribute(), this = %X, attribute = %d\n", this, attribute );

  // There is only ascii output.
  return ( FORMAT_ATTRIBUTE_ASCII == attribute );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::writeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbOpenSgDatabase::writeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );
  SL_ASSERT ( osg::NullFC != _getRoot() );

  // Write the root to file.
  osg::NodePtr root = _getRoot();
  osg::beginEditCP(root);
  root->updateVolume();
  osg::endEditCP(root);
  return osg::SceneFileHandler::the().write( root, filename.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a Transform.
  osg::TransformPtr core = createGroup ( assembly, query.getValue(), osg::NullFC );
  if ( !core )
    return ERROR ( "Failed to create osg::Transform for assembly.", CadKit::FAILED );
    
  osg::NodePtr node = osg::Node::create ();
  if ( !node )
    return ERROR ( "Failed to create osg::Node for assembly.", CadKit::FAILED );
    
  node->setCore(core);

  // Add this group to the scene.
  _groupStack->top()->addChild ( node );

  // Make it the new current group.
  this->_pushGroup ( node );

  // Set the client data for this assembly.
  this->setClientData ( assembly, node.getCPtr() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
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

bool DbOpenSgDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a group (really a Transform).
  osg::TransformPtr transformcore = createGroup ( part, query.getValue(), osg::NullFC );
  if ( !transformcore )
    return ERROR ( "Failed to create osg::Transform for entity.", CadKit::FAILED );
    
  osg::NodePtr transformnode = osg::Node::create();
  if ( !transformnode )
    return ERROR ( "Failed to create osg::Node for entity.", CadKit::FAILED );
    
  transformnode->setCore(transformcore);

  // Create a LOD.
  osg::DistanceLODPtr lodcore = osg::DistanceLOD::create ();
  if ( !lodcore )
    return ERROR ( "Failed to create osg::LOD for given lod handle.", CadKit::FAILED );
    
  osg::NodePtr lodnode = osg::Node::create();
  if ( !lodnode )
    return ERROR ( "Failed to create osg::Node for given lod handle.", CadKit::FAILED );
    
  lodnode->setCore(lodcore);

  // Add this MatrixTransform to the scene.
  _groupStack->top()->addChild ( transformnode );

  // Add this LOD to the MatrixTransform.
  transformnode->addChild ( lodnode );

  // Set the client data for this part.
  this->setClientData ( part, transformnode.getCPtr() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );

  // Get the osg::Transform associated with the part.
  osg::NodePtr node((osg::Node*) ( this->getClientData ( part ) ));
  osg::TransformPtr group = osg::TransformPtr::dcast( node->getCore() );
  if ( !group )
    return ERROR ( "Failed to find group for part.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( 1 == node->getNChildren() );

  osg::NodePtr lodnode = node->getChild( 0 );
  if ( !lodnode )
    return ERROR ( "Failed to get LOD child of group.", CadKit::FAILED );
    
  // Get the lod from the group.
  osg::DistanceLODPtr lod = osg::DistanceLODPtr::dcast ( lodnode->getCore() );
  if ( !lod )
    return ERROR ( "Failed to find lod to set ranges for.", CadKit::FAILED );

  // Set the lod center and ranges.
  this->_setLodParameters ( lodnode );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod center and ranges.
//
///////////////////////////////////////////////////////////////////////////////

void DbOpenSgDatabase::_setLodParameters ( osg::NodePtr node ) const
{
  SL_PRINT3 ( "In DbOpenSgDatabase::_setLodParameters(), this = %X, lod = %X\n", this, lod );
  
  osg::DistanceLODPtr lod = osg::DistanceLODPtr::dcast ( node->getCore() );
  SL_ASSERT ( lod != osg::NullFC );

  // If there are no children then just return. Do not assert, this has 
  // happened with some big files.
  if ( 0 == node->getNChildren() )
    return;

  // Get the first child.
  osg::NodePtr child = node->getChild ( 0 );

  // Get the bounding sphere for the first child.
  child->updateVolume();
  osg::DynamicVolume dv = child->getVolume();
  dv.morphToType ( osg::DynamicVolume::SPHERE_VOLUME );
  
  osg::Volume &v = dv.getInstance();
  osg::SphereVolume *boundingSphere = dynamic_cast< osg::SphereVolume* > ( &v );
  if ( !boundingSphere )
    return;
  
  SL_VERIFY ( boundingSphere->getRadius() > 0.0f );

  // The maximum distance for the lod ranges.
  float maxDist = MAX_LOD_DISTANCE_FACTOR * boundingSphere->getRadius();

  // Get the center of the bounding sphere.
  const osg::Pnt3f &center = boundingSphere->getCenter();
  
  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  osg::MFReal32 &ranges = *lod->getMFRange();

  // Set the first range.
  ranges.addValue(0.0f);

  // Loop through all of the children, starting at the second one.
  unsigned int numChildren = node->getNChildren();
  for ( unsigned int i = 1; i < numChildren; ++i )
  {
    // Get the i'th child node.
    //child = node->getChild ( i );
    //SL_ASSERT ( child );

    // As of now, this should be true. However, it isn't important to the 
    // algorithm. Take this out if/when this is no longer true.
    //SL_ASSERT ( NULL != dynamic_cast<osg::Geode *> ( child ) );

    // Set the range.
    float dist = ( ( (float) i ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    ranges.addValue(dist);
  }

  // There should be one more range value than the number of children.
  ranges.addValue(LAST_LOD_RANGE);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Find the group associated with the corresponding part or assembly.
  osg::NodePtr group = this->_findGroup ( instance, query );
  if ( !group )
    return ERROR ( "Failed to find group associated with the corresponding part or assembly.", CadKit::FAILED );

  // Create a group (really a Transform) by making a shallow copy of 
  // the given group. The new group contains all the same children as the given group.
  osg::TransformPtr xform = CadKit::createGroup ( instance, query.getValue(), group );
  if ( !xform )
    return ERROR ( "Failed to create osg::Transform for instance.", CadKit::FAILED );
  
  osg::NodePtr mt = osg::Node::create();
  if ( !mt )
    return ERROR ( "Failed to create osg::Node for instance.", CadKit::FAILED );
    
  mt->setCore ( xform );

  // Add this MatrixTransform to the scene.
  _groupStack->top()->addChild ( mt );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::startEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::startEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a node to hold the Geometry.
  osg::NodePtr geomNode = osg::Node::create();
  if ( !geomNode )
    return ERROR ( "Failed to create osg::Node to hold geometry.", CadKit::FAILED );
  
  // Create the Geometry core.
  osg::GeometryPtr geom = osg::Geometry::create();
  if ( !geom )
    return ERROR ( "Failed to create osg::Geometry for given lod handle.", CadKit::FAILED );
    
  geomNode->setCore( geom );

  // Get the osg::Group associated with the lod's parent part.
  osg::NodePtr node ( (osg::Node*) ( this->getClientData ( query->getParent ( lod ) ) ) );
  osg::TransformPtr group = osg::TransformPtr::dcast ( node->getCore() );
  if ( !group )
    return ERROR ( "Failed to find lod to add geode to.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( 1 == node->getNChildren() );

  // The lod should be the only child of the group.
  osg::NodePtr lodnode = node->getChild ( 0 );
  if ( !lodnode )
    return ERROR ( "Failed to get lod child.", CadKit::FAILED );
  
  osg::DistanceLODPtr osgLod = osg::DistanceLODPtr::dcast ( lodnode->getCore() );
  if ( !osgLod )
    return ERROR ( "Failed to find lod to add geode to.", CadKit::FAILED );

  // Add this Geode to the LOD.
  lodnode->addChild ( geomNode );

  // Set the client data for this lod.
  this->setClientData ( lod, geomNode.getCPtr() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::startEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::startEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the osg::Geometry we need.
  osg::NodePtr node ( (osg::Node*) ( this->getClientData ( query->getParent ( shape ) ) ) );
  if ( !node )
    return ERROR ( "Failed to get osg::Geometry node from client data.", CadKit::FAILED );
  
  osg::GeometryPtr geom = osg::GeometryPtr::dcast ( node->getCore() );
  if ( !geom )
    return ERROR ( "Failed to find geometry to add shape geometry to.", CadKit::FAILED );
    
  node->setCore(geom);

  // Add the vertices, normals, etc. We have to call this before we add the 
  // attributes (like material) because in OSG, materials take precedence.
  if ( false == this->_addDataSets ( caller, shape, geom ) )
    return ERROR ( "Failed to add shape sets for given shape.", CadKit::FAILED );

  // Add the material, texture, etc.
  if ( false == this->_addAttributes ( caller, shape, geom ) )
    return ERROR ( "Failed to add shape sets for given shape.", CadKit::FAILED );

  // Set the state of the geometry.
  // XXX
  //geometry->setStateSet ( state );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's attributes to the given state.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, osg::GeometryPtr geometry )
{
  SL_PRINT5 ( "In DbOpenSgDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
  SL_ASSERT ( caller );
  SL_ASSERT ( 0 != geometry.getCPtr() );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( !query )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // If there are no color attributes then look for a material. We do this 
  // because OSG will use the material first.
  if ( false == this->_hasColorAttribute ( caller, shape ) )
  {
    // Get the material from the shape, part, or assembly.
    SlMaterialf material;
    if ( true == query->getMaterial ( shape, material, true ) )
      CadKit::setMaterial ( material, geometry );
    else
      CadKit::setDefaultMaterial ( geometry );
  }
  else
    CadKit::setDefaultMaterial ( geometry );

  // TODO, texture.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices, normals, etc, to the given geometry.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::_addDataSets ( IUnknown *caller, ShapeHandle shape, osg::GeometryPtr geometry )
{
  SL_PRINT5 ( "In DbOpenSgDatabase::_addDataSets(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( 0 != geometry.getCPtr() );
  
  // Add the vertices. If we don't add any then we failed.
  if ( false == this->_addVertices ( caller, shape, geometry ) )
    return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), CadKit::FAILED );

  // Add the normals, colors, and texture coordinates, if there are any.
  // It is ok if these fail.
  this->_addNormals   ( caller, shape, geometry );
  this->_addColors    ( caller, shape, geometry );
  this->_addTexCoords ( caller, shape, geometry );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::_addVertices ( IUnknown *caller, ShapeHandle shape, osg::GeometryPtr geometry )
{
  SL_PRINT5 ( "In DbOpenSgDatabase::_addVertices(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( 0 != geometry.getCPtr() );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeVerticesVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the primitive type.
  VertexSetType type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN != type );

  // Get the vertices.
  DbOpenSgVertexSetter setter ( type );
  if ( query->getVertices ( shape, setter ) )
  {
    osg::beginEditCP ( geometry );

    // Set the vertices and the primitives.
    geometry->setTypes ( setter.getTypes() );
    geometry->setLengths ( setter.getLengths() );
    geometry->setPositions ( setter.getPositions() );
    
    osg::endEditCP ( geometry );

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

bool DbOpenSgDatabase::_addNormals ( IUnknown *caller, ShapeHandle shape, osg::GeometryPtr geometry )
{
  SL_PRINT5 ( "In DbOpenSgDatabase::_addNormals(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( 0 != geometry.getCPtr() );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeNormalsVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the normal binding.
  VertexBinding binding;
  if ( false == query->getNormalBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain normal binding.", CadKit::FAILED );
    
  /// @todo IGNORING BINDING

  // Get the normals.
  DbOpenSgNormalSetter setter;
  if ( query->getNormals ( shape, setter ) )
  {
    osg::beginEditCP ( geometry );
    geometry->setNormals ( setter.getNormals() );
    osg::endEditCP ( geometry );
    
    // Set the normals and the binding.
    //geometry->setNormalArray ( setter.getNormals() );
    //geometry->setNormalBinding ( setter.getBinding() );

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

bool DbOpenSgDatabase::_addColors ( IUnknown *caller, ShapeHandle shape, osg::GeometryPtr geometry )
{
  SL_PRINT5 ( "In DbOpenSgDatabase::_addColors(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry.getCPtr() != 0 );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeColorsVec4f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain color binding.", CadKit::FAILED );

  /// @todo IGNORING BINDING

  // Get the colors.
  DbOpenSgColorSetter setter;
  if ( query->getColors ( shape, setter ) )
  {
    osg::beginEditCP ( geometry );
    geometry->setColors ( setter.getColors() );
    osg::endEditCP ( geometry );
  
    // Set the colors and the binding.
    //geometry->setColorArray ( setter.getColors() );
    //geometry->setColorBinding ( setter.getBinding() );

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

bool DbOpenSgDatabase::_addTexCoords ( IUnknown *caller, ShapeHandle shape, osg::GeometryPtr geometry )
{
  SL_PRINT5 ( "In DbOpenSgDatabase::_addTexCoords(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry.getCPtr() != 0 );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeTexCoordsVec2f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the texture coordinates.
  DbOpenSgTextureCoordSetter texCoords;
  if ( query->getTextureCoords ( shape, texCoords ) )
  {
    osg::beginEditCP ( geometry );
    geometry->setTexCoords ( texCoords.getTextureCoords() );
    osg::endEditCP ( geometry );
  
    // Set the texture coordinates.
    //geometry->setTexCoordArray ( 0, texCoords.getTextureCoords() );

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

bool DbOpenSgDatabase::_hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const
{
  SL_PRINT4 ( "In DbOpenSgDatabase::_hasColorAttribute(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
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

bool DbOpenSgDatabase::endEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::endEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::startEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::startEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // We ignore this because we build the geometry in the shape function above.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgDatabase::endEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOpenSgDatabase::endEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbOpenSgDatabase::_pushGroup ( osg::NodePtr group )
{
  SL_PRINT3 ( "In DbOpenSgDatabase::_pushGroup(), this = %X, group = %X\n", this, group );
  SL_ASSERT ( group.getCPtr() != 0 );

  // Push it onto our stack.
  _groupStack->push ( group );

  // Reference it.
  addRefCP(group);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

osg::NodePtr DbOpenSgDatabase::_getRoot() const
{
  SL_PRINT2 ( "In DbOpenSgDatabase::_getRoot(), this = %X\n", this );

  // If the stack is empty then the root is null.
  return ( _groupStack->size() ) ? _groupStack->top() : osg::NullFC;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbOpenSgDatabase::_popGroup()
{
  SL_PRINT2 ( "In DbOpenSgDatabase::_popGroup(), this = %X\n", this );
  SL_ASSERT ( false == _groupStack->empty() );

  // Unreference the top one.
  subRefCP(_groupStack->top());

  // Pop it off our stack.
  _groupStack->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the group associated with the instance's part or assembly.
//
///////////////////////////////////////////////////////////////////////////////

osg::NodePtr DbOpenSgDatabase::_findGroup ( InstanceHandle instance, IInstanceQueryFloat *query ) const
{
  SL_PRINT4 ( "In DbOpenSgDatabase::_findGroup(), this = %X, instance = %X, query = %X\n", this, instance, query );
  SL_ASSERT ( query );

  // See if this instance is a part.
  PartHandle part = query->getCorrespondingPart ( instance );
  if ( part )
    return osg::NodePtr ( (osg::Node *) ( this->getClientData ( part ) ) );

  // See if this instance is an assembly.
  AssemblyHandle assembly = query->getCorrespondingAssembly ( instance );
  if ( assembly )
    return osg::NodePtr ( (osg::Node *) ( this->getClientData ( assembly ) ) );

  // It didn't work.
  return osg::NullFC;
}
