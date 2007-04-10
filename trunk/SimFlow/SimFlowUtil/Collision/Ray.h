
#ifndef __COLLISION_RAY_H__
#define __COLLISION_RAY_H__

#include <string>

#include "SimFlowUtil/Export.h"

#include "osg/Vec3f"
#include "osg/Referenced"

namespace SimFlow
{
  namespace SimFlowUtil
  {
    namespace Collision
    {    
      class SIMFLOWUTIL_EXPORT Ray
	    {
		  public:
			  
        Ray();
  			Ray( osg::Vec3f, osg::Vec3f, bool active=true );
        Ray( std::string, osg::Vec3f, osg::Vec3f, bool active=true );			

			  ~Ray();

        std::string     ID();
        void            ID( std::string );

        bool				    active();
			  void				    active( bool );
			  
        osg::Vec3f	    startPoint();			
			  void				    startPoint( osg::Vec3f );

        osg::Vec3f	    endPoint();	
			  void				    endPoint( osg::Vec3f );	

			  osg::Vec3f	    baseStartPoint();	
			  void				    baseStartPoint( osg::Vec3f );	

        osg::Vec3f	    baseEndPoint();	
			  void				    baseEndPoint( osg::Vec3f );	
			  
        osg::Vec3f	    worldIntersectPoint();
			  void				    worldIntersectPoint( osg::Vec3f );				
			  
        osg::Vec3f		  localIntersectPoint();
			  void				    localIntersectPoint( osg::Vec3f );			  

        osg::Vec3f			worldIntersectNormal();
			  void				    worldIntersectNormal( osg::Vec3f );
			  
        osg::Vec3f			localIntersectNormal();
			  void				    localIntersectNormal( osg::Vec3f );			  

	    private:

        std::string	    mID;

        bool				    mActive;

		    osg::Vec3f		  mStartPoint;
		    osg::Vec3f			mEndPoint; 

		    osg::Vec3f			mBaseStartPoint;
		    osg::Vec3f			mBaseEndPoint; 

		    osg::Vec3f			mWorldIntersectPoint;
		    osg::Vec3f			mWorldIntersectNormal;

		    osg::Vec3f			mLocalIntersectPoint;  
		    osg::Vec3f			mLocalIntersectNormal; 		    
	    };
    }
  }
}

#endif // __COLLISION_RAY_H__
