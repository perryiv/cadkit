
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to toggle the visibility.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_TOGGLE_VIEW_H__
#define __CADKIT_HELIOS_QT_COMMANDS_TOGGLE_VIEW_H__

#include "Helios/Qt/Commands/Command.h"

class QWidget;

namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT ToggleView : public Usul::Commands::Command
{
public:

  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( ToggleView );

  // Constructor.
  ToggleView ( QWidget *widget );

protected:
  // Use reference counting.
  virtual ~ToggleView();

  // Execute.
  virtual void              _execute();

  // Update the enabled flag.
  virtual bool              updateEnable () const;

  // Update the checked state.
  virtual bool              updateCheck () const;

private:
  QWidget *_widget;
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_SAVE_DOCUMENT_H__
