
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

#include "Helios/Qt/Plugins/PasswordPrompt/PasswordPromptComponent.h"

#include "Usul/Components/Factory.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Safe.h"

#include "QtGui/QInputDialog"

USUL_DECLARE_COMPONENT_FACTORY ( PasswordPromptComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PasswordPromptComponent, Usul::Base::Object );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PasswordPromptComponent::PasswordPromptComponent() : BaseClass(),
	_done ( false ),
	_text()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PasswordPromptComponent::~PasswordPromptComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PasswordPromptComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
	case Usul::Interfaces::IPasswordPrompt::IID:
		return static_cast < Usul::Interfaces::IPasswordPrompt* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt the user for a password.
//
///////////////////////////////////////////////////////////////////////////////

std::string PasswordPromptComponent::promptForPassword ( const std::string& text )
{
	// Only prompt for one thread at a time.
	Guard guard ( this->mutex() );

	Usul::Threads::Safe::set ( _text.mutex(), "", _text.value() );
	Usul::Threads::Safe::set ( _done.mutex(), false, _done.value() );

	QMetaObject::invokeMethod ( this, "_promptForPassword", Qt::QueuedConnection, Q_ARG ( QString, text.c_str() ) );

	while ( false == Usul::Threads::Safe::get ( _done.mutex(), _done.value() ) )
	{
		Usul::System::Sleep::milliseconds ( 500 );
	}

	return Usul::Threads::Safe::get ( _text.mutex(), _text.value() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt the user for a password.
//
///////////////////////////////////////////////////////////////////////////////

void PasswordPromptComponent::_promptForPassword ( QString text )
{
	QString result ( QInputDialog::getText ( 0x0, "Enter Password", text, QLineEdit::Password ) );

	Usul::Threads::Safe::set ( _text.mutex(), result.toStdString(), _text.value() );
	Usul::Threads::Safe::set ( _done.mutex(), true, _done.value() );
}
