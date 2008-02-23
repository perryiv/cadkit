
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

#include "StarSystem/Tile.h"

#include "Usul/Jobs/Job.h"

namespace StarSystem {


class CutImageJob : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( CutImageJob );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CutImageJob );

  CutImageJob ( Tile::RefPtr tile );

protected:

  virtual ~CutImageJob();

  virtual void      _started();

private:

  Tile::RefPtr _tile;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_CUT_IMAGE_JOB_CLASS_H_
