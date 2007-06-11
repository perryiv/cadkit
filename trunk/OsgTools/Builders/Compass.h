
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_BUILDERS_COMPASS_H_
#define _OSG_TOOLS_BUILDERS_COMPASS_H_

#include "osg/Group"
#include <string>
#include "OsgTools/Export.h"



//#if _MSC_VER
//  #include <osgViewer/Viewer>
//#endif
namespace osg { class MatrixTransform; class Geode; }
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
  void setHeadingRelativeToNorth(const osg::Vec2& north, const osg::Vec2& pos);

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
};


} // Namespace Builders
} // Namespace OsgTools


#endif // _OSG_TOOLS_BUILDERS_COMPASS_H_
