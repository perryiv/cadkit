
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
#include "Standard/SlStringFunctions.h"
#include "Standard/SlBitmask.h"

#include "Interfaces/IDataSource.h"
#include "Interfaces/IDataTarget.h"
#include "Interfaces/IControlled.h"
#include "Interfaces/IMessagePriority.h"
#include "Interfaces/ILodOption.h"
#include "Interfaces/IZeroRange.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <time.h>
#endif

#define PRINT if ( _out ) (*_out)
#define MIN_NUM_ARGS 2
#define NEGATIVE_ZERO_INDEX 0
#define POSITIVE_ZERO_INDEX 1

// Don't bother formatting the string if the controller won't print it.
#define PRINT_LEVEL(priority)\
  if ( priority <= _progressPrintLevel )\
    (*_out)

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( CtTranslation, SlRefBase );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( CtTranslation, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CtTranslation::CtTranslation() : SlRefBase ( 0 ),
  _out ( NULL ),
  _progressPrintLevel ( 0 ),
  _printFlags ( 0 ),
  _lodOption ( CadKit::PROCESS_ALL_LODS ),
  _zeroRange ( 0.0, 0.0 )
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
  case IMessageNotify::IID:
    return static_cast<IMessageNotify *>(this);
  case ICommandLine::IID:
    return static_cast<ICommandLine *>(this);
  case ITranslator::IID:
    return static_cast<ITranslator *>(this);
  case IOutputStream::IID:
    return static_cast<IOutputStream *>(this);
  case CadKit::IUnknown::IID:
    return static_cast<CadKit::IUnknown *>(static_cast<IMessageNotify *>(this));
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
//  Options string, for the "usage" output.
//
///////////////////////////////////////////////////////////////////////////////

#define _COMMAND_LINE_OPTIONS "\n\
  -pp <priority>    Print to stdout any progress messages with a priority\n\
                    level less than or equal to 'priority'.\n\
  -pe               Print error messages to stdout.\n\
  -pw               Print warning messages to stdout.\n\
  -pi               Print general information messages to stdout.\n\
  -v                Verbose output to stdout, same as '-pe -pw -pi -pp 1'.\n\
  -al               Where applicable, translate all of the Levels-of-Detail (LODs)'.\n\
  -hl               Where applicable, translate only the highest Level-of-Detail (LOD)'.\n\
  -ll               Where applicable, translate only the lowest Level-of-Detail (LOD)'.\n\
  -z <low> <high>   Any number in the range [low,high] is set to 0.\n\
  --print-progress  Same as '-pp'.\n\
  --print-errors    Same as '-pe'.\n\
  --print-warnings  Same as '-pw'.\n\
  --print-info      Same as '-pi'.\n\
  --verbose         Same as '-v'.\n\
  --all-lods        Same as '-al'.\n\
  --high-lod        Same as '-hl'.\n\
  --low-lod         Same as '-ll'.\n\
  --zero            Same as '-z'."


//  -ea <action>      What to do if an error is encountered. Possible actions:\n\
//                      exit:     Exit the program.\n\
//                      continue: Continue executing is possible.\n\
//  -wa <action>      What to do if a warning is encountered. Possible actions:\n\
//                      exit:     Exit the program.\n\
//                      continue: Continue executing is possible.\n\
//  --error-action    Same as '-ea'.\n\
//  --warning-action  Same as '-wa'.\n\


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
  usage += _COMMAND_LINE_OPTIONS;

  // Return the usage string.
  return usage;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the arguments.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::parseArguments ( const int &argc, const char **argv, CtTranslation::Args &args )
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

    //
    // See if this argument is one of our flags.
    //

    if ( arg == "-pp" || arg == "--print-progress" )
    {
      // Get the next argument, if there is one.
      std::string option ( ( i + 1 == argc ) ? "" : argv[i+1] );

      // See if the option string is an integer.
      if ( true == CadKit::isUnsignedInteger ( option ) )
      {
        this->setOutputStream ( &(std::cout) );
        this->_setProgressPrintLevel ( CadKit::toUnsignedInteger ( option ) );

        // Increment the loop index.
        ++i;
      }

      // Otherwise return false.
      else
        return false;
    }

    else if ( arg == "-pe" || arg == "--print-errors" )
    {
      this->setOutputStream ( &(std::cout) );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_ERRORS );
    }

    else if ( arg == "-pw" || arg == "--print-warnings" )
    {
      this->setOutputStream ( &(std::cout) );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_WARNINGS );
    }

    else if ( arg == "-pi" || arg == "--print-info" )
    {
      this->setOutputStream ( &(std::cout) );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_INFO );
    }

    else if ( arg == "-v" || arg == "--verbose" )
    {
      this->setOutputStream ( &(std::cout) );
      this->_setProgressPrintLevel ( 1 );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_ERRORS );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_WARNINGS );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_INFO );
    }

    else if ( arg == "-al" || arg == "--all-lods" )
    {
      _lodOption = CadKit::PROCESS_ALL_LODS;
    }

    else if ( arg == "-hl" || arg == "--high-lod" )
    {
      _lodOption = CadKit::PROCESS_HIGH_LOD;
    }

    else if ( arg == "-ll" || arg == "--low-lod" )
    {
      _lodOption = CadKit::PROCESS_LOW_LOD;
    }

    else if ( arg == "-z" || arg == "--zero" )
    {
      // Get the next two arguments, if there are any.
      std::string negative  ( ( i + 1 == argc ) ? "" : argv[i+1] );
      std::string positive ( ( i + 2 == argc ) ? "" : argv[i+2] );

      // See if the strings are a numbers.
      if ( true == CadKit::isNumber ( negative ) && true == CadKit::isNumber ( positive ) )
      {
        // Set the zero value.
        _zeroRange[NEGATIVE_ZERO_INDEX] = ::atof ( negative.c_str() );
        _zeroRange[POSITIVE_ZERO_INDEX] = ::atof ( positive.c_str() );

        // Increment the loop index.
        i += 2;
      }

      // Otherwise return false.
      else
        return false;
    }

    // Otherwise just save the argument.
    else
      args.push_back ( argv[i] );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the file.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::translate ( const std::string &filename, CadKit::IUnknown *source, CadKit::IUnknown *target )
{
  SL_PRINT5 ( "In CtTranslation::translate(), this = %X, filename = %s, source = %X, target = %X\n", this, filename.c_str(), source, target );
  SL_ASSERT ( false == filename.empty() );

  // See if we can tell the target the priority level.
  SlQueryPtr<IMessagePriority> targetMessagePriority ( target );
  if ( targetMessagePriority.isValid() )
    targetMessagePriority->setMessagePriorityLevel ( CadKit::MESSAGE_PROGRESS, _progressPrintLevel );

  // See if we can tell the source the priority level.
  SlQueryPtr<IMessagePriority> sourceMessagePriority ( source );
  if ( sourceMessagePriority.isValid() )
    sourceMessagePriority->setMessagePriorityLevel ( CadKit::MESSAGE_PROGRESS, _progressPrintLevel );

  // See if we can tell the target which LODs to process.
  SlQueryPtr<ILodOption> targetLodOption ( target );
  if ( targetLodOption.isValid() )
    targetLodOption->setLodProcessOption ( _lodOption );

  // See if we can tell the source which LODs to process.
  SlQueryPtr<ILodOption> sourceLodOption ( source );
  if ( sourceLodOption.isValid() )
    sourceLodOption->setLodProcessOption ( _lodOption );

  // See if we can set the zero range.
  SlQueryPtr<IZeroRangeFloat> sourceZeroRange ( source );
  if ( sourceZeroRange.isValid() )
    sourceZeroRange->setZeroRange ( static_cast<float> ( _zeroRange[NEGATIVE_ZERO_INDEX] ), static_cast<float> ( _zeroRange[POSITIVE_ZERO_INDEX] ) );

  // See if we can set the zero range.
  SlQueryPtr<IZeroRangeFloat> targetZeroRange ( target );
  if ( targetZeroRange.isValid() )
    targetZeroRange->setZeroRange ( static_cast<float> ( _zeroRange[NEGATIVE_ZERO_INDEX] ), static_cast<float> ( _zeroRange[POSITIVE_ZERO_INDEX] ) );

  // Make sure the source supports the interface we need.
  SlQueryPtr<IDataSource> ds ( source );
  if ( ds.isNull() )
  {
    PRINT << "Failed to get needed interface from the data source." << std::endl;
    return false;
  }

  // Set the source's target.
  ds->setDataTarget ( target );

  // See if the source can be controlled.
  SlQueryPtr<IControlled> cs ( source );
  if ( cs.isValid() )
    cs->setController ( this->queryInterface ( CadKit::IUnknown::IID ) );

  // See if the target can be controlled.
  SlQueryPtr<IControlled> ct ( target );
  if ( ct.isValid() )
    ct->setController ( this->queryInterface ( CadKit::IUnknown::IID ) );

  // Print the current time.
  time_t now = ::time ( NULL );
  PRINT_LEVEL ( 1 ) << "Start time: " << ::asctime ( localtime ( &now ) ) << std::flush;

  // Load the data.
  if ( false == ds->loadData ( filename ) )
  {
    PRINT << "Failed to translate: " << filename.c_str() << std::endl;
    return false;
  }

  // See if the target should write a file.
  SlQueryPtr<IDataTarget> dt ( target );
  if ( dt.isValid() )
  {
    // Get the output filename.
    std::string outfile ( dt->getDefaultOutputName ( filename ) );

    PRINT_LEVEL ( 1 ) << "Writing: " << outfile.c_str() << std::endl;

    // Store the data.
    if ( false == dt->storeData ( outfile ) )
    {
      PRINT << "Failed to write: " << outfile.c_str() << std::endl;
      return false;
    }
  }

  // Print the current time.
  now = ::time ( NULL );
  PRINT_LEVEL ( 1 ) << "End time: " << ::asctime ( localtime ( &now ) ) << std::flush;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of a message.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::messageNotify ( const std::string &message, const unsigned long &id, const MessageType &type )
{
  SL_PRINT5 ( "In CtTranslation::messageNotify(), this = %X, id = %d, type = %d, message = %s\n", this, id, type, message.c_str() );

  // See what kind of message it is.
  switch ( type )
  {
  case CadKit::MESSAGE_ERROR:

    if ( CadKit::hasBits ( _printFlags, (unsigned int) _PRINT_ERRORS ) )
      return this->_messageNotify ( "Error",   message, id );
    break;

  case CadKit::MESSAGE_WARNING:

    if ( CadKit::hasBits ( _printFlags, (unsigned int) _PRINT_WARNINGS ) )
      return this->_messageNotify ( "Warning",   message, id );
    break;
  
  case CadKit::MESSAGE_INFO:

    if ( CadKit::hasBits ( _printFlags, (unsigned int) _PRINT_INFO ) )
      PRINT << message.c_str() << std::endl;
    break;

  case CadKit::MESSAGE_PROGRESS:

    // When it is a progress message the id is the priority level.
    if ( id <= _progressPrintLevel )
      PRINT << message.c_str() << std::endl;
    break;

  default:

    SL_ASSERT ( 0 ); // What message is this?
    break;
  }

  // Keep going.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of a message.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::_messageNotify ( const std::string &type, const std::string &message, const unsigned long &id ) const
{
  SL_PRINT4 ( "In CtTranslation::_messageNotify(), this = %X, message = %s, id = %d\n", this, message.c_str(), id );

  // Decide what to do.
  switch ( id )
  {
  case CadKit::NO_INTERFACE:

    // This really isn't an error. The targets and sources should routinely
    // check for interfaces. This is just a notification that an interface was 
    // requested and not found.
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress printing level.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setProgressPrintLevel ( const unsigned int &level )
{
  SL_PRINT3 ( "In CtTranslation::_setProgressPrintLevel(), this = %X, level = %d\n", this, level );
  _progressPrintLevel = level;
}
