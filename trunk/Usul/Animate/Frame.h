
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ANIMATE_FRAME_H__
#define __USUL_ANIMATE_FRAME_H__

namespace Usul 
{
  namespace Animate
  {
    template < class UserData_, class Matrix > class Frame
    {
    public:
      typedef UserData_ UserData;
      Frame() { }
      Frame ( UserData userData, Matrix matrix ) : _userData ( userData ), _matrix ( matrix ) { }

      const UserData& getUserData () const { return _userData; }
      UserData& getUserData () { return _userData; }
      const Matrix& getMatrix ()  const { return _matrix; }

    protected:
      UserData _userData;
      Matrix _matrix;
    }; //Frame

  }; // namespace Animate

}; //namespace Usul

#endif //__USUL_ANIMATE_FRAME_H__

