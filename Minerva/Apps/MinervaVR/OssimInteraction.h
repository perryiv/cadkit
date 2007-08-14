
#ifndef __OSSIMINTERACTION_H__
#define __OSSIMINTERACTION_H__

#ifdef HAVE_UNISTD_H
  #undef HAVE_UNISTD_H
#endif

#include "VrjCore/OsgInteraction.h"
#include "VrjMath/Function.h"
#include "VrjMath/ExpFunction.h"
#include "VrjUtil/OsgVrjUtil.h"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetNavigator.h"

#include "ossimPlanet/ossimPlanetAction.h"
#include "ossimPlanet/ossimPlanetLand.h"
#include "ossimPlanet/ossimPlanetLandModel.h"
#include "ossimPlanet/mkUtils.h"
#include "ossim/base/ossimEnvironmentUtility.h"
#include "ossimPlanet/ossimPlanetActionRouter.h"

#include "OsgTools/Builders/Compass.h"

#include "Minerva/Document/MinervaDocument.h"

#ifdef min
  #undef min
#endif

#include "ossimPlanet/ossimPlanetInteractionController.h"

namespace VrjCore
{
  struct EventAdapter
  {  
    enum MouseProxy
    {
      LEFT_BUTTON     = 0, 
      MIDDLE_BUTTON, 
      RIGHT_BUTTON, 
      MP_NONE
    };

    enum EventType
    {
      PUSH    = 0, 
      RELEASE, 
      DRAG, 
      HOLD,
      ET_NONE
    };    

    EventAdapter()
    {
    }

    EventAdapter( MouseProxy mp, EventType et, const float& x, const float& y )
    {
      mMouseProxy = mp;
      mEventType  = et;
      mAnalogValX = x, 
      mAnalogValY = y;
    } 

    ~EventAdapter()
    {
    }

    float                                 mAnalogValX;
    float                                 mAnalogValY;

    float                                 mLastAnalogValX;
    float                                 mLastAnalogValY;

    MouseProxy                            mMouseProxy;
    EventType                             mEventType;
  };

  class OssimInteraction : public OsgInteraction
  {
    public:             
      typedef std::map< unsigned int, EventAdapter >  EventMap;

