
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgNode.h: Base class for all scene graph nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_NODE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_NODE_H_

#include "SgVisitor.h"
#include "SgNodeMacros.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefPtr.h"
# include "Standard/SlBitmask.h"
#endif


namespace CadKit
{
class SG_API SgNode : public SlRefBase
{
public:

  typedef FlagsNode Flags;

  // Possible flags.
  enum
  {
    ACCEPT_VISITORS = (1L<<0), // Default is true.
    UNICODE_NAME    = (1L<<1), // Default is false.
  };

  // Return the total number of nodes in existance (not necessarily 
  // in a scene graph). When your program exits this should be zero or else
  // you have a memory leak.
  #ifdef _DEBUG
  static const long &       getNumNodes() { return _numNodes; }
  #endif

  // The client can set their own data associated with the node.
  const void *              getClientData() const { return _clientData; }
  void                      setClientData ( const void *clientData ) { _clientData = clientData; }

  // The client can set a name for the node.
  const char *              getNameA() const { return static_cast<char *>(_name);; }
  const wchar_t *           getNameW() const { return static_cast<wchar_t *>(_name); }
  void                      setNameA ( const char *name );
  void                      setNameW ( const wchar_t *name );

  // Set the data members from the given node.
  void                      setValue ( const SgNode &node );

  // Reference/unreference the node.
  void                      ref()   { this->_incrementReferenceCount(); }
  void                      unref() { this->_decrementReferenceCount(); }

protected:

  #ifdef _DEBUG
  static long _numNodes;
  #endif

  Flags _flags;
  mutable const void *_reservedData;
  const void *_clientData;
  void *_name; // Instance of std::string is 16 bytes, this is 4.

  SgNode();
  SgNode ( const SgNode &node );
  virtual ~SgNode();

  void                      _setName ( const void *name );

  SG_DECLARE_NODE ( SgNode, 0x0000101C );
  SL_DECLARE_BITMASK_FUNCTIONS(NodeFlags,Flags,_flags);
};

}; // namespace CadKit


#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_NODE_H_
