
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_ANIMATE_PATH_H__
#define __VRV_ANIMATE_PATH_H__

#include "Helios/Plugins/Animate/Frame.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "Serialize/XML/Macros.h"

#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }


namespace Animate {

class Path : public Usul::Base::Object,
             public Usul::Interfaces::IUpdateListener
{
public:
  typedef Usul::Base::Object                                      BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( Path );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Path ();

  /// Append a frame.
  virtual void      append ( Frame *frame );

  /// Clear.
  virtual void      clear () = 0;

  /// Get/Set the dirty flag.
  void              dirty ( bool );
  bool              dirty () const;

  /// Start the animation from beginning.
  virtual void      start ( Usul::Interfaces::IUnknown * caller );

  /// Are we animating?
  bool              animating () const;

  /// Get/Set flag to accept new frames.
  void              acceptNewFrames ( bool b );
  bool              acceptNewFrames () const;

protected:
  virtual ~Path ();

  /// Append a frame.
  virtual void      _append ( Frame *frame ) = 0;

  /// (Usul::Interfaces::IUpdateListener).
  virtual void      updateNotify    ( Usul::Interfaces::IUnknown *caller );

  bool                _dirty;
  bool                _acceptNewFrames;
  double              _startTime;
  bool                _animating;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Path );
};

}


#endif // __VRV_ANIMATE_PATH_H__
