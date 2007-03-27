
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
# include <string>
# include <time.h>
#endif


namespace CadKit
{
class SG_API SgNode : public SlRefBase
{
public:

  // Possible flags.
  enum
  {
    ACCEPT_VISITORS = 0x00000001, // Default is true.
    UNICODE_NAME    = 0x00000002, // Default is false.
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

  // Get the time (in clock tics) to render this node.
  const clock_t &           getRenderTime() const { return _renderTime; }

  // The client can set a name for the node.
  const char *              getName() const { return _name; }
  void                      setName ( const char *name );

  // Does the given node have the same visible properties?
  virtual bool              isEqualVisibleProperties ( const SgNode &node ) const;

  // Set the data members from the given node.
  void                      setValue ( const SgNode &node );

protected:

  #ifdef _DEBUG
  static long _numNodes;
  #endif

  unsigned int _flags;
  mutable const void *_reservedData;
  const void *_clientData;
  char *_name;
  clock_t _renderTime;

  SgNode();
  SgNode ( const SgNode &node );
  virtual ~SgNode();

  void                      _setRenderTime ( const clock_t &clockTics ) { _renderTime = clockTics; }

  SL_DECLARE_BITMASK_FUNCTIONS ( NodeFlags, unsigned int, _flags );
  SG_DECLARE_NODE ( SgNode, 1033952892 );
};

}; // namespace CadKit


#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_NODE_H_
