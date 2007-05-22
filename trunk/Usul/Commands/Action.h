
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all actions. An action is an event handler.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMMANDS_ACTION_CLASS_H_
#define _USUL_COMMANDS_ACTION_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Base/Object.h"

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Commands {


class USUL_EXPORT Action : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Action );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Action );

  // Get the caller.
  const IUnknown *            caller() const;
  IUnknown *                  caller();
  
  // Get/Set the icon path.
  void                        iconPath ( const std::string& value );
  std::string                 iconPath() const;
  
  // Get/Set the shortcut.
  void                        shortcut ( const std::string& value );
  std::string                 shortcut() const;
  
  // Get/Set the text.
  void                        text( const std::string& value );
  std::string                 text() const;
  
  // Get/Set the tooltip.
  void                        tooltip ( const std::string& value );
  std::string                 tooltip() const;

protected:

  // Constructor.
  Action ( IUnknown *caller );

  // Use reference counting.
  virtual ~Action();

private:

  // No copying or assignment.
  Action ( const Action & );
  Action &operator = ( const Action & );

  void                        _destroy();

  IUnknown::RefPtr _caller;
  std::string      _text;
  std::string      _toolTipText;
  std::string      _shortcut;
  std::string      _iconPath;
};


} // namespace Commands
} // namespace Usul


#endif //_USUL_COMMANDS_ACTION_CLASS_H_
