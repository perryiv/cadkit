
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAYER_H__
#define __STAR_SYSTEM_RASTER_LAYER_H__

#include "StarSystem/Export.h"
#include "StarSystem/Extents.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Predicates/LessVector.h"

#include "osg/Vec2d"
#include "osg/Image"
#include "osg/ref_ptr"

#include <functional>
#include <map>

namespace osg { class Vec3d; }
namespace Usul { namespace Jobs { class Job; } }


namespace StarSystem {


class STAR_SYSTEM_EXPORT RasterLayer : public Usul::Base::Object,
  public Usul::Interfaces::ILayer,
  public Usul::Interfaces::IRasterLayer
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef StarSystem::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec3uc Color;
  typedef std::equal_to < unsigned char > EqualPredicate;
  typedef Usul::Predicates::LessVector < EqualPredicate, 3 > LessColor;
  typedef std::map < Usul::Math::Vec3uc, unsigned char, LessColor > Alphas;
  typedef osg::ref_ptr < osg::Image > ImagePtr;

  USUL_DECLARE_REF_POINTERS ( RasterLayer );
  
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  RasterLayer();

  // Add an alpha value.
  void                  alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );

  // Get the alpha values.
  Alphas                alphas() const;

  /// Get/Set the extents.
  void                  extents ( const Extents& extents );
  Extents               extents () const;

  /// Get the texture.
  virtual osg::Image*   texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * ) = 0;
  
  /// Get the guid for the layer.
  virtual std::string   guid() const;
  
  /// Get/Set the name.
  virtual std::string   name() const;
  virtual void          name( const std::string& );
  
  /// Get/Set show layer
  virtual void          showLayer ( bool b );
  virtual bool          showLayer() const;

protected:

  virtual ~RasterLayer();

  virtual osg::Image *  _createBlankImage ( unsigned int width, unsigned int height ) const;

private:

  Extents _extents;
  Alphas _alphas;
  std::string _guid;
  std::string _name;
  bool _shown;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( RasterLayer );
};


}

#endif // __STAR_SYSTEM_RASTER_LAYER_H__
