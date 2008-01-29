
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
#include "Interfaces/IOutputStream.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlVec2.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <iostream>
# include <list>
# include <string>
#endif


namespace CadKit
{
class CT_API CtTranslation : public SlRefBase, 
                             public IMessageNotify,
                             public ICommandLine,
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
  virtual bool            messageNotify ( const std::string &message, unsigned long id, const MessageType &type );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ICommandLine interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Parse the command-line arguments and execute.
  virtual bool            execute ( int argc, char **argv, IUnknown *source, IUnknown *target );

  // Get the usage string.
  virtual std::string     getUsageString ( const std::string &program, bool extended );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IOutputStream interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the output stream. Default is null.
  virtual void            setOutputStream ( std::ostream *out );

protected:

  enum
  {
    _PRINT_WARNINGS = 0x00000002,
    _PRINT_INFO     = 0x00000004,
  };

  std::ostream *_out;
  unsigned int _progressPrintLevel;
  unsigned int _printFlags;
  LodProcessOption _lodOption;
  SlVec2d _zeroRange;
  SlRefPtr<IUnknown> _target;
  SlRefPtr<IUnknown> _source;
  std::string _outputDir;

  virtual ~CtTranslation();

  void                    _attachModules();

  void                    _execute ( int argc, char **argv, IUnknown *source, IUnknown *target );

  std::string             _getTargetFileExtension();
  std::string             _getOutputName ( const std::string &filename );
  std::string             _getUsageSubString ( const std::string &feature ) const;

  bool                    _messageNotify ( const std::string &type, const std::string &message, unsigned long id ) const;

  void                    _read ( const std::string &filename );

  bool                    _parseArguments ( int argc, char **argv, std::list<std::string> &args );

  void                    _setBinaryOutput ( bool state );
  void                    _setLodOption ( const LodProcessOption &option );
  void                    _setProgressPrintLevel ( unsigned int level );
  void                    _setZeroRange ( double negative, double positive );
  void                    _setOutputNumDecimals ( unsigned int numDecimals );
  void                    _setScale ( double x, double y, double z );
  void                    _setRotation ( double x, double y, double z, double deg );
  void                    _setUnits ( const std::string &units );

  void                    _translate ( const std::string &filename );

  void                    _write ( const std::string &filename );

  SL_DECLARE_REFERENCE_POINTER ( CtTranslation );
  SL_DECLARE_DYNAMIC_CLASS ( CtTranslation, 1032745026 );
};

}; // namespace CadKit

#endif // _CADKIT_TRANSLATION_CONTROLLER_H_
