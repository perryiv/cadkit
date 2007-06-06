///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Compass.h"
using OsgTools::Builders;
///////////////////////////////////////////////////////////////////////////////
//
//  Update Callback for hiding and showing the compass
//
///////////////////////////////////////////////////////////////////////////////
class MyTransformCallback : public osg::NodeCallback
{
    public:

        MyTransformCallback(Compass* compass, float start, float end, float step)
        {
            _start = start;
			_end = end;
			_step = step;
			_compass = compass;
        }

        virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
        {
            osg::MatrixTransform* transform = dynamic_cast<osg::MatrixTransform*>(node);    
			if(_compass->getAnimationState())
			{
				if(_compass->getCompassState())
				{
					if (_start < _end)
					{
						_start += _step;
						transform->setMatrix(osg::Matrix::scale(_start,_start,_start));
						
					}
					else
						_compass->setAnimationState(false);
				}
				else
				{
					if (_start > _end)
					{
						_start += _step;
						transform->setMatrix(osg::Matrix::scale(_start,_start,_start));
						
					}
					else
						_compass->setAnimationState(false);
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
            if( cv != 0L)
            {
                osg::MatrixTransform *tx = dynamic_cast<osg::MatrixTransform *>(node);
                if( tx != 0L )
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
_texfn("./icons/big_compass_1.tga"),
_rotdeg(0.0f),
_scale(.07f),
_compassgroup(new osg::Group()),
_showcompass(true),
_numslices(32),
_radius(.8),
_animating(false),
_rotate_by_view(true)
{
	_pos = (osg::Vec3(_scale * 4.0f, _scale * -3.0f, -1.0f));
	_compassobject = buildCompassObject();
	buildCompass();
}
Compass::~Compass(){}
///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set Methods
//
///////////////////////////////////////////////////////////////////////////////
bool Compass::getCompassState(){return _showcompass;}

void Compass::setTextureFilename(const std::string& fn){_texfn = fn;buildCompass();}

osg::Node* Compass::getCompass(){return _compassgroup.get();}

float Compass::getRotation(){return _rotdeg;}
void Compass::setRotation(float r){_rotdeg = r;}

const osg::Vec3& Compass::getPosition(){return _pos;}
void Compass::setPotion(const osg::Vec3& p){_pos = p;}

float Compass::getScale(){return _scale;}
void Compass::setScale(float s){_scale = s;}

bool Compass::getAnimationState(){return _animating;}
void Compass::setAnimationState(bool state){_animating = state;}

///////////////////////////////////////////////////////////////////////////////
//  true to use the view matrix
//  false to use "by angle degree"
//		- set using "setRotation" method.
//		- call "updateCompass" after changing angle values to rebuild 
//		  the compass
///////////////////////////////////////////////////////////////////////////////
void Compass::setRotationMode(bool m){_rotate_by_view = m;}

///////////////////////////////////////////////////////////////////////////////
//
//  Rebuilds compass based upon animation show/hide parameters.
//	Call this method to rebuild after changing parameters in a set method
//
///////////////////////////////////////////////////////////////////////////////
void Compass::updateCompass()
{
	if(_showcompass)
		buildCompass();
	if(!_showcompass && _animating)
		buildCompass();
	if(!_showcompass && !_animating)
		emptyCompass();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the acutal compass object, consisting of 2 textured triangle fans
//
///////////////////////////////////////////////////////////////////////////////
osg::Geode* Compass::buildCompassObject()
{
	int x;
	float angle;
	osg::ref_ptr<osg::Geode> geode (new osg::Geode());
	osg::ref_ptr<osg::StateSet> stateset (new osg::StateSet());
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(_texfn);

	if (image.get())
    {
		osg::ref_ptr<osg::Texture2D> texture (new osg::Texture2D()); 
		texture->setImage(image.get());
		stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
        
        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    }
	
	osg::ref_ptr<osg::Vec3Array> top_fan (new osg::Vec3Array());
	osg::ref_ptr<osg::Vec3Array> bottom_fan (new osg::Vec3Array());
	osg::ref_ptr<osg::Vec2Array> top_fan_tex (new osg::Vec2Array());
	osg::ref_ptr<osg::Vec2Array> btm_fan_tex (new osg::Vec2Array());
	
	top_fan->reserve(_numslices +1);
	bottom_fan->reserve(_numslices +1);

	top_fan_tex->reserve(_numslices +1);
	btm_fan_tex->reserve(_numslices +1);
	
	top_fan->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	bottom_fan->push_back(osg::Vec3(0.0f, 0.0f, -0.05f));
	top_fan_tex->push_back(osg::Vec2(0.5f, 0.5f));
	btm_fan_tex->push_back(osg::Vec2(0.5f, 0.5f));
	
	for(x = 0; x < _numslices + 1; ++x)
	{	
		angle =  float (x) * ( (2 * osg::PI) / _numslices);
		
		top_fan->push_back(osg::Vec3(_radius * ( cos(angle) ), _radius * (sin(angle)), 0.0f));
		bottom_fan->push_back(osg::Vec3(_radius * ( cos(angle) ), _radius * (sin(angle)), -0.05f));

		top_fan_tex->push_back(osg::Vec2(0.5f + (0.5f * ( cos(angle) ) ),0.5f +( 0.5f * (sin(angle)) ) ));
		btm_fan_tex->push_back(osg::Vec2(0.5f + (0.5f * ( cos(angle) ) ),0.5f +( 0.5f * (sin(angle)) ) ));

	}
	osg::ref_ptr<osg::Geometry> top_geometry ( new osg::Geometry() ); 
	osg::ref_ptr<osg::Geometry> bottom_geometry ( new osg::Geometry() ); 
	osg::ref_ptr<osg::Vec3Array> top_normal = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> bottom_normal = new osg::Vec3Array;

	top_normal->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	bottom_normal->push_back(osg::Vec3(0.0f,0.0f,-1.0f));

	top_geometry->setVertexArray(top_fan.get());
	bottom_geometry->setVertexArray(bottom_fan.get());

	top_geometry->setTexCoordArray(0,top_fan_tex.get());
	bottom_geometry->setTexCoordArray(0,btm_fan_tex.get());

	top_geometry->setNormalArray(top_normal.get());
	bottom_geometry->setNormalArray(bottom_normal.get());

	top_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	bottom_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	top_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,top_fan->size()));
	bottom_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,bottom_fan->size()));

	top_geometry->setStateSet( stateset.get() );
	bottom_geometry->setStateSet( stateset.get() );

	geode->setStateSet( stateset.get() );
    
	geode->addDrawable(top_geometry.get());
	geode->addDrawable(bottom_geometry.get());

	return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set matrix transforms for the compass.  Add the Cull and update callbacks
//	for rotation and animation.
//
///////////////////////////////////////////////////////////////////////////////
osg::MatrixTransform* Compass::initCompass(osg::Node* geode)
{
	
	osg::ref_ptr<osg::MatrixTransform> rot (new osg::MatrixTransform());
	osg::ref_ptr<osg::MatrixTransform> anim (new osg::MatrixTransform());
	osg::ref_ptr<CompassOrientationCallback> rotation_callback (new CompassOrientationCallback(this));
	
	anim->setUpdateCallback(new MyTransformCallback(this, _animation_start, _animation_end, _animation_step));
	if(_rotate_by_view)
	{
		rot->setCullCallback(rotation_callback.get());
	}
	else
	{
		rot->setMatrix( osg::Matrix::scale( _scale,_scale,_scale ) * 
					    osg::Matrix::rotate( osg::inDegrees(_rotdeg),0.0f,0.0f,1.0f ) * 
					    osg::Matrix::rotate( osg::inDegrees(-70.0f),1.0f,0.0f,0.0f ) *
				        osg::Matrix::translate( _pos ) );
	}				
								  
	rot->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	anim->addChild(_compassobject.get());
	rot->addChild(anim.get());
	
	return rot.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Remove all children from the compass group.  Called when compass is
//  "hidden"
//
///////////////////////////////////////////////////////////////////////////////
void Compass::emptyCompass()
{
	_compassgroup->removeChildren(0,_compassgroup->getNumChildren());
}
///////////////////////////////////////////////////////////////////////////////
//
//  Build the compass.
//
///////////////////////////////////////////////////////////////////////////////
void Compass::buildCompass()
{
	osg::ref_ptr<osg::Group> group (new osg::Group());
	osg::ref_ptr<osg::Geode> geode = buildCompassObject();

	_compassgroup->removeChildren(0,_compassgroup->getNumChildren()); 
	_compassgroup->addChild( initCompass(geode.get()) );	
}
///////////////////////////////////////////////////////////////////////////////
//
//  Set parameters to display the "show" animation.
//
///////////////////////////////////////////////////////////////////////////////
void Compass::showCompass()
{
	_animation_start = 0.0f;
	_animation_end = 1.0f; 
	_animation_step = 1.0f / 20.0f;
}
///////////////////////////////////////////////////////////////////////////////
//
//  Set parameters to display the "hide" animation.
//
///////////////////////////////////////////////////////////////////////////////
void Compass::hideCompass()
{
	_animation_start = 1.0f;
	_animation_end = 0.0f; 
	_animation_step = -1 * (1.0f / 20.0f);
}
///////////////////////////////////////////////////////////////////////////////
//
//  Handle key pressed events.  Called from CompassEventHandler.
//
///////////////////////////////////////////////////////////////////////////////
void Compass::keyChange(int key,int value)
{
    if (value)
    {
        
		if (key==osgGA::GUIEventAdapter::KEY_Left)
		{	
			if(_showcompass)
			_pos = osg::Vec3(_pos.x() - _scale / 10, _pos.y(), _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Right )
		{	
			if(_showcompass)
			_pos = osg::Vec3(_pos.x() + _scale / 10, _pos.y(), _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Down )
		{	
			if(_showcompass)
			_pos = osg::Vec3(_pos.x(), _pos.y() - _scale / 10, _pos.z());
		}
		else if (key==osgGA::GUIEventAdapter::KEY_Up  )
		{	
			if(_showcompass)
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
			if(_rotate_by_view)
				_rotate_by_view = false;
			else
				_rotate_by_view = true;
		}
		else if (key==osgGA::GUIEventAdapter::KEY_End)
		{
			if(_showcompass)
			{
				hideCompass();
				_animating = true;
				_showcompass = false;
			}
			else
			{
				showCompass();
				_animating = true;
				_showcompass = true;
			}

		}
       
		updateCompass();

    }
    
}


