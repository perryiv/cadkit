
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
# include <string>
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

protected:

  static long _numNodes;
  std::string _name;

  DbXmlNode();
  DbXmlNode ( const char *name );
  DbXmlNode ( const DbXmlNode &node );
  virtual ~DbXmlNode();

  DB_XML_DECLARE_NODE ( DbXmlNode, 1032912296 );
};

}; // namespace CadKit


#endif // _CADKIT_DATABASE_XML_LIBRARY_NODE_H_
