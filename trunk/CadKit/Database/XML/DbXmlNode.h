
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
//  DbXmlNode.h: Base class for all nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_XML_LIBRARY_NODE_H_
#define _CADKIT_DATABASE_XML_LIBRARY_NODE_H_

#include "DbXmlVisitor.h"
#include "DbXmlNodeMacros.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefPtr.h"
# include "Standard/SlRefBase.h"
#endif


namespace CadKit
{
class DbXmlGroup;

class DB_XML_API DbXmlNode : public SlRefBase
{
public:

  // Return the total number of nodes in existance.
  static const long &       getNumNodes() { return _numNodes; }

  // Get/set the name.
  const std::string &       getName() const { return _name; }
  void                      setName  ( const char *name ) { _name = name; }

  // Reference/unreference the node.
  void                      ref()   { this->_incrementReferenceCount(); }
  void                      unref() { this->_decrementReferenceCount(); }

protected:

  static long _numNodes;
  std::string _name;

  DbXmlNode();
  DbXmlNode ( const char *name );
  DbXmlNode ( const DbXmlNode &node );
  virtual ~DbXmlNode();

  DB_XML_DECLARE_NODE ( DbXmlNode, 0x0000101C );
};

}; // namespace CadKit


#endif // _CADKIT_DATABASE_XML_LIBRARY_NODE_H_
