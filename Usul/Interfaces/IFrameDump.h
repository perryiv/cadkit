
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
#include <vector>

namespace Usul {
namespace Interfaces {


struct IFrameDump : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef std::string Filename;
  typedef std::vector< Filename > Filenames;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFrameDump );

  /// Id for this interface.
  enum { IID = 3168586510u };

  // Possible states for when to dump a frame.
  enum DumpState
  {
    NEVER_DUMP, 
    IF_DOCUMENT_NOT_BUSY, 
    ALWAYS_DUMP 
  };

  /// Set properties for dumping frames.
  virtual void frameDumpProperties ( const std::string &dir, 
                                     const std::string &base, 
                                     const std::string &ext, 
                                     unsigned int start = 0, 
                                     unsigned int digits = 10 ) = 0;

  /// Turn on/off frame-dumping.
  virtual void setFrameDumpState ( DumpState ) = 0;

  /// Are we dumping frames?
  virtual DumpState getFrameDumpState() const = 0;

  /// Reset the file name counter.
  virtual void resetFrameDumpCounter() = 0;

  /// Get the current file number
  virtual unsigned int currentFile() const = 0;

  /// Should the filenames that have been written be saved?
  virtual void saveNames ( bool ) = 0;
  virtual bool saveNames() const = 0;

  /// Get the filenames that were written out.
  virtual Filenames filenames() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_FRAME_DUMP_H_
