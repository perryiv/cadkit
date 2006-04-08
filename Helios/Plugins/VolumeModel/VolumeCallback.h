
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback for changing spacing of images in volume
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOLUME_DRAGGER_CALLBACK_CLASS_H_
#define _VOLUME_DRAGGER_CALLBACK_CLASS_H_

#include "OsgTools/Draggers/Callback.h"

#include "osg/MatrixTransform"
#include "osg/ref_ptr"

#include <vector>

class VolumeCallback : public OsgTools::Draggers::Callback
{
public:

  // Useful typedefs
  typedef OsgTools::Draggers::Callback BaseClass;
  typedef osg::ref_ptr<VolumeCallback> Ptr;

  // Constructors.
  VolumeCallback();
  VolumeCallback ( const VolumeCallback & );

  // Execute the callback.
  virtual void operator() ( OsgTools::Draggers::Dragger &d );

  void addMT ( osg::MatrixTransform* mt ) { _transforms.push_back ( mt ); }

protected:

  // Use reference counting.
  virtual ~VolumeCallback();

  typedef std::vector< osg::ref_ptr< osg::MatrixTransform > > Transforms;

  Transforms _transforms;
};




#endif // _VOLUME_DRAGGER_CALLBACK_CLASS_H_
