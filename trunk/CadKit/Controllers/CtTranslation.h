
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

#include "Interfaces/IMessageNotify.h"
#include "Interfaces/ICommandLine.h"
#include "Interfaces/ITranslator.h"
#include "Interfaces/IOutputStream.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlVec2.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <iostream>
# include <list>
#endif


namespace CadKit
{
class CT_API CtTranslation : public SlRefBase, 
                             public IMessageNotify,
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
  //  IMessageNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Notification of a message.
  virtual bool            messageNotify ( const std::string &message, const unsigned long &id, const MessageType &type );

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
  virtual bool            parseArguments ( const int &argc, const char **argv, ICommandLine::Args &args );

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

  enum
  {
    _PRINT_ERRORS   = 0x00000001,
    _PRINT_WARNINGS = 0x00000002,
    _PRINT_INFO     = 0x00000004,
  };

  std::ostream *_out;
  unsigned int _progressPrintLevel;
  unsigned int _printFlags;
  LodProcessOption _lodOption;
  SlVec2d _zeroRange;

  virtual ~CtTranslation();

  bool                    _messageNotify ( const std::string &type, const std::string &message, const unsigned long &id ) const;

  void                    _setProgressPrintLevel ( const unsigned int &level );

  SL_DECLARE_REFERENCE_POINTER ( CtTranslation );
  SL_DECLARE_DYNAMIC_CLASS ( CtTranslation, 1032745026 );
};

}; // namespace CadKit

#endif // _CADKIT_TRANSLATION_CONTROLLER_H_