      OssimInteraction( Minerva::Document::MinervaDocument* document ) :
        mPlanet ( 0x0 ),
        mNavigator ( 0x0 ),
        _home (),
        _homeLook(),
        _compass ( new OsgTools::Builders::Compass ),
        mLeftButtonEvent(),
        mMiddleButtonEvent(),
        mRightButtonEvent(),
        mEventMap(),
        _document ( document )
	{
	  _compass->setRotateByViewMatrix ( false );
	  //_compass->setPitch ( 0.0 );
		//mAnalogValX = 0.0;
   //mAnalogValY = 0.0;
   mPlanet = 0x00;
   mNavigator = new ossimPlanetNavigator(new ossimPlanetPrimaryBody("earth_wgs84",
                                                                      6378137.0,
                                                                     6356752.3142,
                                                                      86400,
                                                                      5.9742e24,
                                                                      -180,
                                                                     180));   
   // make sure the controller has an instance
   ossimPlanetInteractionController* iac = ossimPlanetInteractionController::instance();
   

   iac->defineInteractionValuator("LON_VALUATOR", -1.0f, 1.0f);
   iac->defineInteractionValuator("LATZOOM_VALUATOR", -1.0f, 1.0f);

   // initialize bindings
   ossimFilename path = ossimEnvironmentUtility::instance()->getUserOssimSupportDir();
   path = path.dirCat("planet");
   path = path.dirCat("binds.act");
   if(!path.exists())
   {
      path = ossimEnvironmentUtility::instance()->getInstalledOssimSupportDir();
      path = path.dirCat("planet");
      path = path.dirCat("binds.act");
   }
   if(path.exists())
   {
      ossimPlanetActionRouter::instance()->executeFile(path);
   }
   else
   {
      ossimPlanetAction(":iac bind left_mousedown     {:navigator rotatestart}").execute();
      ossimPlanetAction(":iac tie x_mouse LON_VALUATOR").execute();
      ossimPlanetAction(":iac tie y_mouse LATZOOM_VALUATOR").execute();
      ossimPlanetAction(":iac bind scroll_up          {:navigator gotoelev 2.0 .5}").execute();
      ossimPlanetAction(":iac bind scroll_down        {:navigator gotoelev .5 .5}").execute();
      ossimPlanetAction(":iac bind scroll_left        {:navigator gotoelev 2.0 .5}").execute();
      ossimPlanetAction(":iac bind scroll_right       {:navigator gotoelev .5 .5}").execute();
      ossimPlanetAction(":iac bind left_mousedown     {:navigator rotatestart}").execute();
      ossimPlanetAction(":iac bind left_mouseup       {:navigator rotatestop}").execute();
      ossimPlanetAction(":iac bind shift_left_mousedown    {:navigator loslookstart}").execute();
      ossimPlanetAction(":iac bind shift_left_mouseup      {:navigator loslookstop}").execute();
      ossimPlanetAction(":iac bind middle_mousedown  {:navigator zoomstart}").execute();
      ossimPlanetAction(":iac bind middle_mouseup    {:navigator zoomstop}").execute();
      ossimPlanetAction(":iac bind right_mousedown   {:navigator lookstart}").execute();
      ossimPlanetAction(":iac bind right_mouseup     {:navigator lookstop}").execute();
      ossimPlanetAction(":iac bind right_key            {:navigator flystart -.015625 0.0}").execute();
      ossimPlanetAction(":iac bind left_key          {:navigator flystart .015625 0.0}").execute();
      ossimPlanetAction(":iac bind up_key          {:navigator flystart 0.0 -.015625}").execute();
      ossimPlanetAction(":iac bind down_key         {:navigator flystart 0.0 .015625}").execute();
      ossimPlanetAction(":iac bind u_key 	    {:navigator resetzoomlook}").execute();
      ossimPlanetAction(":iac bind p_key     	    {:navigator printlatlonelev}").execute();
      ossimPlanetAction(":iac bind space_key         {:navigator reset}").execute();
      ossimPlanetAction(":iac bind return_key        {:navigator stop}").execute();
      // doing default bindings      
   }

   mLeftButtonEvent   = VrjCore::EventAdapter( VrjCore::EventAdapter::LEFT_BUTTON,    VrjCore::EventAdapter::ET_NONE, 0.0f, 0.0f );
   mMiddleButtonEvent = VrjCore::EventAdapter( VrjCore::EventAdapter::MIDDLE_BUTTON,  VrjCore::EventAdapter::ET_NONE, 0.0f, 0.0f );
   mRightButtonEvent  = VrjCore::EventAdapter( VrjCore::EventAdapter::RIGHT_BUTTON,   VrjCore::EventAdapter::ET_NONE, 0.0f, 0.0f );

   mEventMap[0] = mLeftButtonEvent;
   mEventMap[1] = mMiddleButtonEvent;
   mEventMap[2] = mRightButtonEvent;

   mFunction =  new VrjMath::ExpFunction( 4.0 );
	}

      ~OssimInteraction(){};

      void                                  planet( ossimPlanet* );

      virtual void                          matrix( const osg::Matrixd& );
      virtual osg::Matrixd                  matrix();


      virtual void                          joystickAxis1GlobalAction( double );
      virtual void                          joystickAxis2GlobalAction( double );
      virtual void                          joystickAxis3GlobalAction( double );
      virtual void                          joystickAxis4GlobalAction( double );
      virtual void                          joystickAxis5GlobalAction( double );
      virtual void                          joystickAxis6GlobalAction( double );

      virtual void                          updateDeviceData();

      virtual void                          updateTransforms();

      virtual void                          resetNavigation();
      virtual void                          resetState();

