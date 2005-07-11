
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

#ifndef _OSG_SAL_FILE_IO_CLASS_H_
#define _OSG_SAL_FILE_IO_CLASS_H_

#include "SAL/OSG/CompileGuard.h"
#include "SAL/Interfaces/IRead.h"
#include "SAL/Interfaces/IWrite.h"
#include "SAL/Interfaces/INode.h"

#include "Usul/Base/Referenced.h"

namespace osg { class Referenced; class Node; };


namespace SAL {
namespace OSG {


class FileIO : public Usul::Base::Referenced,
               public SAL::Interfaces::IRead,
               public SAL::Interfaces::IWrite
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

  typedef USUL_VALID_REF_POINTER ( osg::Referenced ) ValidOsgRefPtr;
  typedef USUL_VALID_REF_POINTER ( osg::Node ) ValidOsgNodePtr;
  typedef USUL_REF_POINTER ( osg::Node ) OsgNodePtr;
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
  virtual Interfaces::INode * readNodeFile ( const std::string &filename ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IWrite
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Write the scene to file.
  virtual void    writeNodeFile ( const std::string &filename, Interfaces::INode *node ) const;
};


}; // namespace OSG
}; // namespace SAL


#endif // _OSG_SAL_NODE_CLASS_H_
