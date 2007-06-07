///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Compass.h"

#include "osgUtil/CullVisitor"

#include "Usul/CommandLine/Arguments.h"

using namespace OsgTools::Builders;


///////////////////////////////////////////////////////////////////////////////
//
//  Update Callback for hiding and showing the compass
//
///////////////////////////////////////////////////////////////////////////////

class CompassAnimationCallback : public osg::NodeCallback
{
    public:

        CompassAnimationCallback(Compass* compass, float start, float end, float step)
        {
            _start = start;
			_end = end;
			_step = step;
			_compass = compass;
        }

        virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
        {
            osg::MatrixTransform* transform = dynamic_cast<osg::MatrixTransform*>(node);    
            if(0L != transform)
            {
			        if(_compass->isAnimating())
			        {
				        if(_compass->isVisible())
				        {
					        if (_start < _end)
					        {
						        _start += _step;
						        transform->setMatrix(osg::Matrix::scale(_start,_start,_start));
        						
					        }
					        else
						        _compass->setAnimation(false);
				        }
				        else
				        {
					        if (_start > _end)
					        {
						        _start += _step;
						        transform->setMatrix(osg::Matrix::scale(_start,_start,_start));
        						
					        }
					        else
						        _compass->setAnimation(false);
				        }
			        }
            }   
            traverse(node,nv);            
            
        }
        
    protected:
    
		osg::ref_ptr<Compass> _compass;
        float               _angular_velocity;
		float				_start;
		float				_end;
		float				_step;

};


///////////////////////////////////////////////////////////////////////////////
//
//  Cull Callback for rotating the compass based on the current view matrix
//
///////////////////////////////////////////////////////////////////////////////

class CompassOrientationCallback : public osg::NodeCallback
{
    public:
		CompassOrientationCallback(Compass *compass) :		
		_scale(.07f)		
		{
			_compass = compass;
			_pos = (osg::Vec3(_scale * 4.0f, _scale * -3.0f, -1.0f));
		}
        virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
        {
            osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor *>(nv);
            if( 0L != cv)
            {
                osg::MatrixTransform *tx = dynamic_cast<osg::MatrixTransform *>(node);
                if( 0L != tx )
                {
                    osg::Matrix VM = cv->getState()->getInitialViewMatrix();
					 
					VM(3,0) = 0.0;
                    VM(3,1) = 0.0;
                    VM(3,2) = 0.0;

					osg::Quat quat ( VM.getRotate() );
					quat.set(0,0,quat.z(),quat.w());
					VM.setRotate(quat);

					tx->setMatrix(osg::Matrix::scale(_compass->getScale(),_compass->getScale(),_compass->getScale()));
					tx->postMult(VM);
                    tx->postMult( osg::Matrix::rotate(osg::inDegrees(-70.0f),1.0f,0.0f,0.0f) *
							      osg::Matrix::translate(_compass->getPosition()));
					 
                }
            }
            traverse( node, nv );
        }
		
	 protected:
		 osg::ref_ptr<Compass> _compass;
		 osg::Vec3 _pos;
		 float _scale;
}; 


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
///////////////////////////////////////////////////////////////////////////////

Compass::Compass():
  _topTexfn ( ),
  _botTexfn ( ),
  _rotdeg ( 0.0f ),
  _scale ( .07f ),
  _compassGroup ( new osg::Group() ),
  _showOrHideCompass ( true ),
  _numslices ( 32 ),
  _radius ( .8 ),
  _animating ( false ),
  _rotateByView ( true )
{
	_topTexfn = Usul::CommandLine::Arguments::instance().directory() + "/icons/big_compass_1.tga";
  _botTexfn = Usul::CommandLine::Arguments::instance().directory() + "/icons/big_compass_mask.tga";
	//_pos = (osg::Vec3(_scale * 4.0f, _scale * -3.0f, -1.0f));
  this->setPositionByPercentage( 1.0f, 0.0f );
	_compassObject = _buildCompassObject();
	buildCompass();
}
Compass::~Compass(){}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the state of the compass: visible or not
//
///////////////////////////////////////////////////////////////////////////////