      void                                  setHomePosition ( double lat, double lon, double height );
      void                                  setHomeLook ( double yaw, double pitch, double roll );
      void                                  goHome();

      osg::Node*                            compass () { return _compass->getCompass(); }

      virtual void joystickButton5GlobalAction(DeviceData d_data)
        {
          _document->startAnimationCommand();
        }
			virtual void joystickButton6GlobalAction(DeviceData d_data)
        {
          _document->stopAnimationCommand();
        }

			virtual void joystickButton7GlobalAction(DeviceData d_data)
        {
          double speed ( _document->sceneManager()->animationSpeed () );
          _document->sceneManager()->animationSpeed ( speed * 2.0 );
        }

			virtual void joystickButton8GlobalAction(DeviceData d_data)
        {
          double speed ( _document->sceneManager()->animationSpeed () );
          _document->sceneManager()->animationSpeed ( speed / 2.0 );
        }

    protected:      

      osg::ref_ptr< ossimPlanet >           mPlanet;
      ossimPlanetNavigator*                 mNavigator;
      ossimGpt                              _home;
      osg::Vec3d                            _homeLook;
      osg::ref_ptr < OsgTools::Builders::Compass >  _compass;


      EventAdapter                           mLeftButtonEvent;
      EventAdapter                           mMiddleButtonEvent;
      EventAdapter                           mRightButtonEvent;

      EventMap                               mEventMap;
      Minerva::Document::MinervaDocument::RefPtr  _document;
  };  
}

inline void VrjCore::OssimInteraction::planet( ossimPlanet* planet )
{
  mPlanet = planet;
  mNavigator->setLandModel(mPlanet->getLand()->getModel().get());
}

inline void VrjCore::OssimInteraction::matrix( const osg::Matrixd& matrix )
{
   if( !mPlanet )
   {
      ossimPlanetAction(":navigator reset").execute();
      return;
   }

   osg::Vec3d eye( matrix( 3,0 ),matrix( 3,1 ),matrix( 3,2 ) );
   osg::Vec3d latLonHeight;
   osg::Matrixd lsrMatrix;
   mPlanet->getLand()->getModel()->inverse(eye, latLonHeight);
   mPlanet->getLand()->getModel()->lsrMatrix(latLonHeight, lsrMatrix);

   if(!mkUtils::isNAN(latLonHeight[0])&&
      !mkUtils::isNAN(latLonHeight[1])&&
      !mkUtils::isNAN(latLonHeight[2]))
   {
      osg::Vec3d hpr;
      mNavigator->setLatLonHeight(latLonHeight[0],
                                    latLonHeight[1],
                                    latLonHeight[2]*mPlanet->getLand()->getModel()->getNormalizationScale());
      

      /*
      if(mkUtils::matrixToHpr(hpr, lsrMatrix, matrix))
      {
         if(!mkUtils::isNAN(hpr[0])&&
            !mkUtils::isNAN(hpr[1])&&
            !mkUtils::isNAN(hpr[2]))
         {
            if(ossimAbs(hpr[0]) < FLT_EPSILON)
            {
               hpr[0] = 0.0;
            }
            if(ossimAbs(hpr[1]) < FLT_EPSILON)
            {
               hpr[1] = 0.0;
            }
            if(ossimAbs(hpr[2]) < FLT_EPSILON)
            {
               hpr[2] = 0.0;
            }
            
            mNavigator->orientation()[0] = hpr[0];
            mNavigator->orientation()[1] = hpr[1];
            mNavigator->orientation()[2] = hpr[2];
         }
      }
      */
   }
   
   
   mNavigator->update();
}

inline osg::Matrixd VrjCore::OssimInteraction::matrix() 
{
  //mNavigator->update();
  return mNavigator->viewMatrix();
}

inline void VrjCore::OssimInteraction::joystickAxis1GlobalAction( double data )
{ 
  if( mFunction != 0x00 )
  {
    mEventMap[1].mAnalogValX    = mFunction->f( data, data );
  }
  else
  {
    mEventMap[1].mAnalogValX    = data;
  }  
}

