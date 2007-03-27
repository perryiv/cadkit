
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "DirectXJoystick.h"
#include "Usul/Devices/IButtonListener.h"
#include "Usul/System/Clock.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/App.h"

#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IGetBoundingBox.h"

#include "osg/BoundingBox"
#include "osg/Quat"
#include "osg/Matrix"

#include <iostream>

#include "OpenThreads/Thread"
#include "OpenThreads/Mutex"
#include "OpenThreads/ScopedLock"

//Defines for DirectX
#define STRICT
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <list>


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( DirectXJoystick ) MessageMap[] =
{
  // Message type, id, and handler.
  FXMAPFUNC ( FX::SEL_TIMEOUT,         3, DirectXJoystick::onTimeout  ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( DirectXJoystick, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );



namespace Detail
{
  //Maximium joystick value
  const int maxJoystickValue ( 100 );

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Small class to guard the state class.
  //
  ///////////////////////////////////////////////////////////////////////////////

  class GuardState
  {
  public:
    Usul::Devices::State state()
    {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock ( _mutex );
      return _state;
    }

    void state ( const Usul::Devices::State& state )
    {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock ( _mutex );
      _state = state;
    }

  private:
    Usul::Devices::State _state;
    OpenThreads::Mutex _mutex;
  };

  // Current state of the joystick.
  GuardState currentState;


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Joystick implementation.  Adapted from the DirectX Joystick example
  //  provided in the DirectX SDK.
  //
  ///////////////////////////////////////////////////////////////////////////////

  namespace Private
  {
    LPDIRECTINPUT8       _pDI = 0x0;
    LPDIRECTINPUTDEVICE8 _pJoystick = 0x0;

    //-----------------------------------------------------------------------------
    // Name: EnumJoysticksCallback()
    // Desc: Called once for each enumerated joystick. If we find one, create a
    //       device interface on it so we can play with it.
    //-----------------------------------------------------------------------------
    BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
    {
        HRESULT hr;

        // Obtain an interface to the enumerated joystick.
        hr = _pDI->CreateDevice( pdidInstance->guidInstance, &_pJoystick, NULL );

        // If it failed, then we can't use this joystick. (Maybe the user unplugged
        // it while we were in the middle of enumerating it.)
        if( FAILED(hr) ) 
            return DIENUM_CONTINUE;

        // Stop enumeration. Note: we're just taking the first joystick we get. You
        // could store all the enumerated joysticks and let the user pick.
        return DIENUM_STOP;
    }

    //-----------------------------------------------------------------------------
    // Name: EnumObjectsCallback()
    // Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
    //       joystick. This function enables user interface elements for objects
    //       that are found to exist, and scales axes min/max values.
    //-----------------------------------------------------------------------------
    BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                      VOID* pContext )
    {

        // For axes that are returned, set the DIPROP_RANGE property for the
        // enumerated axis in order to scale min/max values.
        if( pdidoi->dwType & DIDFT_AXIS )
        {
            DIPROPRANGE diprg; 
            diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
            diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            diprg.diph.dwHow        = DIPH_BYID; 
            diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
            diprg.lMin              = -maxJoystickValue; 
            diprg.lMax              = +maxJoystickValue; 
        
            // Set the range for the axis
            if( FAILED( _pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
                return DIENUM_STOP;
             
        }

        return DIENUM_CONTINUE;
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Thread to poll joystick.
  //
  ///////////////////////////////////////////////////////////////////////////////

  class Joystick : public OpenThreads::Thread
  {
  public:
    typedef OpenThreads::Thread BaseClass;

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Constructor.
    //
    ///////////////////////////////////////////////////////////////////////////////

    Joystick ( ) :
    BaseClass()
    {
      
    }

    ~Joystick()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Initialize.
    //
    ///////////////////////////////////////////////////////////////////////////////

    bool init ()
    {
      HRESULT hr;

      // Register with the DirectInput subsystem and get a pointer
      // to a IDirectInput interface we can use.
      // Create a DInput object
      if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
        IID_IDirectInput8, (VOID**)&Private::_pDI, NULL ) ) )
          return false;

      // Look for a simple joystick we can use for this sample program.
      if( FAILED( hr = Private::_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, 
        Private::EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY ) ) )
          return false;

      // Make sure we got a joystick
      if( NULL == Private::_pJoystick )
      {
          return false;
      }

      // Set the data format to "simple joystick" - a predefined data format 
      //
      // A data format specifies which controls on a device we are interested in,
      // and how they should be reported. This tells DInput that we will be
      // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
      if( FAILED( hr = Private::_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
          return false;

      // Set the cooperative level to let DInput know how this device should
      // interact with the system and with other DInput applications.
      if( FAILED( hr = Private::_pJoystick->SetCooperativeLevel( (HWND) FoxTools::Functions::mainWindow()->id(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) ) )
          return false;

      // Enumerate the joystick objects. The callback function enabled user
      // interface elements for objects that are found, and sets the min/max
      // values property for discovered axes.
      if( FAILED( hr = Private::_pJoystick->EnumObjects( Private::EnumObjectsCallback, 
                                                  (VOID*) this, DIDFT_ALL ) ) )
          return false;

      // Acquire the joystick.
      Private::_pJoystick->Acquire();

      return true;
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  clean up.
    //
    ///////////////////////////////////////////////////////////////////////////////

    void cleanUp ()
    {
      if( Private::_pJoystick ) 
        Private::_pJoystick->Unacquire();
    
      // Release any DirectInput objects.
      if ( Private::_pJoystick )
        Private::_pJoystick->Release();
      if( Private::_pDI )
        Private::_pDI->Release();

    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Do the work.
    //
    ///////////////////////////////////////////////////////////////////////////////

    virtual void run()
    {
      const unsigned int duration ( 75 );

      while ( true )
      {
        // Stop the thread if there is no joystick.
        if( 0x0 != Private::_pJoystick )
        {
          HRESULT hr = Private::_pJoystick->Poll();

          if ( FAILED( hr ) )
          {
            hr = Private::_pJoystick->Acquire();
            while( hr == DIERR_INPUTLOST ) 
              hr = Private::_pJoystick->Acquire();

            // Not sure if this is needed.
            Private::_pJoystick->Poll();
          }

          DIJOYSTATE2 js;           // DInput joystick state 

          // If there is a problem, punt.
          if( !FAILED( hr = Private::_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
          {

            Usul::Devices::State state;

            state.x( (float) js.lX );
            state.y( (float) js.lY );
            state.z( (float) js.lZ );

            // Loop through the buttons.
            for( int i = 0; i < 16; i++ )
            {
                if ( js.rgbButtons[i] & 0x80 )
                {
                  //std::cout << "Button " << i << " pressed." << std::endl;
                  state.buttonPressed( i, true );
                }
                else
                {
                  state.buttonPressed( i, false );
                }
            }

            
            currentState.state ( state );
          }
        }

        ::Sleep( duration );
      }
    }

  };

  Joystick       joystick;

  std::list< DirectXJoystick* > list;
}

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DirectXJoystick , DirectXJoystick::BaseDevice );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DirectXJoystick::DirectXJoystick ( Usul::Interfaces::IUnknown* caller ) :
BaseDevice(),
_trackball ( caller )
{
  if ( Detail::joystick.init() )
  {
    if( Detail::list.empty() )
      Detail::joystick.start();

    FoxTools::Functions::application()->addTimeout ( this, 3, 75 );
  }

  Detail::list.push_back( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DirectXJoystick::~DirectXJoystick()
{
}




///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* DirectXJoystick::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case IDevice::IID:
    return static_cast < IDevice* > ( this );
  default:
    return 0x0;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear internal data.
//
///////////////////////////////////////////////////////////////////////////////

void DirectXJoystick::clear()
{
  FoxTools::Functions::application()->removeTimeout ( this, 3 );

  // This cannot be in the destructor because of the circular references.
  // Remove self from the list of DirectXJoystick devices.
  Detail::list.remove ( this );

  // If there are no other DirectXJoystick objects...
  if( Detail::list.empty() )
  {
    // Tell the joystick to clean up.
    Detail::joystick.cleanUp();

    // Cancel the thread.
    Detail::joystick.cancel();
  }

  // Clear the listeners.
  Usul::Devices::BaseDevice::clear();

  // Unref the viewer.
  _trackball = static_cast < Usul::Interfaces::ITrackball* > ( 0x0 );
}


long DirectXJoystick::onTimeout ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  Usul::Devices::State state ( Detail::currentState.state() );

  float lastX ( _lastState.x() );
  float lastY ( _lastState.y() );
  float lastZ ( _lastState.z() );

  float currentX ( state.x() );
  float currentY ( state.y() );
  float currentZ ( state.z() );

  static bool pause ( false );
  static float speed ( 0.0 );
  const float topSpeed ( 0.1 );

  // Speed up.
  if( state.buttonPressed( 5 ) )
  {
    if( speed != topSpeed )
      speed += 0.01;
  }

  // Slow down.
  if( state.buttonPressed( 4 ) )
  {
    if ( speed != -topSpeed )
      speed -= 0.01;
  }

  // Start stop navigating.
  if ( state.buttonPressed( 9 ) && !_lastState.buttonPressed( 9 ) )
  {
    pause = !pause;
  }

  // Stop.
  if ( state.buttonPressed( 8 ) && !_lastState.buttonPressed( 8 ) )
  {
    speed = 0.0;
  }

  bool moveTrackball ( false );

  // How close to zero is considered not moving.
  const float tolerencePercent ( 0.01 );
  const float tolerence ( Detail::maxJoystickValue * tolerencePercent );

  
  if( currentY < -tolerence || currentY > tolerence || 
      currentX < -tolerence || currentX > tolerence || 
      currentZ < -tolerence || currentZ > tolerence )
  {
    moveTrackball = true;
  }

  float distance   ( _trackball->getDistance() );
  osg::Quat rot    ( _trackball->getRotation() );
  osg::Vec3 center ( _trackball->getCenter()   );

  if( moveTrackball && !pause )
  {
    double angle;
    osg::Vec3 axis;
    rot.getRotate( angle, axis );

  #if 1
    osg::Matrix rotation_matrix( rot );


    osg::Vec3 uv = osg::Vec3(0.0f,1.0f,0.0f)*rotation_matrix;
    osg::Vec3 sv = osg::Vec3(1.0f,0.0f,0.0f)*rotation_matrix;

    double yaw ( currentZ);
    double pitch ( -( currentY ) );

    osg::Matrix m1;
    m1.makeRotate( pitch, sv );

    osg::Matrix m2;
    m2.makeRotate( yaw, uv );

    //axis = axis * m1;
    //axis = axis * m2;
    //axis.normalize();

    center.y() = center.y() - pitch * 2;
    center.x() = center.x() + yaw * 2;

  #endif
    

  #if 0
    float angle_;

    float px0 = 0.0; //currentX / 10;
    float py0 = 0.0; //lastY / 10;
          
    float px1 = currentZ / 10;
    float py1 = -currentY / 10;
    
    //rot.makeRotate( angle, axis );

    this->_getTrackballInfo( rot, axis, angle_, px1,py1,px0,py0);

  #endif
    angle += osg::DegreesToRadians( ( currentX / 20 ) );

  #if 1
    if ( angle < 0.0 )
    {
      float diff = 0.0 - angle;
      angle = osg::DegreesToRadians( 360.0 ) - diff;
    }

    if( angle > osg::DegreesToRadians( 360.0 ) )
    {
      float diff = angle - osg::DegreesToRadians( 360.0 );
      angle = diff;
    }
  #endif
    //std::cout << "Angle: " << angle << std::endl;

    rot.makeRotate( angle, axis );

  }

  if ( speed != 0.0f && !pause)
  {
    Usul::Interfaces::IGetBoundingBox::ValidQueryPtr getBound ( _trackball );
    osg::BoundingBox bb ( getBound->getBoundingBox() );

    distance -= ( speed * bb.radius() );

    moveTrackball = true;
  }

  // Render if the trackball moved
  if( moveTrackball && !pause )
  {
    // Set the trackball with the new values.
    _trackball->setTrackball( center, distance, rot, true, true );

    // Render the scene.
    Usul::Interfaces::IViewer::ValidQueryPtr viewer ( _trackball );
    viewer->render();
  }

  // Loop through the buttons.
  for ( Usul::Devices::State::const_iterator buttonIter = _lastState.begin(); buttonIter != _lastState.end(); ++buttonIter )
  {
    unsigned int button ( buttonIter->first );
    bool lastState ( _lastState.buttonPressed( button ) );
    bool currentState ( state.buttonPressed( button ) );

    // if it wasn't pressed before, but it is now...
    if ( false == lastState && true == currentState )
    {
      this->_notifyButtonPressed( button, state );
    }

    // if it wast pressed before, but isn't is now...
    if ( true == lastState && false == currentState )
    {
      this->_notifyButtonReleased( button, state );
    }
  }
    

  

  if( lastX != currentX || lastY != currentY )
  {
    this->_notifyMotionListeners( state );
  }

            
  _lastState = state;          

  FoxTools::Functions::application()->addTimeout ( this, 3, 75 );
  return 1;
}


void DirectXJoystick::_getTrackballInfo(const osg::Quat& rot, osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y)
{
    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */

    osg::Matrix rotation_matrix( rot );


    osg::Vec3 uv = osg::Vec3(0.0f,1.0f,0.0f)*rotation_matrix;
    osg::Vec3 sv = osg::Vec3(1.0f,0.0f,0.0f)*rotation_matrix;
    osg::Vec3 lv = osg::Vec3(0.0f,0.0f,-1.0f)*rotation_matrix;

    osg::Vec3 p1 = sv * p1x + uv * p1y - lv * _tb_project_to_sphere(0.8f, p1x, p1y);
    osg::Vec3 p2 = sv * p2x + uv * p2y - lv * _tb_project_to_sphere(0.8f, p2x, p2y);

    /*
     *  Now, we want the cross product of P1 and P2
     */

// Robert,
//
// This was the quick 'n' dirty  fix to get the trackball doing the right 
// thing after fixing the Quat rotations to be right-handed.  You may want
// to do something more elegant.
//   axis = p1^p2;
axis = p2^p1;
    axis.normalize();

    /*
     *  Figure out how much to rotate around that axis.
     */
    float t = (p2 - p1).length() / (2.0 * 0.8f);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    angle = osg::inRadians(asin(t));

}


/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
float DirectXJoystick::_tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;

    d = sqrt(x*x + y*y);
                                 /* Inside sphere */
    if (d < r * 0.70710678118654752440)
    {
        z = sqrt(r*r - d*d);
    }                            /* On hyperbola */
    else
    {
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}
