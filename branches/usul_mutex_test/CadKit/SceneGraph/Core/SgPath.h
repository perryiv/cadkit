
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