inline void VrjCore::OssimInteraction::joystickAxis2GlobalAction( double data )
{ 
  data = -data;
  if( mFunction != 0x00 )
  {
    mEventMap[1].mAnalogValY    = mFunction->f( data, data );
  }
  else
  {
    mEventMap[1].mAnalogValY    = data;
  }
}

inline void VrjCore::OssimInteraction::joystickAxis3GlobalAction( double data )
{
  	

  //if ( data != 0.0 )
  //{
	//data = ( 0.025 * data ) / fabs( data ); 
	data =  0.25 * data;
  //}
  
  //std::cout << "Data after compression is: " << data << std::endl;

  if( mFunction != 0x00 )
  {
    mEventMap[2].mAnalogValY    = data;
  }
  else
  {
    mEventMap[2].mAnalogValY    = data;
  }  
}

inline void VrjCore::OssimInteraction::joystickAxis4GlobalAction( double data )
{  
  data *= -1.0;
  if( mFunction != 0x00 )
  {
    mEventMap[2].mAnalogValX      = mFunction->f( data, data );
  }
  else
  { 
    mEventMap[2].mAnalogValX      = data;
  }
}

inline void VrjCore::OssimInteraction::joystickAxis5GlobalAction( double data )
{ 
  if( mFunction != 0x00 )
  {
    mEventMap[0].mAnalogValX      = mFunction->f( data, data );
  }
  else
  {
    mEventMap[0].mAnalogValX      = data;
  }
}

inline void VrjCore::OssimInteraction::joystickAxis6GlobalAction( double data )
{
  if( mFunction != 0x00 )
  {
    mEventMap[0].mAnalogValY = mFunction->f( data, data );
  }
  else
  {
    mEventMap[0].mAnalogValY      = data;
  }  
}