bool Compass::isVisible()
{
	return _showOrHideCompass;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture filename for the compass
//
///////////////////////////////////////////////////////////////////////////////

void Compass::setTextureFilename(const std::string& fn)
{
  _topTexfn = fn;
  buildCompass();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the compass group with object and matrix transformations
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Compass::getCompass()
{
  return _compassGroup.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the amount of rotation of the compass
//
///////////////////////////////////////////////////////////////////////////////

float Compass::getRotation()
{
  return _rotdeg;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the amount of rotation on the compass
//
///////////////////////////////////////////////////////////////////////////////

void Compass::setRotation(float r)
{
  _rotdeg = r;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the position on the screen of the compass
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3& Compass::getPosition()
{
  return _pos;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position on the screen of the compass by absolute postion
//
///////////////////////////////////////////////////////////////////////////////

void Compass::setPotion(const osg::Vec3& p)
{
  _pos = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position on the screen of the compass by percentage of screen
//  Pass in a float value between 0 and 1;
//
///////////////////////////////////////////////////////////////////////////////

void Compass::setPositionByPercentage ( float width, float height )
{
  float w = ( _scale *  width * 8.0f ) - ( 4.0f * _scale );
  float h = ( _scale *  height * 6.0f ) - ( 3.0f * _scale );

  _pos = osg::Vec3(w, h, -1.0f);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the xyz scale value of the compass
//
///////////////////////////////////////////////////////////////////////////////

float Compass::getScale()
{
  return _scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the xyz scale value of the compass
//
///////////////////////////////////////////////////////////////////////////////

void Compass::setScale(float s)
{
  _scale = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if the compass is in the act of showing or hiding
//
///////////////////////////////////////////////////////////////////////////////

bool Compass::isAnimating()
{
  return _animating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animation state to trigger a show or hide.
//  Use showCompass() or hideCompass() methods.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::setAnimation(bool state)
{
  _animating = state;
}


///////////////////////////////////////////////////////////////////////////////
//  true to use the view matrix
//  false to use "by angle degree"
//		- set using "setRotation" method.
//		- call "updateCompass" after changing angle values to rebuild 
//		  the compass
///////////////////////////////////////////////////////////////////////////////

void Compass::setRotationMode(bool m)
{
  _rotateByView = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuilds compass based upon animation show/hide parameters.
//	Call this method to rebuild after changing parameters in a set method
//
///////////////////////////////////////////////////////////////////////////////

void Compass::updateCompass()
{
	if(_showOrHideCompass)
		buildCompass();
	if(!_showOrHideCompass && _animating)
		buildCompass();
	if(!_showOrHideCompass && !_animating)
		_emptyCompass();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a triangle fan object to use as a compass face
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode* Compass::_buildTriangleFan(std::string tex, float zoff, unsigned int render_level, const osg::Vec3& n)
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode() );
	osg::ref_ptr< osg::StateSet > stateset ( new osg::StateSet() );
	osg::ref_ptr< osg::Image > image = osgDB::readImageFile( tex );

	if ( image.get() )
  {
	  osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D() ); 
	  texture->setImage ( image.get() );
	  stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
	  stateset->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
	  stateset->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );    
    stateset->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
    stateset->setRenderBinDetails ( render_level, "RenderBin" );
    
  }
	
	osg::ref_ptr< osg::Vec3Array > fan ( new osg::Vec3Array() );
	osg::ref_ptr< osg::Vec2Array > fan_tex ( new osg::Vec2Array() );
	
	fan->reserve( _numslices +1 );
	fan_tex->reserve( _numslices +1 );
	
	fan->push_back( osg::Vec3 ( 0.0f, 0.0f, zoff ) );
	fan_tex->push_back( osg::Vec2 ( 0.5f, 0.5f ) );
	
	for ( unsigned int x = 0; x < _numslices + 1; ++x )
	{	
		float angle =  float ( x ) * ( ( 2 * osg::PI ) / _numslices );
		fan->push_back ( osg::Vec3 ( _radius * ( cos( angle ) ), _radius * ( sin ( angle ) ), zoff ) );
		fan_tex->push_back(osg::Vec2(0.5f + (0.5f * ( cos( angle ) ) ),0.5f +( 0.5f * ( sin ( angle ) ) ) ));
	}
	osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry() ); 
	osg::ref_ptr< osg::Vec3Array > normal = new osg::Vec3Array;
  osg::ref_ptr< osg::Vec4Array > color ( new osg::Vec4Array() );
  color->push_back ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );
	geometry->setColorArray ( color.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
	normal->push_back ( n );

	geometry->setVertexArray ( fan.get() );

	geometry->setTexCoordArray ( 0, fan_tex.get() );

	geometry->setNormalArray ( normal.get() );

	geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

	geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_FAN, 0, fan->size() ) );

	geometry->setStateSet ( stateset.get() );

	geode->setStateSet ( stateset.get() );
    
	geode->addDrawable ( geometry.get() );

	return geode.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the acutal compass object, consisting of 2 textured triangle fans
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Compass::_buildCompassObject()
{
  osg::ref_ptr < osg::Group > group ( new osg::Group() );
  group->addChild ( this->_buildTriangleFan ( _topTexfn, 0.0f, 1001, osg::Vec3 ( 0.0f, 1.0f, 0.0f ) ) );
  group->addChild ( this->_buildTriangleFan ( _botTexfn, -0.05f, 1000, osg::Vec3 ( 0.0f, -1.0f, 0.0f ) ) );
	return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set matrix transforms for the compass.  Add the Cull and update callbacks
//	for rotation and animation.
//
///////////////////////////////////////////////////////////////////////////////

osg::MatrixTransform* Compass::_initCompass(osg::Node* geode)
{
	
	osg::ref_ptr < osg::MatrixTransform > rot ( new osg::MatrixTransform() );
	osg::ref_ptr < osg::MatrixTransform > anim ( new osg::MatrixTransform() );
	osg::ref_ptr < CompassOrientationCallback > rotation_callback ( new CompassOrientationCallback ( this ) );
	
	anim->setUpdateCallback ( new CompassAnimationCallback ( this, _animationStart, _animationEnd, _animationStep ) );
	if( _rotateByView )
	{
		rot->setCullCallback ( rotation_callback.get() );
	}
	else
	{
		rot->setMatrix ( osg::Matrix::scale ( _scale, _scale, _scale ) * 
					           osg::Matrix::rotate ( osg::inDegrees ( _rotdeg ), 0.0f, 0.0f, 1.0f ) * 
					           osg::Matrix::rotate ( osg::inDegrees ( -70.0f ), 1.0f, 0.0f, 0.0f ) *
				             osg::Matrix::translate ( _pos ) );
	}				
								  
	rot->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

	anim->addChild ( _compassObject.get() );
	rot->addChild ( anim.get() );
	
	return rot.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all children from the compass group.  Called when compass is
//  "hidden"
//
///////////////////////////////////////////////////////////////////////////////

void Compass::_emptyCompass()
{
	_compassGroup->removeChildren ( 0, _compassGroup->getNumChildren() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the compass.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::buildCompass()
{
	osg::ref_ptr < osg::Group > group ( new osg::Group() );
	osg::ref_ptr < osg::Node > geode = _buildCompassObject();

	osg::ref_ptr < osg::StateSet > ss ( _compassGroup->getOrCreateStateSet() );
	ss->setRenderBinDetails ( 1000, "RenderBin" );
	ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
	ss->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::INHERIT );

	_compassGroup->removeChildren ( 0, _compassGroup->getNumChildren() ); 
	_compassGroup->addChild ( _initCompass ( geode.get() ) );	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set parameters to display the "show" animation.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::_showCompass()
{
	_animationStart = 0.0f;
	_animationEnd = 1.0f; 
	_animationStep = 1.0f / 20.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set parameters to display the "hide" animation.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::_hideCompass()
{
	_animationStart = 1.0f;
	_animationEnd = 0.0f; 
	_animationStep = -1 * (1.0f / 20.0f);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle key pressed events.  Called from CompassEventHandler.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::keyChange(int key,int value)
{
#if _MSC_VER
    if (value)
    {
        
		if (key==osgGA::GUIEventAdapter::KEY_Left)
		{	
			if(_showOrHideCompass)
			_pos = osg::Vec3(_pos.x() - _scale / 10, _pos.y(), _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Right )
		{	
			if(_showOrHideCompass)
			_pos = osg::Vec3(_pos.x() + _scale / 10, _pos.y(), _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Down )
		{	
			if(_showOrHideCompass)
			_pos = osg::Vec3(_pos.x(), _pos.y() - _scale / 10, _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Up  )
		{	
			if(_showOrHideCompass)
			_pos = osg::Vec3(_pos.x(), _pos.y() + _scale / 10, _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Page_Down  )
		{	
			setRotation(getRotation() - 1.0f);
			//_rotdeg += 1.0f;
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Delete )
		{	
			setRotation(getRotation() + 1.0f);
			//_rotdeg -= 1.0f;
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Space )
		{	
			if(_rotateByView)
				_rotateByView = false;
			else
				_rotateByView = true;
		}
		else if (key==osgGA::GUIEventAdapter::KEY_End)
		{
			if(_showOrHideCompass)
			{
				_hideCompass();
				_animating = true;
				_showOrHideCompass = false;
			}
			else
			{
				_showCompass();
				_animating = true;
				_showOrHideCompass = true;
			}

		}
       
		updateCompass();

    }
#endif
    
}


