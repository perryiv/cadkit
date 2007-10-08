
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_ANIMATE_RECORDED_PATH_H__
#define __VRV_ANIMATE_RECORDED_PATH_H__

#include "Helios/Plugins/Animate/Path.h"
#include "Helios/Plugins/Animate/Frame.h"

#include "Serialize/XML/Macros.h"

#include <list>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace Animate {

class RecordedPath : public Animate::Path
{
public:
  typedef Animate::Path                                      BaseClass;
  typedef std::list < Frame::RefPtr >                        Frames;

  USUL_DECLARE_REF_POINTERS ( RecordedPath );

  RecordedPath ();

  /// Clear.
  virtual void      clear ();

  /// Start the animation from beginning.
  virtual void      start ( Usul::Interfaces::IUnknown * caller );

  /// Animate one step.
  virtual void      updateNotify ( Usul::Interfaces::IUnknown * caller );

protected:
  virtual ~RecordedPath ();

  /// Append a frame.
  virtual void      _append ( Frame *frame );

  void              _animate ( Usul::Interfaces::IUnknown * caller );
  void              _record ( Usul::Interfaces::IUnknown * caller );
private:
  Frames              _frames;
  Frames::iterator    _current;

  SERIALIZE_XML_DEFINE_MEMBERS ( RecordedPath );
};

}


#endif // __VRV_ANIMATE_PATH_H__
