
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_IMAGES_DOWN_SAMPLE_H__
#define __OSGTOOLS_IMAGES_DOWN_SAMPLE_H__

#include "OsgTools/Export.h"

#include "Usul/Math/Vector2.h"

#include "osg/ref_ptr"
#include "osg/Program"

#include <string>

namespace osg     { class Image; }

namespace OsgTools {
namespace Images {


class OSG_TOOLS_EXPORT DownSample
{
public:
  DownSample();

  /// Get/Set the size.
  void                         size ( unsigned int width, unsigned int height );
  void                         size ( const Usul::Math::Vec2ui& size );
  const Usul::Math::Vec2ui&    size () const;

  /// Get/Set the number of samples.
  void                         numSamples ( unsigned int samples );
  unsigned int                 numSamples () const;

  /// Set the filter size.
  void                         filterSize ( float x, float y );

  /// Down sample the image.
  osg::Image*                  operator () ( osg::Image* input );

protected:
  void                         _initialize ();
  std::string                  _buildFragmentProgram ( bool isXPass );
  float                        _sumFilter ( int r, float evenOffset, bool isXPass );
  float                        _sampleFilter ( float u, bool isXPass );
  float                        _filterValue ( float x, float y );

private:
  Usul::Math::Vec2ui _size;
  unsigned int       _numSamples;
  Usul::Math::Vec2f  _filterSize;
  bool               _dirty;
  
  osg::ref_ptr < osg::Program > _xProgram;
  osg::ref_ptr < osg::Program > _yProgram;
};


}
}


#endif // __OSGTOOLS_IMAGES_DOWN_SAMPLE_H__
