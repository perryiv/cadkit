
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PASSWORD_PROMPT_COMPONENT_CLASS_H_
#define _PASSWORD_PROMPT_COMPONENT_CLASS_H_

#include "Helios/Qt/Plugins/PasswordPrompt/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IPasswordPrompt.h"
#include "Usul/Threads/Variable.h"

#include "QtCore/QObject"

#include <string>


class PasswordPromptComponent : public QObject,
	                              public Usul::Base::Object,
                                public Usul::Interfaces::IPlugin,
																public Usul::Interfaces::IPasswordPrompt
{
	Q_OBJECT;
public:

  /// Typedefs.
  typedef QObject BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
	typedef Usul::Threads::Variable<bool> Boolean;
	typedef Usul::Threads::Variable<std::string> String;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PasswordPromptComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  PasswordPromptComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Password Prompt"; }

	/// Prompt the user for a password.
	virtual std::string         promptForPassword ( const std::string& text );

protected: 

  // Do not copy.
  PasswordPromptComponent ( const PasswordPromptComponent & );
  PasswordPromptComponent &operator = ( const PasswordPromptComponent & );

  /// Use reference counting.
  virtual ~PasswordPromptComponent();

private slots:
	void _promptForPassword ( QString text );

private:
	Boolean _done;
	String  _text;
};


#endif // _PASSWORD_PROMPT_COMPONENT_CLASS_H_
