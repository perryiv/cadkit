
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
//  Command to save a document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_ABOUT_H__
#define __CADKIT_HELIOS_QT_COMMANDS_ABOUT_H__

#include "Helios/Qt/Commands/Command.h"

class QWidget;

namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT About : public Usul::Commands::Command
{
public:
  
  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;
  
  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( About );
  
  // Constructor.
  About ( QWidget *parent, const std::string& text, const std::string& about );
  
protected:
  // Use reference counting.
  virtual ~About();
  
  // Execute.
  virtual void              _execute();
  
private:
  QWidget *_parent;
  std::string _about;
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_ABOUT_H__
