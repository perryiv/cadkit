
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ossimPlanetCompass_HEADER
#define ossimPlanetCompass_HEADER

#include "StarSystem/Export.h"

#include "osg/Group"

#include <string>

namespace osg { class MatrixTransform; class Geode; }

namespace StarSystem {

class STAR_SYSTEM_EXPORT Compass : public osg::Group
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

  osg::Geode*            _buildTriangleFan ( const std::string& tex, float zoff, unsigned int render_level );

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

}

#endif // ossimPlanetCompass_HEADER
