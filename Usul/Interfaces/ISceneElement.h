
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to operate on a group of primitives.
//  This should probably be split up into multiple interfaces.
//  I also would like to add a method for taking a snap shot of primitives.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISceneElement_H__
#define __USUL_INTERFACES_ISceneElement_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Quat; class Vec3f; class Vec4f; }

namespace Usul {
namespace Interfaces {


struct ISceneElement : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISceneElement );

  /// Id for this interface.
  enum { IID = 1142124871u };

  /// Set the visiblity flag.
  virtual void                 setVisibility ( bool b ) = 0;

  /// Get the visiblity flag.
  virtual bool                 getVisibility ( ) const = 0;

  /// Set the translation.
  virtual void                 setTranslation ( const osg::Vec3f& translate ) = 0;

  /// Add the translation.
  //virtual void                 addTranslation ( const osg::Vec3f& delta ) = 0;

  /// Get the translation.
  virtual osg::Vec3f           getTranslation ( ) const = 0;

  /// Set the rotation.
  virtual void                 setRotation ( const osg::Quat& rotation ) = 0;

  /// Get the rotation.
  virtual osg::Quat            getRotation ( ) const = 0;

  /// Set the diffuse color
  virtual void                 setDiffuseColor ( const osg::Vec4f& ) = 0;

  /// Get the diffuse color
  virtual const osg::Vec4f&    getDiffuseColor ( ) const = 0;

  /// Set the specular color
  virtual void                 setSpecularColor ( const osg::Vec4f& ) = 0;

  /// Get the specular color
  virtual const osg::Vec4f&    getSpecularColor () const = 0;

  /// Set the shininess value
  virtual void                 setShininess( float s ) = 0;

  /// Get the shininess value
  virtual float                getShininess() const = 0;

  /// Set the transparency value
  virtual void                 setTransparency ( float t ) = 0;

  /// Get the transparency value
  virtual float                getTransparency () const = 0;

  /// Export a snap shot
  //virtual void                 beginExportSnapShot ( osg::Image* image, unsigned int height, unsigned int width ) = 0;
  //virtual void                 endExportSnapShot   ( ) = 0;
}; // struct ISceneElement


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISceneElement_H__ */