inline void VrjCore::OssimInteraction::updateDeviceData()
{  
  OsgInteraction::updateDeviceData();

  if( DtSharedData::mCommand.isLocal() )
  {
    ossimString modKeyString("");
    ossimPlanetInteractionController* iac = ossimPlanetInteractionController::instance();  

    //iac->defineInteractionValuator("LON_VALUATOR", -1.0f, 1.0f);
    //iac->defineInteractionValuator("LATZOOM_VALUATOR", -1.0f, 1.0f);

    EventMap::iterator itr;
    for( itr = mEventMap.begin(); itr != mEventMap.end(); ++itr )
  {
      if ( ( itr->second.mAnalogValX != 0.0 && itr->second.mLastAnalogValX == 0.0 ) ||
           ( itr->second.mAnalogValY != 0.0 && itr->second.mLastAnalogValY == 0.0 )
      )
      {
        itr->second.mEventType = EventAdapter::PUSH;
      }    
      else if( ( itr->second.mAnalogValX != 0.0 && itr->second.mLastAnalogValX == itr->second.mAnalogValX ) ||
        ( itr->second.mAnalogValY != 0.0 && itr->second.mLastAnalogValY == itr->second.mAnalogValY )
      )
      {
        itr->second.mEventType = EventAdapter::HOLD;
      }
      else if ( ( itr->second.mAnalogValX == 0.0 && itr->second.mLastAnalogValX != 0.0  ) ||
                ( itr->second.mAnalogValY == 0.0 && itr->second.mLastAnalogValY != 0.0  )
      )
      {
        itr->second.mEventType = EventAdapter::RELEASE;
        //itr->second.mLastAnalogValX = 0.0;
        //itr->second.mLastAnalogValY = 0.0;
      }
      else
      {
        itr->second.mEventType = EventAdapter::ET_NONE;
      }
      
      itr->second.mLastAnalogValX = itr->second.mAnalogValX;
      itr->second.mLastAnalogValY = itr->second.mAnalogValY;

      if( itr->second.mEventType == EventAdapter::PUSH )
      {
        iac->updateInteractionValuators("x_mouse", itr->second.mAnalogValX );
        iac->updateInteractionValuators("y_mouse", itr->second.mAnalogValY );

        switch( itr->second.mMouseProxy )
        {
          case EventAdapter::LEFT_BUTTON:
          {
            iac->defineInteractionValuator("LON_VALUATOR", -1.0f, 1.0f);
            iac->defineInteractionValuator("LATZOOM_VALUATOR", -1.0f, 1.0f);
            iac->executeBoundAction((modKeyString + "left_mousedown").c_str());
            break;
          }
          case EventAdapter::MIDDLE_BUTTON:
          {
            iac->executeBoundAction((modKeyString + "middle_mousedown").c_str());
            break;
          }
          case EventAdapter::RIGHT_BUTTON:
          {	
            iac->executeBoundAction((modKeyString + "right_mousedown").c_str());
            break;
          }
        }
      }
      else if( itr->second.mEventType == EventAdapter::RELEASE )
      {
        iac->updateInteractionValuators("x_mouse", itr->second.mAnalogValX );
        iac->updateInteractionValuators("y_mouse", itr->second.mAnalogValY );
        switch( itr->second.mMouseProxy )
        {
          case EventAdapter::LEFT_BUTTON:
          {
            iac->executeBoundAction((modKeyString + "left_mouseup").c_str());
            break;
          }
          case EventAdapter::MIDDLE_BUTTON:
          {
            iac->executeBoundAction((modKeyString + "middle_mouseup").c_str());
            break;
          }
          case EventAdapter::RIGHT_BUTTON:
          {
            iac->executeBoundAction((modKeyString+ "right_mouseup").c_str());
            break;
          }
        }
      }
      else if( itr->second.mEventType == EventAdapter::HOLD )
      {
        iac->updateInteractionValuators("x_mouse", itr->second.mAnalogValX );
        iac->updateInteractionValuators("y_mouse", itr->second.mAnalogValY );
      }
    }
    mNavigator->update();

    //    _compass->setHeading( -mNavigator->orientation().x() );
    //_compass->setHeadingRelativeToNorth( osg::Vec2 ( 0, 90 ), osg::Vec2 ( mNavigator->lon(), mNavigator->lat() ) );
    _compass->setHeading( -mNavigator->orientation().x() );
    _compass->setPitch ( -mNavigator->orientation().y() );
    _compass->updateCompass();
 
    gmtl::Matrix< float , 4 ,4 >  m;
    DtSharedData::mCommand->mSharedTransformMatrix = VrjUtil::OsgVrjUtil::convertOsgToGmtl( m, matrix() );
  }
}

inline void VrjCore::OssimInteraction::updateTransforms()
{
  if( mSceneTransformNode.valid() )
  {
    osg::Matrix mat( DtSharedData::mCommand->mSharedTransformMatrix.mData );
    mat.postMult( osg::Matrix::rotate( osg::DegreesToRadians( -90.0 ), 1.0, 0.0, 0.0 ) );
    mSceneTransformNode->setMatrix( osg::Matrix::inverse( mat ) ); 
  }
}

inline void VrjCore::OssimInteraction::resetState()
{
  this->goHome();
}

inline void VrjCore::OssimInteraction::setHomePosition ( double lat, double lon, double height )
{
  _home.lat = lat;
  _home.lon = lon;
  _home.hgt = height;
}

inline void VrjCore::OssimInteraction::setHomeLook ( double yaw, double pitch, double roll )
{
  _homeLook.x() = yaw;
  _homeLook.y() = pitch;
  _homeLook.z() = roll;
}


void VrjCore::OssimInteraction::goHome()
{
  mNavigator->setLatLonHeight( _home.lat, _home.lon, _home.hgt );
  mNavigator->orientation() = _homeLook;
  mNavigator->update();
}

void VrjCore::OssimInteraction::resetNavigation()
{
  this->goHome();
}

#endif //__OSSIMINTERACTION_H__

