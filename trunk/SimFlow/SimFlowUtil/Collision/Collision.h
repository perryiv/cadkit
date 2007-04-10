
#ifndef __COLLISION_COLLISION_H__
#define __COLLISION_COLLISION_H__

#include <vector>

#include "gmtl/Math.h"
#include "gmtl/Matrix.h"
#include "gmtl/MatrixOps.h"

#include "osg/Vec3f"
#include "osg/Group"
#include "osg/LineSegment"
#include "osgUtil/IntersectVisitor"

#include "SimFlowUtil/Collision/Ray.h"

#include "SimFlowUtil/Export.h"

namespace SimFlow
{
  namespace SimFlowUtil
  {
    namespace Collision
    {
      class SIMFLOWUTIL_EXPORT Collision
	    {
		  public:            
			
        // Easy to remember names. 
        typedef std::vector< Ray >		Rays;

        enum Scope
        {
          GLOBAL = 0,
          LOCAL 
        };

        /*
        enum SearchMode
        {
          IGNORE = 0, 
          FIND
        };
        */

			  Collision();
			
			  ~Collision();	
			
        Ray&              ray( unsigned int );
        Ray&              ray( std::string );        
        Rays							rays();

      	void							ray( Ray );			    
  
        void							testCollision( osg::Node*, Ray&, Scope=GLOBAL, std::string="", unsigned int=1 );

        void							testCollision( osg::Node*, std::vector< Ray >&, Scope=GLOBAL, std::string="", unsigned int=1 );

        void							testCollision( osg::Node*, Ray&, std::vector< osg::Node* >, Scope=GLOBAL, std::string="", unsigned int=1 );

        void              getHits( osg::Node*, osg::LineSegment*, osgUtil::IntersectVisitor::HitList& );

			  void							update();			  

		  private:
				Rays      				mRays;		
			  
			  osg::Vec3f				mCurrDistance;
			  osg::Vec3f				mOrigDistance;   						

			  gmtl::Matrix44f		mPitchMatrix;
			  gmtl::Matrix44f		mWorldMatrix;			  
      };
    }
  }
}

#endif // __COLLISION_COLLISION_H__
