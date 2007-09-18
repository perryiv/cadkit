
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

#include "VRV/Export.h"
#include "VRV/Animate/Frame.h"

#include "Usul/Base/Object.h"

#include "Serialize/XML/Macros.h"

#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace VRV {
namespace Animate {

class VRV_EXPORT Path : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object                                      BaseClass;

  USUL_DECLARE_REF_POINTERS ( Path );

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

  /// Animate one step.
  virtual void      animate ( Usul::Interfaces::IUnknown * caller ) = 0;

  /// Are we animating?
  bool              animating () const;

  /// Get/Set the number of animation steps.
  void              steps ( unsigned int value );
  unsigned int      steps ( ) const;

  /// Get/Set flag to accept new frames.
  void              acceptNewFrames ( bool b );
  bool              acceptNewFrames () const;
protected:
  virtual ~Path ();

  /// Append a frame.
  virtual void      _append ( Frame *frame ) = 0;

  bool                _dirty;
  bool                _acceptNewFrames;
  unsigned int        _numberSteps;
  unsigned int        _currentStep;
  double              _startTime;
  bool                _animating;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Path );
};

}
}


#endif // __VRV_ANIMATE_PATH_H__
