
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to fetch an image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_CUT_IMAGE_JOB_CLASS_H_
#define _STAR_SYSTEM_CUT_IMAGE_JOB_CLASS_H_

#include "StarSystem/Extents.h"

#include "Usul/Jobs/Job.h"

#include "osg/Image"
#include "osg/ref_ptr"
#include "osg/Vec2d"

namespace StarSystem { class RasterLayer; }


namespace StarSystem {


class CutImageJob : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;
  typedef osg::ref_ptr < osg::Image > ImagePtr;
  typedef StarSystem::Extents < osg::Vec2d > Extents;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CutImageJob );

  CutImageJob ( const Extents &extents, unsigned int level, RasterLayer *layer );

  osg::Image *      image();

protected:

  virtual ~CutImageJob();

  virtual void      _started();

private:

  Extents _extents;
  unsigned int _level;
  RasterLayer *_raster;
  ImagePtr _image;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_CUT_IMAGE_JOB_CLASS_H_
