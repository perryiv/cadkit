
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnknown interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the reference count.
  virtual unsigned long   getRefCount() const;

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( const unsigned long &iid );

  // Reference/unreference this instance.
  virtual void            ref();
  virtual void            unref();

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

  SL_DECLARE_REFCOUNT_TYPE ( CtTranslation );
  SL_DECLARE_DYNAMIC_CLASS ( CtTranslation, 1032745026 );
};

}; // namespace CadKit

#endif // _CADKIT_TRANSLATION_CONTROLLER_H_
