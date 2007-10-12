
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading and writing a node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PERFORMER_SAL_FILE_IO_CLASS_H_
#define _PERFORMER_SAL_FILE_IO_CLASS_H_

#include "SAL/Performer/CompileGuard.h"

#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/IWrite.h"
#include "Usul/Base/Referenced.h"

class pfMemory;
class pfNode;


namespace SAL {
namespace Performer {


class FileIO : public Usul::Base::Referenced,
               public Usul::Interfaces::IRead,
               public Usul::Interfaces::IWrite
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FileIO );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  FileIO();

protected:

  typedef USUL_VALID_REF_POINTER ( ::pfMemory ) ValidPfMemoryPtr;
  typedef USUL_VALID_REF_POINTER ( ::pfNode ) ValidPfNodePtr;
  typedef USUL_REF_POINTER ( ::pfNode ) PfNodePtr;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Do not copy.
  FileIO ( const FileIO & );
  FileIO &operator = ( const FileIO & );

  /// Use reference counting.
  virtual ~FileIO();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IRead
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Read the scene from file.
  virtual Unknown *   read ( const std::string &filename, Unknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IWrite
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Write the scene to file.
  virtual void        write ( const std::string &filename, Unknown *data, Unknown *caller );
};


}; // namespace Performer
}; // namespace SAL


#endif // _PERFORMER_SAL_NODE_CLASS_H_
