
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtDatabase: Jupiter database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtDatabase.h"
#include "DbJtInline.h"

#include "Interfaces/IEntityNotify.h"
#include "Interfaces/IWarningNotify.h"
#include "Interfaces/IProgressNotify.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlQueryPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif

// This is the only way to get a pointer to the traverser from inside the 
// callback function.
namespace CadKit { DbJtDatabase *_traverser = NULL; }

// To help shorted up the lines.
#undef  ERROR
#define ERROR         this->_notifyError
#define PROGRESS      this->_notifyProgress
#define WARNING       this->_notifyWarning
#define FORMAT        CadKit::getString
#define UNKNOWN(ptr)  SlRefPtr<CadKit::IUnknown> ( ptr ? ptr->queryInterface ( CadKit::IUnknown::IID ) : NULL )
#define THIS_UNKNOWN  UNKNOWN ( this )

using namespace CadKit;

// These live in DbJtTraverser for now...
namespace CadKit
{
void _incrementPointerReferenceCount ( eaiEntity *p );
void _decrementPointerReferenceCount ( eaiEntity *p );
};

SL_IMPLEMENT_DYNAMIC_CLASS ( DbJtDatabase, SlRefBase );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbJtDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtDatabase::DbJtDatabase ( const unsigned int &customerId ) : SlRefBase ( 0 ),
  _target ( NULL ),
  _controller ( NULL ),
  _customerId ( customerId ),
  _initialized ( false ),
  _assemblyLoadOption ( INSTANCE_ASSEMBLY ),
  _brepLoadOption ( TESS_ONLY ),
  _shapeLoadOption ( ALL_LODS ),
  _assemblies ( new Assemblies ),
  _currentPart ( NULL ),
  _currentInstance ( NULL )
{
  SL_PRINT2 ( "In DbJtDatabase::DbJtDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _assemblies.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtDatabase::~DbJtDatabase()
{
  SL_PRINT2 ( "In DbJtDatabase::~DbJtDatabase(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbJtDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbJtDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IAssemblyQueryFloat::IID:
    return static_cast<IAssemblyQueryFloat *>(this);
  case IPartQueryFloat::IID:
    return static_cast<IPartQueryFloat *>(this);
  case IInstanceQueryFloat::IID:
    return static_cast<IInstanceQueryFloat *>(this);
  case IDataSource::IID:
    return static_cast<IDataSource *>(this);
  case IControlled::IID:
    return static_cast<IControlled *>(this);
  case CadKit::IUnknown::IID:
    return static_cast<CadKit::IUnknown *>(static_cast<IControlled *>(this));
  default:
    return NULL;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_init()
{
  SL_PRINT2 ( "In DbJtDatabase::_init(), this = %X\n", this );

  // Just return if we are already initialized.
  if ( true == _initialized )
    return true;

#if ( _DMDTK_VERSION > 4 )

  if ( false == PROGRESS ( "Initializing DirectModel Data Toolkit." ) )
    return false;

  // Initialize DMDTk.
  if ( eai_ERROR == eaiEntityFactory::init() )
  {
    ERROR ( FORMAT ( "Failed to initialize the DirectModel Data Toolkit." ) );
    return false;
  }

  if ( false == PROGRESS ( "Done initializing DirectModel Data Toolkit." ) )
    return false;

#endif // _DMDTK_VERSION

  // Get the customer number.
  const unsigned int &customer = this->_getCustomerId();

  if ( false == PROGRESS ( FORMAT ( "Attempting to register DirectModel Data Toolkit customer number %d.", customer ) ) )
    return false;

  // Register the customer.
  if ( eai_ERROR == eaiEntityFactory::registerCustomer ( customer ) )
  {
    ERROR ( FORMAT ( "Failed to register DirectModel Data Toolkit customer number: %d.", customer ), 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done registering DirectModel Data Toolkit customer." ) )
    return false;

  // We are now initialized.
  _initialized = true;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::loadData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbJtDatabase::loadData(), this = %X, name = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Try to traverse.
  try
  {
    // See if we need to initialize.
    if ( false == this->_init() )
      return false;

    // Traverse the database.
    return this->_traverse ( filename );
  }

  // Catch any exceptions.
  catch ( ... )
  {
    ERROR ( FORMAT ( "Exception generated when traversing '%s'", filename.c_str() ), 0 );
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the database.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_traverse ( const std::string &filename )
{
  SL_PRINT3 ( "In DbJtDatabase::_traverse(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Initialize.
//  _currentNode = NULL;
//  _currentLevel = 0;

  if ( false == PROGRESS ( "Creating CAD importer." ) )
    return false;

  // Declare a CAD importer. The returned instance has a zero reference count.
  // Assigning it to the SlRefPtr will increment it to one. When the SlRefPtr 
  // goes out of scope the internal pointer will be deferenced (back to zero),
  // and the importer will get deleted.
  SlRefPtr<eaiCADImporter> importer = eaiEntityFactory::createCADImporter();
  if ( importer.isNull() )
  {
    ERROR ( FORMAT ( "Failed to create CAD importer. Is your license set up correctly?" ), 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done creating CAD importer." ) )
    return false;

  // Set the options.
  importer->setShapeLoadOption ( (eaiCADImporter::ShapeLoadOption) _shapeLoadOption );
  importer->setBrepLoadOption ( (eaiCADImporter::BrepLoadOption) _brepLoadOption );
  importer->setAssemblyOption ( (eaiCADImporter::AssemblyOption) _assemblyLoadOption );

  if ( false == PROGRESS ( "Creating traverser." ) )
    return false;

  // Declare the traverser. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiTraverser> traverser = eaiEntityFactory::createTraverser();
  if ( traverser.isNull() )
  {
    ERROR ( "Failed to create database traverser.", 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done creating traverser.\nRegistering traversal callbacks." ) )
    return false;

  // Hook up the callback function.
  if ( eai_OK != traverser->setupPreActionCallback ( &DbJtDatabase::_preActionTraversalCallback ) )
  {
    ERROR ( "Failed to register pre-action traversal callback.", 0 );
    return false;
  }

  // And the other callback function too.
  if ( eai_OK != traverser->setupPostActionCallback ( &DbJtDatabase::_postActionTraversalCallback ) )
  {
    ERROR ( "Failed to register post-action traversal callback.", 0 );
    return false;
  }

  if ( false == PROGRESS ( FORMAT ( "Done registering traversal callbacks.\nImporting database: %s", filename.c_str() ) ) )
    return false;

  // Import the database. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiHierarchy> root = importer->import ( filename.c_str() );
  if ( root.isNull() )
  {
    ERROR ( FORMAT ( "Failed to import database: %s", filename.c_str() ), 0 );
    return false;
  }

  if ( false == PROGRESS ( FORMAT ( "Done importing database: %s\nStarting the traversal.", filename.c_str() ) ) )
    return false;

  // There doesn't appear to be a way to get a pointer to this instance inside 
  // the callback function. So I have to use this work-around.
  _traverser = this;

  // Traverse the database.
  if ( eai_OK != traverser->traverseGraph ( root ) )
  {
    ERROR ( "Failed to traverse database.", 0 );
    return false;
  }

  // Reset this.
  _traverser = NULL;

  if ( false == PROGRESS ( FORMAT ( "Done traversing: %s", filename.c_str() ) ) )
    return false;

  // TODO: It throws an exception when I allow this to be unref'd (and deleted).
  importer.makeNull();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal callback.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_preActionTraversalCallback ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalCallback(), hierarchy = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( _traverser );

  // Call the other one.
  return ( true == _traverser->_preActionTraversalNotify ( hierarchy, level ) ) ? eai_OK : eai_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal callback.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_postActionTraversalCallback ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalCallback(), hierarchy = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( _traverser );

  // Call the other one.
  return ( true == _traverser->_postActionTraversalNotify ( hierarchy, level ) ) ? eai_OK : eai_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal notify.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_preActionTraversalNotify ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalNotify(), hierarchy = %X, level = %d\n", hierarchy, level );

  // Check the level.
  if ( level < 0 )
  {
    SL_ASSERT ( 0 ); // Corrupt database?
    return ERROR ( FORMAT ( "Traverser reported eaiHierarchy at level: %2d, name: %s", level, hierarchy->name() ), 0 );
  }

  // Initialize.
  bool result ( false );

  // Determine the type.
  switch ( hierarchy->typeID() )
  {
  case eaiHierarchy::eaiASSEMBLY:

    // Save this assembly.
    this->_pushAssembly ( (eaiAssembly *) hierarchy );

    // Start the assembly.
    result = this->_startAssembly ( (unsigned int) level, (eaiAssembly *) hierarchy );
    break;

  case eaiHierarchy::eaiPART:

    // Save this part.
    this->_setCurrentPart ( (eaiPart *) hierarchy );

    // Start the part.
    result = this->_startPart ( (unsigned int) level, (eaiPart *) hierarchy );
    break;

  case eaiHierarchy::eaiINSTANCE:

    // Save this instance.
    this->_setCurrentInstance ( (eaiInstance *) hierarchy );

    // Start the instance.
    result = this->_startInstance ( (unsigned int) level, (eaiInstance *) hierarchy );
    break;

  default:

    SL_ASSERT ( 0 ); // What entity type is this?
    result = ERROR ( FORMAT ( "Unknown entity type %d, level %2d, name: %s", hierarchy->typeID(), level, hierarchy->name() ), 0 );
    break;
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal notify.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_postActionTraversalNotify ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalNotify(), hierarchy = %X, level = %d\n", hierarchy, level );

  // Check the level.
  if ( level < 0 )
  {
    SL_ASSERT ( 0 ); // Corrupt database?
    return ERROR ( FORMAT ( "Traverser reported eaiHierarchy at level: %2d, name: %s", level, hierarchy->name() ), 0 );
  }

  // Initialize.
  bool result ( false );

  // Determine the type.
  switch ( hierarchy->typeID() )
  {
  case eaiHierarchy::eaiASSEMBLY:

    // End the assembly.
    result = this->_endAssembly ( (unsigned int) level, (eaiAssembly *) hierarchy );

    // Done with this assembly.
    this->_popAssembly();
    break;

  case eaiHierarchy::eaiPART:

    // End the part.
    result = this->_endPart ( (unsigned int) level, (eaiPart *) hierarchy );

    // No more part.
    this->_setCurrentPart ( NULL );
    break;

  case eaiHierarchy::eaiINSTANCE:

    // End the instance.
    result = this->_endInstance ( (unsigned int) level, (eaiInstance *) hierarchy );

    // No more instance.
    this->_setCurrentInstance ( NULL );
    break;

  default:

    SL_ASSERT ( 0 ); // What entity type is this?
    result = ERROR ( FORMAT ( "Unknown entity type %d, level %2d, name: %s", hierarchy->typeID(), level, hierarchy->name() ), 0 );
    break;
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start an assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_startAssembly ( const unsigned int &level, eaiAssembly *entity )
{
  SL_PRINT3 ( "In DbJtDatabase::_startAssembly(), assembly = %X, level = %d\n", entity, level );

  if ( false == PROGRESS ( FORMAT ( "assembly, level %2d, name: %s", level, entity->name() ) ) )
    return false;

  // Get the controller's error handling interface.
  SlQueryPtr<IErrorNotify> controller ( IErrorNotify::IID, _controller );

  // Try this interface.
  SlQueryPtr<IAssemblyNotify> assemblyNotify ( IAssemblyNotify::IID, _target );
  if ( assemblyNotify.isValid() )
    return CadKit::handleEntityStart ( assemblyNotify.getValue(), (AssemblyHandle) entity, THIS_UNKNOWN, controller );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( IGroupNotify::IID, _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityStart ( groupNotify.getValue(), (GroupHandle) entity, THIS_UNKNOWN, controller );

  // If we get here then we couldn't find an appropriate interface.
  // We let the target decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to process assembly '%s' at level %d.\n\tNo known interface available from target.", entity->name(), level ), NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End an assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_endAssembly ( const unsigned int &level, eaiAssembly *entity )
{
  SL_PRINT3 ( "In DbJtDatabase::_endAssembly(), entity = %X, level = %d\n", entity, level );

  // Get the controller's error handling interface.
  SlQueryPtr<IErrorNotify> controller ( IErrorNotify::IID, _controller );

  // Try this interface.
  SlQueryPtr<IAssemblyNotify> assemblyNotify ( IAssemblyNotify::IID, _target );
  if ( assemblyNotify.isValid() )
    return CadKit::handleEntityEnd ( assemblyNotify.getValue(), (AssemblyHandle) entity, THIS_UNKNOWN, controller );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( IGroupNotify::IID, _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityEnd ( groupNotify.getValue(), (GroupHandle) entity, THIS_UNKNOWN, controller );

  // If we get here then we couldn't find an appropriate interface.
  // We let the target decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to process assembly '%s' at level %d.\n\tNo known interface available from target.", entity->name(), level ), CadKit::NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_startPart ( const unsigned int &level, eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_startPart(), hierarchy = %X, level = %d\n", hierarchy, level );

  return PROGRESS ( FORMAT ( "    part, level %2d, name: %s", level, part->name() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End a part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_endPart ( const unsigned int &level, eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_endPart(), hierarchy = %X, level = %d\n", hierarchy, level );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start an instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_startInstance ( const unsigned int &level, eaiInstance *instance )
{
  SL_PRINT3 ( "In DbJtDatabase::_startInstance(), hierarchy = %X, level = %d\n", hierarchy, level );

  return PROGRESS ( FORMAT ( "instance, level %2d, name: %s", level, instance->name() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End an instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_endInstance ( const unsigned int &level, eaiInstance *instance )
{
  SL_PRINT3 ( "In DbJtDatabase::_endInstance(), hierarchy = %X, level = %d\n", hierarchy, level );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data target.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setDataTarget ( IUnknown *target )
{
  SL_PRINT3 ( "In DbJtDatabase::setDataTarget(), this = %X, target = %X\n", this, target );

  // Set the target, it may be null.
  _target = target;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the controller.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setController ( IUnknown *controller )
{
  SL_PRINT3 ( "In DbJtDatabase::setController(), this = %X, controller = %X\n", this, controller );

  // Set the controller, it may be null.
  _controller = controller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the customer number.
//
///////////////////////////////////////////////////////////////////////////////

const unsigned int &DbJtDatabase::_getCustomerId()
{
  SL_PRINT2 ( "In DbJtDatabase::_getCustomerId(), this = %X\n", this );

  // Was it set by the client?
  if ( 0 != _customerId )
    return _customerId;

  // Try to get it from the environment.
  const char *temp = ::getenv ( "DMDTK_CUSTOMER_ID" );
  if ( temp ) 
    _customerId = ::atoi ( temp );

  // Return what we have.
  return _customerId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Error notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_notifyError ( const std::string &message, const unsigned long &id )
{
  SL_PRINT4 ( "In DbJtDatabase::_notifyError(), this = %X, id = %d, message = \n", this, id, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IErrorNotify> controller ( IErrorNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->errorNotify ( message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Progress notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_notifyProgress ( const std::string &message )
{
  SL_PRINT3 ( "In DbJtDatabase::_notifyProgress(), this = %X, message = \n", this, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IProgressNotify> controller ( IProgressNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->progressNotify ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Warning notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_notifyWarning ( const std::string &message, const unsigned long &id )
{
  SL_PRINT4 ( "In DbJtDatabase::_notifyWarning(), this = %X, id = %d, message = \n", this, id, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IWarningNotify> controller ( IWarningNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->warningNotify ( message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the assembly load option.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setAssemblyLoadOption ( const AssemblyLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setAssemblyLoadOption(), this = %X, option = %d\n", this, option );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiINSTANCE_ASSEMBLY == (unsigned int) INSTANCE_ASSEMBLY );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiEXPLODE_ASSEMBLY == (unsigned int) EXPLODE_ASSEMBLY );

  // Set the option.
  _assemblyLoadOption = option;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the brep load option.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setBrepLoadOption ( const BrepLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setBrepLoadOption(), this = %X, option = %d\n", this, option );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiTESS_ONLY == (unsigned int) TESS_ONLY );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiBREP_ONLY == (unsigned int) BREP_ONLY );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiTESS_AND_BREP == (unsigned int) ALL );

  // Set the option.
  _brepLoadOption = option;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shape load option.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setShapeLoadOption ( const ShapeLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setShapeLoadOption(), this = %X, option = %d\n", this, option );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiALL_LODS == (unsigned int) ALL_LODS );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiHIGH_LOD == (unsigned int) HIGH_LOD );

  // Set the option.
  _shapeLoadOption = option;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the assembly.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_pushAssembly ( eaiAssembly *assembly )
{
  SL_PRINT3 ( "In DbJtDatabase::_pushAssembly(), this = %X, assembly = %X\n", this, assembly );
  SL_ASSERT ( assembly );

  // Push it onto our stack.
  _assemblies->push ( assembly );

  // Reference it.
  assembly->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the assembly.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_popAssembly()
{
  SL_PRINT2 ( "In DbJtDatabase::_popAssembly(), this = %X\n", this );
  SL_ASSERT ( false == _assemblies->empty() );

  // Unreference the top one.
  _assemblies->top()->unref();

  // Pop it off of our stack.
  _assemblies->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current part.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_setCurrentPart ( eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_setCurrentPart(), this = %X, part = %X\n", this, part );

  // Release the old one if valid.
  if ( _currentPart )
    _currentPart->unref();

  // Set the new one.
  _currentPart = part;

  // Reference the new one if valid.
  if ( _currentPart )
    _currentPart->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current instance.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_setCurrentInstance ( eaiInstance *instance )
{
  SL_PRINT3 ( "In DbJtDatabase::_setCurrentInstance(), this = %X, instance = %X\n", this, instance );

  // Release the old one if valid.
  if ( _currentInstance )
    _currentInstance->unref();

  // Set the new one.
  _currentInstance = instance;

  // Reference the new one if valid.
  if ( _currentInstance )
    _currentInstance->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbJtDatabase::getName ( AssemblyHandle assembly ) const { return CadKit::getName ( assembly ); }
std::string DbJtDatabase::getName ( PartHandle part )         const { return CadKit::getName ( part ); }
std::string DbJtDatabase::getName ( InstanceHandle instance ) const { return CadKit::getName ( instance ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getTransform ( AssemblyHandle assembly, SlMatrix4f &matrix ) const { return CadKit::getTransform ( (eaiEntity *) assembly, matrix ); }
bool DbJtDatabase::getTransform ( PartHandle part,         SlMatrix4f &matrix ) const { return CadKit::getTransform ( (eaiEntity *) part,     matrix ); }
bool DbJtDatabase::getTransform ( InstanceHandle instance, SlMatrix4f &matrix ) const { return CadKit::getTransform ( (eaiEntity *) instance, matrix ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getMaterial ( AssemblyHandle assembly, SlMaterialf &material ) const { return CadKit::getMaterial ( (eaiEntity *) assembly, material ); }
bool DbJtDatabase::getMaterial ( PartHandle part,         SlMaterialf &material ) const { return CadKit::getMaterial ( (eaiEntity *) part,     material ); }
bool DbJtDatabase::getMaterial ( InstanceHandle instance, SlMaterialf &material ) const { return CadKit::getMaterial ( (eaiEntity *) instance, material ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the corresponding part or assembly.
//
///////////////////////////////////////////////////////////////////////////////

HierarchyHandle DbJtDatabase::getCorresponding ( InstanceHandle instance ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getCorrespondingPart(), this = %X, instance = %X\n", this, instance );
  SL_ASSERT ( instance );

  // It has to be an instance.
  if ( eaiEntity::eaiINSTANCE != ((eaiEntity *) instance)->typeID() )
    return NULL;

  // Return a pointer to the original part or assembly (which may be null).
  return (HierarchyHandle) ((eaiInstance *) instance)->original();
}
