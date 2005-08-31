
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for dumping frames to disk.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FRAME_DUMP_H_
#define _USUL_INTERFACE_FRAME_DUMP_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IFrameDump : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFrameDump );

  /// Id for this interface.
  enum { IID = 3168586510u };

  /// Set properties for dumping frames.
  virtual void            frameDumpProperties ( const std::string &dir, 
                                                const std::string &base, 
                                                const std::string &ext, 
                                                unsigned int start = 0, 
                                                unsigned int digits = 10 ) = 0;

  /// Turn on/off frame-dumping.
  virtual void            frameDumpState ( bool ) = 0;

  /// Are we dumping frames?
  virtual bool            frameDumpState() const = 0;

  /// Reset the file name counter.
  virtual void            frameDumpResetCounter() = 0;

  /// Get the current file number
  virtual unsigned int    frameDumpCurrentFileNum() const = 0;

  /// Small struct to turn on/off frame-dumping.
  struct ScopedDump
  {
    ScopedDump ( IUnknown *u ) : _f ( u )
    {
      if ( _f.valid() )
      {
        _f->frameDumpState ( true );
        _f->frameDumpResetCounter();
      }
    }
    ~ScopedDump()
    {
      if ( _f.valid() )
        _f->frameDumpState ( false );
    }
  protected:
    IFrameDump::QueryPtr _f;
  };
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FRAME_DUMP_H_
