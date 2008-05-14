
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
#include "CtUsage.h"

#include "Standard/SlPrint.h"
#include "Standard/SlPathname.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlBitmask.h"
#include "Standard/SlScopedSet.h"
#include "Standard/SlErrorPolicy.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlPathname.h"

#include "Interfaces/IDataSource.h"
#include "Interfaces/IDataRead.h"
#include "Interfaces/IDataWrite.h"
#include "Interfaces/IControlled.h"
#include "Interfaces/IMessagePriority.h"
#include "Interfaces/ILodOption.h"
#include "Interfaces/IZeroRange.h"
#include "Interfaces/IFileExtension.h"
#include "Interfaces/IOutputAttribute.h"
#include "Interfaces/IOutputPrecision.h"
#include "Interfaces/IScale.h"
#include "Interfaces/IRotate.h"
#include "Interfaces/IOutputUnits.h"

#include <time.h>
#include <stdexcept>
#include <sstream>


#define PRINT if ( _out ) (*_out)
#define MIN_NUM_ARGS 2
#define NEGATIVE_ZERO_INDEX 0
#define POSITIVE_ZERO_INDEX 1

// Don't bother formatting the string if it won't be printed.
#define PRINT_LEVEL(priority)\
  if ( priority <= _progressPrintLevel && _out )\
    (*_out)

