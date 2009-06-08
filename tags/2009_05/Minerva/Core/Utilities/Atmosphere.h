
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
//  Atmospheric scattering.  See: 
//  Ray tracing of light through atmosphere: http://www.gamedev.net/reference/articles/article2093.asp
//  Explaination of formulas: http://nis-lab.is.s.u-tokyo.ac.jp/~nis/cdrom/skymul.pdf
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_UTILITIES_ATMOSPHERE_H__
#define __MINERVA_CORE_UTILITIES_ATMOSPHERE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Utilities/SkyDome.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Image"

namespace osgUtil { class CullVisitor; }

namespace Minerva {
namespace Core {
namespace Utilities {

class MINERVA_EXPORT Atmosphere : public SkyDome
{
public:
  typedef SkyDome BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  // Declare smart pointers.
  USUL_DECLARE_REF_POINTERS ( Atmosphere );
  
  /// Constructor.
  Atmosphere();
  
  /// Set/get the dirty flag.
  void            dirty ( bool b );
  bool            dirty() const;

  virtual void    traverse ( osg::NodeVisitor &nv );
  
protected:
  
  /// Use reference counting.
  virtual ~Atmosphere();

  /// Update colors.
  void            _updateColors( osgUtil::CullVisitor& cv );
  
  /// Update single color.
  void            _updateColor ( const osg::Vec3& eye, const osg::Vec3& lightDirection, const osg::Vec3& vertex, osg::Vec4& color );
  
  /// Build lookup table.
  void            _buildOpticalDepthBuffer();
  
  /// Get value from lookup table.
  void            _valueOpticalDepthBuffer( osg::Vec4& value, double x, double y );
  
private:
  
  void            _destroy();
  
  bool _dirty;
  
  unsigned int _samples;
  double _Kr;
  double _Kr4PI;
  double _Km;
  double _Km4PI;
  double _intensity;
  double _g;
  
  Usul::Math::Vec3d _wavelength;
  Usul::Math::Vec3d _wavelength4;
  
  double _rayleighScaleDepth;
  double _mieScaleDepth;
  
  osg::ref_ptr<osg::Image> _opticalDepthBuffer;
};


}
}
}

#endif // __MINERVA_CORE_UTILITIES_ATMOSPHERE_H__
