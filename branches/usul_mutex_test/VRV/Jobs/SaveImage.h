
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Write a image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_JOBS_SAVE_IMAGE_H__
#define __VRV_JOBS_SAVE_IMAGE_H__

#include "Usul/Jobs/Job.h"

#include "osg/Image"

#include <string>

namespace VRV {
namespace Jobs {


class SaveImage : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;

  SaveImage ( const std::string& filename, osg::Image *image );

protected:
  virtual ~SaveImage ();

  virtual void _started();

private:
  std::string _filename;
  osg::ref_ptr < osg::Image > _image;
};

}
}

#endif // __VRV_JOBS_SAVE_IMAGE_H__
