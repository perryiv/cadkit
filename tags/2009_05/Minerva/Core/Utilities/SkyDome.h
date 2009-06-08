
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Simple sky dome.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_UTILITIES_SKY_DOME_H__
#define __MINERVA_CORE_UTILITIES_SKY_DOME_H__

#include "Minerva/Core/Export.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Array"
#include "osg/Geode"


namespace Minerva {
namespace Core {
namespace Utilities {

class MINERVA_EXPORT SkyDome : public osg::Geode
{
public:
  typedef osg::Geode BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef osg::ref_ptr<SkyDome> RefPtr;

  /// Constructor.
  SkyDome();
  
  /// Get the colors.
  osg::Vec4Array* colors() const;
  
  /// Set/get the dirty flag.
  void            dirty ( bool b );
  bool            dirty() const;
  
  /// Set/get the inner radius.
  void            innerRadius ( double value );
  double          innerRadius() const;
  
  // Return the mutex. Use with caution.
  Mutex &         mutex() const;
  
  /// Set/get the outer radius.
  void            outerRadius ( double value );
  double          outerRadius() const;
  
  /// Traverse the node.
  virtual void    traverse ( osg::NodeVisitor &nv );
  
  /// Get the vertices.
  osg::Vec3Array* vertices() const;
  
protected:
  
  /// Use reference counting.
  virtual ~SkyDome();
  
  /// Update geometry.
  void            _updateGeometry();
  
private:
  
  void            _destroy();
  
  mutable Mutex *_mutex;
  double _innerRadius;
  double _outerRadius;
  bool _dirty;
  osg::ref_ptr<osg::Vec3Array> _vertices;
  osg::ref_ptr<osg::Vec4Array> _colors;
};


}
}
}

#endif // __MINERVA_CORE_UTILITIES_SKY_DOME_H__
