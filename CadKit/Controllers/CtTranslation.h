
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

#ifndef _CADKIT_TRANSLATION_CONTROLLER_H_
#define _CADKIT_TRANSLATION_CONTROLLER_H_

#include "CtApi.h"

#include "Interfaces/IErrorNotify.h"
#include "Interfaces/IProgressNotify.h"
#include "Interfaces/IWarningNotify.h"
#include "Interfaces/ICommandLine.h"
#include "Interfaces/ITranslator.h"
#include "Interfaces/IOutputStream.h"

#include "Standard/SlRefBase.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <iostream>
# include <list>
#endif


namespace CadKit
{
class CT_API CtTranslation : public SlRefBase, 
                             public IErrorNotify,
                             public IProgressNotify,
                             public IWarningNotify,
                             public ICommandLine,
                             public ITranslator,
                             public IOutputStream
{
public:

  CtTranslation();

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IErrorNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Report an error. Return true if the caller should continue.
  virtual bool            errorNotify ( const std::string &message, const unsigned long &id ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IProgressNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Report progress. Return true if the caller should continue.
  virtual bool            progressNotify ( const std::string &message ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IWarningNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Report a warning. Return true if the caller should continue.
  virtual bool            warningNotify ( const std::string &message, const unsigned long &id ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ICommandLine interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Check the arguments.
  virtual bool            checkArguments ( const int &argc, const char **argv ) const;

  // Get the usage string.
  virtual std::string     getUsageString ( const std::string &program, const std::string &ext ) const;

  // Parse the arguments.
  virtual void            parseArguments ( const int &argc, const char **argv, ICommandLine::Args &args );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IOutputStream interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the output stream. Default is null.
  virtual void            setOutputStream ( std::ostream *out );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ITranslator interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Translate the database.
  virtual bool            translate ( const std::string &name, IUnknown *source, IUnknown *target );

protected:

  std::ostream *_out;

  virtual ~CtTranslation();

  bool                    _messageNotify ( const std::string &type, const std::string &message, const unsigned long &id ) const;

  SL_DECLARE_REFERENCE_POINTER ( CtTranslation );
  SL_DECLARE_DYNAMIC_CLASS ( CtTranslation, 1032745026 );
};

}; // namespace CadKit

#endif // _CADKIT_TRANSLATION_CONTROLLER_H_
