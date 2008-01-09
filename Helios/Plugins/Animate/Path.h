
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
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "Serialize/XML/Macros.h"

#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }


namespace Animate {

class Path : public Usul::Documents::Document,
             public Usul::Interfaces::IUpdateListener
{
public:
  typedef Usul::Documents::Document        BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( Path );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Path ( const std::string& type );

  /// Append a frame.
  virtual void      append ( Frame *frame );

  /// Get/Set the dirty flag.
  void              dirty ( bool );
  bool              dirty () const;

  /// Start the animation from beginning.
  virtual void      start ( Usul::Interfaces::IUnknown * caller );

  /// Stop the animation.
  virtual void      stop ( Usul::Interfaces::IUnknown * caller );

  /// Get/Set animate flag?
  void              animating ( bool );
  bool              animating () const;
  

  /// Get/Set flag to accept new frames.
  void              acceptNewFrames ( bool b );
  bool              acceptNewFrames () const;

  /// Get the extension for the file.
  virtual std::string  extension () const = 0;

  /// Get the name of the path.
  virtual std::string  name () const = 0;

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Get the filters that correspond to what this document can do.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Read.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;

protected:
  virtual ~Path ();

  /// Append a frame.
  virtual void      _append ( Frame *frame ) = 0;

  /// (Usul::Interfaces::IUpdateListener).
  virtual void      updateNotify    ( Usul::Interfaces::IUnknown *caller );

private:
  bool                _dirty;
  bool                _acceptNewFrames;
  double              _startTime;
  bool                _animating;
  bool                _renderLoopState;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Path );
};

}


#endif // __VRV_ANIMATE_PATH_H__
