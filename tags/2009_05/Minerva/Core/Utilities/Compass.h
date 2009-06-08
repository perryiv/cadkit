
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_UTILITIES_COMPASS_H_
#define _MINERVA_CORE_UTILITIES_COMPASS_H_

#include "Minerva/Core/Export.h"

#include "osg/Group"

#include <string>

namespace osg { class MatrixTransform; class Geode; }

namespace Minerva {
namespace Core {
namespace Utilities {

class MINERVA_EXPORT Compass : public osg::Group
{
public:
  typedef osg::Group BaseClass;
  
  Compass();

  virtual void traverse(osg::NodeVisitor& nv);
  
  float heading() const;
  void heading ( float r );
  
  float pitch() const;
  void pitch ( float p );
   
  float roll() const;
  void roll(float r);
  
  const osg::Vec3& position() const;
  void position ( const osg::Vec3& p );
   
  float scale() const;
  void  scale ( float s );

  void updateCompass ( );
   
protected:
        
  virtual ~Compass();
  
  void                   _buildCompass();

private:
        
	osg::Vec3 _pos;
	float _scale;
  std::string _ring;
	std::string _interior;
	unsigned int _numslices;
	float _radius;
	osg::ref_ptr<osg::MatrixTransform> _compassRingObject;
	osg::ref_ptr<osg::MatrixTransform> _compassInteriorObject;
	
	double _heading, _pitch, _roll;
  osg::ref_ptr< osg::Vec4Array > _colors;
};

} // namespace Utilities
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_UTILITIES_COMPASS_H_

