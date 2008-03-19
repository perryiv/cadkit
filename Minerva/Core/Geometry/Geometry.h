
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_GEOMETRY_H__
#define __MINERVA_CORE_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Interfaces/IOffset.h"

#include "Usul/Base/Referenced.h"

#include "osg/Vec3f"
namespace osg { class Geometry; class Node; }

#include <vector>

namespace Minerva {
namespace Core {
namespace Geometry {

class MINERVA_EXPORT Geometry : public Usul::Base::Referenced,
                                public Minerva::Interfaces::IOffset
{
public:
  typedef Usul::Base::Referenced        BaseClass;

  USUL_DECLARE_QUERY_POINTERS( Geometry );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Geometry();

  // Is it valid?
  bool                        valid() const;

  /// Get/Set the dirty flag
  void                        dirty ( bool b );
  bool                        dirty () const;

  /// Get/Set the srid.
  unsigned int                srid () const { return _srid; }
  void                        srid ( unsigned int srid ) { _srid = srid; }

protected:
  virtual ~Geometry();

  /// Usul::Interfaces::IOffset
  virtual const osg::Vec3f&   spatialOffset () const;
  virtual void                spatialOffset ( const osg::Vec3f& );

private:
  int _srid;
  osg::Vec3f _offset;
  bool _dirty;

};

}
}
}

#endif // __MINERVA_CORE_GEOMETRY_H__
