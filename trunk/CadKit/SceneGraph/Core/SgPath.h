
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
//  SgPath.h: A path through the scene graph tree.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_PATH_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_PATH_H_

#include "SgNode.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include <vector> // gcc doesn't like <list>.
#endif

#ifdef CADKIT_DEFINE_SG_PATH_WRITE_TO_OSTREAM
# include <iostream.h>
#endif

#ifdef CADKIT_DEFINE_SG_PATH_WRITE_TO_STD_OSTREAM
# include <iostream>
#endif


namespace CadKit
{
class SgPath : public SlRefBase
{
public:

  typedef std::vector<SgNode::Ptr> Nodes;

  SgPath();
  SgPath ( const SgPath &path );
  ~SgPath();

  void              append ( SgNode *node );
  void              clear();

  bool              hasNode ( const SgNode *node ) const;
  bool              hasNodeOfType ( const SlType *type ) const;
  bool              hasNodeOfExactType ( const SlType *type ) const;

  SgNode *          getFirstOfType ( const SlType *type ) const;
  SgNode *          getLastOfType ( const SlType *type ) const;

  SgNode *          getNode ( const unsigned int &index ) const;
  const Nodes *     getNodes() const { return _nodes; }
  unsigned int      getNumNodes() const { return _nodes->size(); }

  SgNode *          getHead() const;
  SgNode *          getTail() const;

  void              setValue ( const SgPath &path );

  #ifdef CADKIT_DEFINE_SG_PATH_WRITE_TO_OSTREAM
  void              write ( ::ostream &out ) const;
  #endif

  #ifdef CADKIT_DEFINE_SG_PATH_WRITE_TO_STD_OSTREAM
  void              write ( std::ostream &out ) const;
  #endif

protected:

  Nodes *_nodes; // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.

  SL_DECLARE_REFERENCE_POINTER ( SgPath );
  SL_DECLARE_DYNAMIC_CLASS ( SgPath, 1032912036 );
};                       

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_PATH_H_
