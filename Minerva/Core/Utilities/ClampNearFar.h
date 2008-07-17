
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to ensure that clipping values are sane.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_CLAMP_NEAR_FAR_H_
#define _MINERVA_CORE_CLAMP_NEAR_FAR_H_

#include "osgUtil/CullVisitor"

namespace Minerva  {
namespace Core {
namespace Utilities {
  
  struct ClampNearFar : public osg::CullSettings::ClampProjectionMatrixCallback
  {
    typedef osg::CullSettings::ClampProjectionMatrixCallback BaseClass;
    
    ClampNearFar ( osgUtil::CullVisitor &cv ) : BaseClass(), _cv ( cv )
    {
    }
    
    virtual bool clampProjectionMatrixImplementation ( osg::Matrixf &projection, double &zNear, double &zFar ) const
    {
      return this->_common ( projection, zNear, zFar );
    }
    
    virtual bool clampProjectionMatrixImplementation ( osg::Matrixd &projection, double &zNear, double &zFar ) const
    {
      return this->_common ( projection, zNear, zFar );
    }
    
  protected:
    
    template < class Matrix > bool _common ( Matrix &projection, double &zNear, double &zFar ) const
    {
      if ( zNear <= 0 )
      {
        zNear = 10;
      }
      
      zFar *= 1.5;

      typedef typename Matrix::value_type value_type;
      
      const value_type trans_near_plane = (-zNear*projection(2,2)+projection(3,2))/(-zNear*projection(2,3)+projection(3,3));
      const value_type trans_far_plane = (-zFar*projection(2,2)+projection(3,2))/(-zFar*projection(2,3)+projection(3,3));
      
      const value_type difference ( trans_near_plane - trans_far_plane );
      
      if ( 0.0 != difference )
      {
        value_type ratio = fabs ( 2.0 / ( difference ) );
        value_type center = -(trans_near_plane+trans_far_plane) / 2.0;
      
        projection.postMult(osg::Matrix(1.0f,0.0f,0.0f,0.0f,
                                      0.0f,1.0f,0.0f,0.0f,
                                      0.0f,0.0f,ratio,0.0f,
                                      0.0f,0.0f,center*ratio,1.0f));
      
        return true;
      }
      
      // Ask cull-visitor to clamp the projection matrix.
      else
        return _cv.clampProjectionMatrixImplementation ( projection, zNear, zFar );
    }
    
    osgUtil::CullVisitor &_cv;
  }; // ClampNearFar.
  
} // namespace Utilities
} // namespace Core
} // namespace Minerva

#endif //_MINERVA_CORE_CLAMP_NEAR_FAR_H_