namespace CadKit { typedef ErrorPolicy::Throw < std::runtime_error > ThrowIfNotFound; };

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( CtTranslation, SlRefBase );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( CtTranslation, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CtTranslation::CtTranslation() : SlRefBase ( 0 ),
  _out ( &(std::cout) ),
  _progressPrintLevel ( 0 ),
  _printFlags ( 0 ),
  _lodOption ( CadKit::PROCESS_ALL_LODS ),
  _zeroRange ( 0.0, 0.0 ),
  _target ( 0x0 ),
  _source ( 0x0 ),
  _outputDir ( "" )
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

CadKit::IUnknown *CtTranslation::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In CtTranslation::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IMessageNotify::IID:
    return static_cast<IMessageNotify *>(this);
  case ICommandLine::IID:
    return static_cast<ICommandLine *>(this);
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
//  Get the usage string.
//
///////////////////////////////////////////////////////////////////////////////

std::string CtTranslation::getUsageString ( const std::string &program, bool extended )
{
  SL_PRINT2 ( "In CtTranslation::getUsageString(), this = %X\n", this );

  // See if you can get the source's file extension.
  std::string ext ( "ext" );
  SlQueryPtr<IFileExtension> fe ( _source );
  if ( fe.isValid() )
    ext = fe->getFileExtension();

  // Format the usage string.
  std::string usage;
  CadKit::format ( usage, 
                   "Usage: %s [options] <filename1.%s> [filename2.%s] ...",
                   CadKit::justFilename ( program ).c_str(),
                   ext.c_str(),
                   ext.c_str() );
  usage += ( ( extended ) ? CtUsage::getLong() : CtUsage::getShort() );

  // Return the usage string.
  return usage;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the usage of the specific feature.
//
///////////////////////////////////////////////////////////////////////////////

std::string CtTranslation::_getUsageSubString ( const std::string &feature ) const
{
  SL_PRINT2 ( "In CtTranslation::_getUsageSubString(), this = %X\n", this );

  // The substring.
  std::string sub;

  // Get the long usage string.
  std::string usage ( CtUsage::getLong() );

  // Search for the feature.
  std::string::size_type start = usage.find ( feature );

  // Did we find it?
  if ( std::string::npos != start )
  {
    // Search for the end of the line.
    std::string::size_type end = usage.find ( '\n', start );

    // Did we find it?
    if ( std::string::npos != end && end > start )
    {
      // Assign the substring.
      sub.assign ( usage.begin() + start, usage.begin() + end );
    }
  }

  // Return the substring, which may still be empty.
  return sub;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the arguments. Returns true if execution should continue.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::_parseArguments ( int argc, char **argv, std::list<std::string> &args )
{
  SL_PRINT3 ( "In CtTranslation::_parseArguments(), this = %X, argc = %d\n", this, argc );

  // Check the number of arguments.
  if ( argc < MIN_NUM_ARGS || NULL == argv )
  {
    std::cout << this->getUsageString ( argv[0], false );
    return false;
  }

  // Get the arguments.
  for ( int i = 1; i < argc; ++i )
  {
    // Grab the current argument test against our flags.
    std::string arg ( argv[i] );

    if ( arg == "-h" || arg == "--help" )
    {
      // Print the usage string now.
      std::cout << this->getUsageString ( argv[0], true );
      return false;
    }

    else if ( arg == "-pp" || arg == "--print-progress" )
    {
      // Get the next argument, if there is one.
      std::string option ( ( i + 1 == argc ) ? "" : argv[++i] );

      // See if the option string is an integer.
      if ( true == CadKit::isUnsignedInteger ( option ) )
        this->_setProgressPrintLevel ( CadKit::toUnsignedInteger ( option ) );

      // Otherwise...
      else
      {
        // Print useful message.
        std::cout << "Error: argument " << arg << " should be followed by an unsigned integer.\n";
        std::cout << this->_getUsageSubString ( "--print-progress" ) << '\n';
        return false;
      }
    }

    else if ( arg == "-pw" || arg == "--print-warnings" )
    {
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_WARNINGS );
    }

    else if ( arg == "-pi" || arg == "--print-info" )
    {
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) _PRINT_INFO );
    }

    else if ( arg == "-v" || arg == "--verbose" )
    {
      this->_setProgressPrintLevel ( 1 );
      _printFlags = CadKit::addBits ( _printFlags, (unsigned int) ( _PRINT_WARNINGS | _PRINT_INFO ) );
    }

    else if ( arg == "-al" || arg == "--all-lods" )
    {
      this->_setLodOption ( CadKit::PROCESS_ALL_LODS );
    }

    else if ( arg == "-hl" || arg == "--high-lod" )
    {
      this->_setLodOption ( CadKit::PROCESS_HIGH_LOD );
    }

    else if ( arg == "-ll" || arg == "--low-lod" )
    {
      this->_setLodOption ( CadKit::PROCESS_LOW_LOD );
    }

    else if ( arg == "-z" || arg == "--zero" )
    {
      // Get the next two arguments, if there are any.
      std::string negative  ( ( i + 1 == argc ) ? "" : argv[i+1] );
      std::string positive ( ( i + 2 == argc ) ? "" : argv[i+2] );

      // See if the strings are a numbers.
      if ( true == CadKit::isNumber ( negative ) && true == CadKit::isNumber ( positive ) )
      {
        // Set the zero value and increment the loop index.
        this->_setZeroRange ( ::atof ( negative.c_str() ), ::atof ( positive.c_str() ) );
        i += 2;
      }

      // Otherwise return false.
      else
        return false;
    }

    else if ( arg == "-ob" || arg == "--output-binary" )
    {
      this->_setBinaryOutput ( true );
    }

    else if ( arg == "-oa" || arg == "--output-ascii" )
    {
      this->_setBinaryOutput ( false );
    }

    else if ( arg == "-c" || arg == "--center" )
    {

    }

    else if ( arg == "-s" || arg == "--scale" )
    {
      // Get the next three arguments, if there are any.
      std::string x ( ( i + 1 == argc ) ? "" : argv[i+1] );
      std::string y ( ( i + 2 == argc ) ? "" : argv[i+2] );
      std::string z ( ( i + 3 == argc ) ? "" : argv[i+3] );

      // See if the strings are a numbers.
      if ( true == CadKit::isNumber ( x ) && 
           true == CadKit::isNumber ( y ) && 
           true == CadKit::isNumber ( z ) )
      {
        // Set the scale and increment the loop index.
        this->_setScale ( ::atof ( x.c_str() ), ::atof ( y.c_str() ), ::atof ( z.c_str() ) );
        i += 3;
      }

      // Otherwise return false.
      else
        return false;
    }

    else if ( arg == "-t" || arg == "--translate" )
    {

    }

    else if ( arg == "-r" || arg == "--rotate" )
    {
      // Get the next four arguments, if there are any.
      std::string x ( ( i + 1 == argc ) ? "" : argv[i+1] );
      std::string y ( ( i + 2 == argc ) ? "" : argv[i+2] );
      std::string z ( ( i + 3 == argc ) ? "" : argv[i+3] );
      std::string d ( ( i + 4 == argc ) ? "" : argv[i+4] );

      // See if the strings are a numbers.
      if ( true == CadKit::isNumber ( x ) && 
           true == CadKit::isNumber ( y ) && 
           true == CadKit::isNumber ( z ) &&
           true == CadKit::isNumber ( d ) )
      {
        // Set the scale and increment the loop index.
        this->_setRotation ( ::atof ( x.c_str() ), ::atof ( y.c_str() ), ::atof ( z.c_str() ), ::atof ( d.c_str() ) );
        i += 4;
      }

      // Otherwise return false.
      else
        return false;    
    }

    else if ( arg == "-stf" || arg == "--scale-to-fit" )
    {

    }

    else if ( arg == "-nd" || arg == "--num-decimals" )
    {
      // Get the next argument, if there is one.
      std::string option ( ( i + 1 == argc ) ? "" : argv[++i] );

      // See if the option string is an integer.
      if ( true == CadKit::isUnsignedInteger ( option ) )
        this->_setOutputNumDecimals ( CadKit::toUnsignedInteger ( option ) );

      // Otherwise...
      else
      {
        // Print useful message.
        std::cout << "Error: argument " << arg << " should be followed by an unsigned integer.\n";
        std::cout << this->_getUsageSubString ( "--num-decimals" ) << '\n';
        return false;
      }
    }

    else if ( arg == "-od" || arg == "--output-directory" )
    {
      // Get the next argument, if there is one.
      _outputDir = ( ( i + 1 == argc ) ? "" : argv[++i] );

      // Make sure the directory exists. TODO
      //if ( false == CadKit::makeDirectory ( _outputDir ) )
      //{
      //  // Print useful message.
      //  std::cout << "Error: unable to create output directory: " << _outputDir << '\n';
      //  return false;
      //}
    }

    else if ( arg == "-ou" || arg == "--output-units" )
    {
      // Get the next argument, if there is one.
      this->_setUnits(std::string( ( i + 1 == argc ) ? "" : argv[++i] ));
    }

    // Otherwise just save the argument.
    else
    {
      args.push_back ( argv[i] );
    }
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line arguments and execute.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::execute ( int argc, char **argv, IUnknown *source, IUnknown *target )
{
  SL_PRINT5 ( "In CtTranslation::execute(), this = %X, argc = %d, source = %X, target = %X\n", this, argc, source, target );

  std::string message;

  try
  {
    this->_execute ( argc, argv, source, target );
    return true;
  }

  catch ( const std::exception &e )
  {
    message = e.what();
  }

  catch ( const std::string &s )
  {
    message = s;
  }

  catch ( const char *s )
  {
    message = s;
  }

#ifdef _CADKIT_CATCH_ALL
  catch ( ... )
  {
    PRINT << "Unknown exception caught." << std::endl;
  }
#endif

  PRINT << "Error: " << message;

  // Do we need a newline character?
  if ( message.size() && '\n' == message[message.size()-1] )
    PRINT << std::flush;
  else
    PRINT << std::endl;

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line arguments and execute.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_execute ( int argc, char **argv, IUnknown *source, IUnknown *target )
{
  SL_PRINT5 ( "In CtTranslation::_execute(), this = %X, argc = %d, source = %X, target = %X\n", this, argc, source, target );

  // Set this instance's target and source. The destructor will reset them 
  // to their original value (which should be null).
  SlScopedSet<SlRefPtr<IUnknown>, IUnknown *> temp1 ( _target, target );
  SlScopedSet<SlRefPtr<IUnknown>, IUnknown *> temp2 ( _source, source );

  // Parse the arguments. Returns false if we should bail. Takes care of 
  // printing "usage" message.
  typedef std::list<std::string> Args;
  Args args;
  if ( false == this->_parseArguments ( argc, argv, args ) )
    return;

  // Attach the modules to each other.
  this->_attachModules();

  // Print the current time.
  time_t now = ::time ( NULL );
#if defined _WIN32 && _MSC_VER >= 1400
  char buf[256];
  tm tmtemp;
  if ( localtime_s(&tmtemp, &now) )
    return;
  if ( asctime_s( buf, 256, &tmtemp ) )
    return;
  PRINT_LEVEL ( 1 ) << "Start time: " << buf << std::flush;
#else
  PRINT_LEVEL ( 1 ) << "Start time: " << ::asctime ( localtime ( &now ) ) << std::flush;
#endif

  // Loop through the arguments.
  for ( Args::const_iterator i = args.begin(); i != args.end(); ++i )
    this->_translate ( *i );

  // Print the current time.
  now = ::time ( NULL );
#if defined _WIN32 && _MSC_VER >= 1400
  if ( localtime_s(&tmtemp, &now) )
    return;
  if ( asctime_s( buf, 256, &tmtemp ) )
    return;
  PRINT_LEVEL ( 1 ) << "End time: " << buf << std::flush;
#else
  PRINT_LEVEL ( 1 ) << "End time: " << ::asctime ( localtime ( &now ) ) << std::flush;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Attach the modules.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_attachModules()
{
  SL_PRINT2 ( "In CtTranslation::_attachModules(), this = %X\n", this );

  // Make sure we have a data source interface.
  SlQueryPtr<IDataSource,ThrowIfNotFound> ds ( _source, "Failed to get IDataSource interface from the given data source." );

  // Set the source's target.
  ds->setDataTarget ( _target );

  // See if the source can be controlled.
  SlQueryPtr<IControlled> cs ( _source );
  if ( cs.isValid() )
    cs->setController ( this->queryInterface ( CadKit::IUnknown::IID ) );

  // See if the target can be controlled.
  SlQueryPtr<IControlled> ct ( _target );
  if ( ct.isValid() )
    ct->setController ( this->queryInterface ( CadKit::IUnknown::IID ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the data.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_translate ( const std::string &filename )
{
  SL_PRINT3 ( "In CtTranslation::_translate(), this = %X, filename = %s\n", this, filename.c_str() );

  // Read and write the data.
  this->_read ( filename );
  this->_write ( this->_getOutputName ( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the data.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_read ( const std::string &filename )
{
  SL_PRINT3 ( "In CtTranslation::_read(), this = %X, filename = %s\n", this, filename.c_str() );

  // Make sure the source supports the interfaces we need.
  SlQueryPtr<IDataRead,ThrowIfNotFound> dr ( _source, "Failed to get IDataRead from _source." );

  // Read the data.
  if ( false == dr->readData ( filename ) )
  {
    std::ostringstream message;
    message << "Failed to translate: " << filename.c_str() << std::endl;
    throw std::runtime_error ( message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the data.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_write ( const std::string &filename )
{
  SL_PRINT3 ( "In CtTranslation::_write(), this = %X, filename = %s\n", this, filename.c_str() );

  // See if the target should write the data somewhere.
  SlQueryPtr<IDataWrite,ThrowIfNotFound> dw ( _target, "Failed to get IDataWrite from _target." );

  PRINT_LEVEL ( 1 ) << "Writing: " << filename.c_str() << std::endl;

  // Write the data.
  if ( false == dw->writeData ( filename ) )
  {
    std::ostringstream message;
    message << "Failed to write: " << filename.c_str() << std::endl;
    throw std::runtime_error ( message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of a message.
//
///////////////////////////////////////////////////////////////////////////////

bool CtTranslation::messageNotify ( const std::string &message, unsigned long id, const MessageType &type )
{
  SL_PRINT5 ( "In CtTranslation::messageNotify(), this = %X, id = %d, type = %d, message = %s\n", this, id, type, message.c_str() );

  // See what kind of message it is.
  switch ( type )
  {
  case CadKit::MESSAGE_ERROR:

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

bool CtTranslation::_messageNotify ( const std::string &type, const std::string &message, unsigned long id ) const
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

void CtTranslation::_setProgressPrintLevel ( unsigned int level )
{
  SL_PRINT3 ( "In CtTranslation::_setProgressPrintLevel(), this = %X, level = %d\n", this, level );

  // See if we can tell the target the priority level.
  SlQueryPtr<IMessagePriority> mp ( _target );
  if ( mp.isValid() )
    mp->setMessagePriorityLevel ( CadKit::MESSAGE_PROGRESS, level );

  // See if we can tell the source the priority level.
  mp = _source;
  if ( mp.isValid() )
    mp->setMessagePriorityLevel ( CadKit::MESSAGE_PROGRESS, level );

  // Set this controller's level too.
  _progressPrintLevel = level;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the level-of-detail option.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setLodOption ( const LodProcessOption &option )
{
  SL_PRINT3 ( "In CtTranslation::_setLodOption(), this = %X, option = %d\n", this, option );

  // See if we can tell the target which LODs to process.
  SlQueryPtr<ILodOption> targetLodOption ( _target );
  if ( targetLodOption.isValid() )
    targetLodOption->setLodProcessOption ( option );

  // See if we can tell the source which LODs to process.
  SlQueryPtr<ILodOption> sourceLodOption ( _source );
  if ( sourceLodOption.isValid() )
    sourceLodOption->setLodProcessOption ( option );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the floating-point range that gets truncated to zero.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setZeroRange ( double negative, double positive )
{
  SL_PRINT4 ( "In CtTranslation::_setZeroRange(), this = %X, negative = %f, positive = %f\n", this, negative, positive );

  // See if we can set the zero range.
  SlQueryPtr<IZeroRangeFloat> sourceZeroRangeFloat ( _source );
  if ( sourceZeroRangeFloat.isValid() )
    sourceZeroRangeFloat->setZeroRange ( static_cast<float> ( negative ), static_cast<float> ( positive ) );

  // See if we can set the zero range.
  SlQueryPtr<IZeroRangeDouble> sourceZeroRangeDouble ( _source );
  if ( sourceZeroRangeDouble.isValid() )
    sourceZeroRangeDouble->setZeroRange ( negative, positive );

  // See if we can set the zero range.
  SlQueryPtr<IZeroRangeFloat> targetZeroRange ( _target );
  if ( targetZeroRange.isValid() )
    targetZeroRange->setZeroRange ( static_cast<float> ( negative ), static_cast<float> ( positive ) );

  // See if we can set the zero range.
  SlQueryPtr<IZeroRangeDouble> targetZeroRangeDouble ( _target );
  if ( targetZeroRangeDouble.isValid() )
    targetZeroRangeDouble->setZeroRange ( negative, positive );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn binary output on/off if available.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setBinaryOutput ( bool state )
{
  SL_PRINT3 ( "In CtTranslation::_setBinaryOutput(), this = %X, state = %d\n", this, state );

  // Set the output format attribute if the interface is available. 
  SlQueryPtr<IOutputAttribute> oa ( _target );
  if ( oa.isValid() )
    oa->setOutputAttribute ( ( state ) ? FORMAT_ATTRIBUTE_BINARY : FORMAT_ATTRIBUTE_ASCII );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the output precision.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setOutputNumDecimals ( unsigned int numDecimals )
{
  SL_PRINT3 ( "In CtTranslation::_setOutputNumDecimals(), this = %X, numDecimals = %d\n", this, numDecimals );

  // Set the output precision if the interface is available. 
  SlQueryPtr<IOutputPrecision> op ( _target );
  if ( op.isValid() )
    op->setOutputNumDecimals ( numDecimals );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setScale ( double x, double y, double z )
{
  SL_PRINT3 ( "In CtTranslation::_setScale(), this = %X, numDecimals = %d\n", this, numDecimals );

  // Set the scale if the interface is available. 
  SlQueryPtr<IScaleDouble> sd ( _source );
  if ( sd.isValid() )
  {
    sd->scale ( x, y, z );
    return;
  }

  // Try this interface too. 
  SlQueryPtr<IScaleFloat> sf ( _source );
  if ( sf.isValid() )
    sf->scale ( static_cast<float> ( x ), static_cast<float> ( y ), static_cast<float> ( z ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotation.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setRotation ( double x, double y, double z, double deg )
{
  SL_PRINT3 ( "In CtTranslation::_setRotation(), this = %X, numDecimals = %d\n", this, numDecimals );

  // Set the scale if the interface is available. 
  SlQueryPtr<IRotateDouble> rd ( _source );
  if ( rd.isValid() )
  {
    rd->rotate ( x, y, z, deg );
    return;
  }

  // Try this interface too. 
  SlQueryPtr<IRotateFloat> rf ( _source );
  if ( rf.isValid() )
    rf->rotate ( static_cast<float> ( x ), static_cast<float> ( y ), static_cast<float> ( z ), static_cast<float> ( deg ) );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the units for the output file.
//
///////////////////////////////////////////////////////////////////////////////

void CtTranslation::_setUnits ( const std::string &units )
{
  SL_PRINT3 ( "In CtTranslation::_setUnits(), this = %X, units = %s\n", this, units.c_str() );

  // Set the scale if the interface is available. 
  SlQueryPtr<IOutputUnits> su ( _target );
  if ( su.isValid() )
  {
    su->setUnits ( units );
    return;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the output filename based on the given filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string CtTranslation::_getOutputName ( const std::string &filename )
{
  SL_PRINT3 ( "In CtTranslation::_getOutputName(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Parse the path.
  SlPathname<std::string> path ( filename );

  // Replace the directory if we have a valid one.
  if ( false == _outputDir.empty() )
    path.setDirectory ( _outputDir );

  // Get the target's file extension.
  std::string ext ( this->_getTargetFileExtension() );

  // Set the extension, or some default.
  path.setExtension ( ( ext.empty() ) ? "data" : ext );

  // Return the output filename.
  return path.getPathname();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the target's file extension, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

std::string CtTranslation::_getTargetFileExtension()
{
  SL_PRINT2 ( "In CtTranslation::_getTargetFileExtension(), this = %X\n", this );

  // Initialize the extension.
  std::string ext;

  // Try to get the extension.
  SlQueryPtr<IFileExtension> fe ( _target );
  if ( fe.isValid() )
    ext = fe->getFileExtension();

  // Return the extension.
  return ext;
}
