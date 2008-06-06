
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for generic model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_OBJECTS_MODEL_H__
#define __MINERVA_CORE_DATA_OBJECTS_MODEL_H__

#include "Minerva/Core/Geometry/Geometry.h"

#include "osg/Vec3"
#include "osg/Node"

namespace Minerva {
namespace Core {
namespace Geometry {
      

class MINERVA_EXPORT Model : public Minerva::Core::Geometry::Geometry
{
public:
  typedef Minerva::Core::Geometry::Geometry BaseClass;
  
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Model );
  
  Model();
  
  /// Get/Set the location.
  void                  location ( const osg::Vec3& );
  osg::Vec3             location() const;
  
  /// Get/Set the orientation.
  void                  orientation( double  heading, double  tilt, double  roll );
  void                  orientation( double& heading, double& tilt, double &roll ) const;
  
  /// Get/Set the scale.
  void                  scale ( const osg::Vec3& );
  osg::Vec3             scale() const;
  
  /// Get/Set the model.
  void                  model ( osg::Node* );
  osg::Node*            model() const;
  
protected:
  virtual ~Model();
  
  virtual osg::Node*    _buildScene( Usul::Interfaces::IUnknown* caller );

private:
  osg::Vec3 _location;
  double _heading;
  double _tilt;
  double _roll;
  osg::Vec3 _scale;
  osg::ref_ptr<osg::Node> _model;
};


}
}
}


#endif // __MINERVA_CORE_DATA_OBJECTS_MODEL_H__
