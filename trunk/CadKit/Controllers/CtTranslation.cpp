
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  CtTranslation: Translation controller class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "CtPrecompiled.h"
#include "CtTranslation.h"

#include "Standard/SlPrint.h"
#include "Standard/SlPathname.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlMessageIds.h"

#include "Interfaces/IErrorNotify.h"
#include "Interfaces/IWarningNotify.h"
#include "Interfaces/IProgressNotify.h"
#include "Interfaces/IDataSource.h"
#include "Interfaces/IDataTarget.h"
#include "Interfaces/IControlled.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

#define PRINT if ( _out ) (*_out)
#define MIN_NUM_ARGS 2

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( CtTranslation, SlRefBase );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( CtTranslation, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CtTranslation::CtTranslation() : SlRefBase ( 0 ),
  _out ( NULL )
{
  SL_PRINT2 ( "In CtTranslation::CtTranslation(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CtTranslation::~CtTranslation()
{
  SL_PRINT2 ( "In CtTranslation::~CtTranslation(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::IUnknown *CtTranslation::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In CtTranslation::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IErrorNotify::IID:
    return static_cast<IErrorNotify *>(this);
  case IProgressNotify::IID:
    return static_cast<IProgressNotify *>(this);
  case IWarningNotify::IID:
    return static_cast<IWarningNotify *>(this);
  case ICommandLine::IID:
    return static_cast<ICommandLine *>(this);
  case ITranslator::IID:
    return static_cast<ITranslator *>(this);
  case IOutputStream::IID:
    return static_cast<IOutputStream *>(this);
  case CadKit::IUnknown::IID:
    return static_cast<CadKit::IUnknown *>(static_cast<IWarningNotify *>(this));
  default:
    return NULL;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the arguments.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::checkArguments ( const int &argc, const char **argv ) const
{
  SL_PRINT3 ( "In CtTranslation::checkArgumentCount(), this = %X, argc = %d\n", this, argc );
  SL_ASSERT ( argv );

  // Check the number of arguments.
  return ( argc < MIN_NUM_ARGS ) ? false : true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the usage string.
//
///////////////////////////////////////////////////////////////////////////////

std::string CtTranslation::getUsageString ( const std::string &program, const std::string &ext ) const
{
  SL_PRINT2 ( "In CtTranslation::getUsageString(), this = %X\n", this );

  // Format the usage string.
  std::string usage;
  CadKit::format ( usage, 
                   "Usage: %s [options] <filename1.%s> [filename2.%s] ...",
                   CadKit::justFilename ( program ).c_str(),
                   ext.c_str(),
                   ext.c_str() );
  usage += "\n\t-v, --verbose     Verbose output to stdout.";

  // Return the usage string.
  return usage;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the arguments.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::parseArguments ( const int &argc, const char **argv, CtTranslation::Args &args )
{
  SL_PRINT3 ( "In CtTranslation::parseArguments(), this = %X, argc = %d\n", this, argc );
  SL_ASSERT ( argc >= MIN_NUM_ARGS );
  SL_ASSERT ( argv );

  // Initialize.
  args.clear();

  // Get the arguments.
  for ( int i = 1; i < argc; ++i )
  {
    // Grab the current argument.
    std::string arg ( argv[i] );

    // See if this argument is the verbose flag.
    if ( arg == "-v" || arg == "--verbose" )
      this->setOutputStream ( &(std::cout) );

    // Otherwise just save the argument.
    else
      args.push_back ( argv[i] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the file.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::translate ( const std::string &filename, CadKit::IUnknown *source, CadKit::IUnknown *target )
{
  SL_PRINT5 ( "In CtTranslation::parseArguments(), this = %X, filename = %s, source = %X, target = %X\n", this, filename.c_str(), source, target );
  SL_ASSERT ( false == filename.empty() );

  // Make sure the source supports the interface we need.
  SlQueryPtr<IDataSource> ds ( IDataSource::IID, source );
  if ( ds.isNull() )
  {
    PRINT << "Failed to get needed interface from the data source." << std::endl;
    return false;
  }

  // Set the source's target.
  ds->setDataTarget ( target );

  // See if the source can be controlled.
  SlQueryPtr<IControlled> cs ( IControlled::IID, source );
  if ( cs.isValid() )
    cs->setController ( this->queryInterface ( CadKit::IUnknown::IID ) );

  // See if the target can be controlled.
  SlQueryPtr<IControlled> ct ( IControlled::IID, target );
  if ( ct.isValid() )
    ct->setController ( this->queryInterface ( CadKit::IUnknown::IID ) );

  // Load the data.
  if ( false == ds->loadData ( filename ) )
  {
    PRINT << "Failed to translate: " << filename.c_str() << std::endl;
    return false;
  }

  // See if the target should write a file.
  SlQueryPtr<IDataTarget> dt ( IDataTarget::IID, target );
  if ( dt.isValid() )
  {
    // Store the data.
    std::string outfile ( dt->getDefaultOutputName ( filename ) );
    if ( false == dt->storeData ( outfile ) )
    {
      PRINT << "Failed to write: " << outfile.c_str() << std::endl;
      return false;
    }
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of progress.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::progressNotify ( const std::string &message ) const
{
  SL_PRINT3 ( "In CtTranslation::errorNotify(), this = %X, message = %s\n", this, message.c_str() );

  // Print the error.
  PRINT << message.c_str() << std::endl;

  // Keep going.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of an error.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::errorNotify ( const std::string &message, const unsigned long &id ) const
{
  SL_PRINT4 ( "In CtTranslation::errorNotify(), this = %X, message = %s, id = %d\n", this, message.c_str(), id );
  return this->_messageNotify ( "Error", message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of a warning.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::warningNotify ( const std::string &message, const unsigned long &id ) const
{
  SL_PRINT4 ( "In CtTranslation::errorNotify(), this = %X, message = %s, id = %d\n", this, message.c_str(), id );
  return this->_messageNotify ( "Warning", message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of a message with an id.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::_messageNotify ( const std::string &type, const std::string &message, const unsigned long &id ) const
{
  SL_PRINT4 ( "In CtTranslation::_messageNotify(), this = %X, message = %s, id = %d\n", this, message.c_str(), id );

  // Decide what to do.
  switch ( id )
  {
  case CadKit::NO_INTERFACE:
    return true;
  default:
    PRINT << type.c_str() << " " << id << ": " << message.c_str() << std::endl;
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the output stream. Default is null.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::setOutputStream ( std::ostream *out )
{
  SL_PRINT3 ( "In CtTranslation::setOutputStream(), this = %X, out = %X\n", this, out );
  _out = out;
}
