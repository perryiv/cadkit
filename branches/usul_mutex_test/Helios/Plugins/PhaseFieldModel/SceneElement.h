
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE_ELEMENT_H__
#define __SCENE_ELEMENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/ISceneElement.h"

#include "osg/ref_ptr"
#include "osg/MatrixTransform"
#include "osg/Material"

class SceneElement : public Usul::Base::Referenced,
                     public Usul::Interfaces::ISceneElement
{
  public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneElement );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  SceneElement();

  // Set the transform.
  void         transform ( osg::MatrixTransform *mt );

protected:
  virtual ~SceneElement();

  /// Usul::Interfaces::ISceneElement
  virtual void                 setVisibility ( bool b );
  virtual bool                 getVisibility ( ) const;
  virtual void                 setTranslation ( const osg::Vec3f& translate );
  virtual osg::Vec3f           getTranslation ( ) const;
  virtual void                 setRotation ( const osg::Quat& rotation );
  virtual osg::Quat            getRotation ( ) const;
  virtual void                 setDiffuseColor ( const osg::Vec4f& );
  virtual const osg::Vec4f&    getDiffuseColor ( ) const;
  virtual void                 setSpecularColor ( const osg::Vec4f& );
  virtual const osg::Vec4f&    getSpecularColor () const;
  virtual void                 setShininess( float s );
  virtual float                getShininess() const;
  virtual void                 setTransparency ( float t );
  virtual float                getTransparency () const;

private:
  osg::ref_ptr < osg::MatrixTransform > _mt;
  osg::ref_ptr < osg::Material > _material;
};


#endif // __SCENE_ELEMENT_H__
