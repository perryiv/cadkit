
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for input events.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_INPUT_LISTENER_H_
#define _USUL_INTERFACES_INPUT_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"

#include <set>


namespace Usul {
namespace Interfaces {


struct IInputListener : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef std::set<int> KeysDown;
  typedef std::set<unsigned int> ButtonsDown;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IInputListener );

  // Id for this interface.
  enum { IID = 3947957280u };

  // Called when the mouse moves.
  virtual void            mouseMoveNotify ( const KeysDown &, 
                                            const ButtonsDown &, 
                                            double normalizedMouseX, 
                                            double normalizedMouseY, 
                                            Usul::Interfaces::IUnknown::RefPtr caller ) = 0;

  // Called when a button is pressed
  virtual void            buttonPressNotify ( const KeysDown &, 
                                              const ButtonsDown &, 
                                              double normalizedMouseX, 
                                              double normalizedMouseY, 
                                              Usul::Interfaces::IUnknown::RefPtr caller ) = 0;

  // Called when a button is released
  virtual void            buttonReleaseNotify ( const KeysDown &, 
                                                const ButtonsDown &, 
                                                double normalizedMouseX, 
                                                double normalizedMouseY, 
                                                Usul::Interfaces::IUnknown::RefPtr caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_INPUT_LISTENER_H_
