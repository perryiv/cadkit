///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPASS_H
#define COMPASS_H


#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/Math>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include "OsgTools/Export.h"

#if _MSC_VER
  #include <osgViewer/Viewer>
#endif

namespace OsgTools {
	namespace Builders {

class OSG_TOOLS_EXPORT Compass : public osg::Referenced
{
public:

	Compass ( );
	virtual ~Compass ( );
	
	void setTextureFilename ( const std::string& fn );
	osg::Node* getCompass ( );
	
	bool isAnimating ( );
	void setAnimation(bool state);

	float getHeading ( );
	void setHeading ( float r );

  float getPitch ( );
	void setPitch ( float p );

	void setRotateByViewMatrix ( bool m );

	const osg::Vec3& getPosition ( );
	void setPotion ( const osg::Vec3& p );
  void setPositionByPercentage ( float width, float height );

	float getScale ( );
	void setScale ( float s );

	bool isVisible ( );
	
	void buildCompass ( );
	void keyChange ( int key,int value );
	
	void updateCompass ( );

protected:
	osg::Node* _buildCompassObject ( );
	osg::MatrixTransform*  _initCompass ( osg::Node* geode );
	
	void _showCompass ( );
	void _hideCompass ( );
	void _emptyCompass ( );
  osg::Geode* _buildTriangleFan ( std::string tex, float zoff, unsigned int render_level, const osg::Vec3& n );
	
private:
	
	osg::Vec3 _pos;
	float _scale;
	std::string _topTexfn,_botTexfn;
	unsigned int _numslices;
	float _radius;
	osg::ref_ptr<osg::Group> _compassGroup;
	osg::ref_ptr<osg::Node> _compassObject;

	bool _showOrHideCompass, _animating;
	bool _rotateByView;
	float _animationStart, _animationEnd, _animationStep;
	
	float _heading, _pitch;	

}; //Compass

} //Namespace Builders
} //Namespace OsgTools
#endif
