
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_STATE_H__
#define __DEVICES_STATE_H__

#include "Usul/Export/Export.h"

#include <map>

namespace Usul {
namespace Devices {

class USUL_EXPORT State
{
public:
  State();
  State( float x, float y, float z );

  //TODO.  Is a button pressed and button held needed?
  bool buttonPressed ( unsigned int button ) const;
  void buttonPressed ( unsigned int button, bool pressed );

  void  x( float );
  float x() const;

  void  y( float );
  float y() const;

  void  z( float );
  float z() const;

  void         numClicks ( unsigned int );
  unsigned int numClicks() const;

  void         rootX ( unsigned int );
  unsigned int rootX() const;

  void         rootY ( unsigned int );
  unsigned int rootY() const;

private:
  typedef std::map < unsigned int, bool > ButtonsPressed;

  ButtonsPressed _buttons;
  float _x;
  float _y;
  float _z;

  unsigned int _numClicks;

  unsigned int _rootX;
  unsigned int _rootY;
};

} // namespace Usul
} // namespace Devices


#endif // __DEVICES_STATE_H__

