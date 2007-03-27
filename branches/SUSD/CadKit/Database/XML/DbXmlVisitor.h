
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  DbXmlVisitor: The base visitor class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_XML_LIBRARY_BASE_VISITOR_H_
#define _CADKIT_DATABASE_XML_LIBRARY_BASE_VISITOR_H_

#include "DbXmlApi.h"
#include "DbXmlVisitorMacros.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include <list>
#endif


namespace CadKit
{
class DbXmlNode;
class DbXmlGroup;
class DbXmlLeaf;

class DB_XML_API DbXmlVisitor : public SlRefBase
{
public:

  // Push/pop the node. These are called from each node's accept() function.
  // Normal usage does not require calling these, but they have to be public.
  void                  pushNode ( DbXmlNode * );
  void                  popNode();

  // Reset the visitor. Call this in between scene visits to ensure that
  // any accumulated states are reset.
  virtual bool          reset();

  // The visit functions.
  virtual bool          visit ( DbXmlNode & ) = 0;
  virtual bool          visit ( DbXmlGroup & ) = 0;
  virtual bool          visit ( DbXmlLeaf & ) = 0;

protected:

  typedef std::list<DbXmlNode *> Stack; // Compiler doesn't like SlRefPtr here.
  Stack _stack;

  DbXmlVisitor();
  virtual ~DbXmlVisitor();

  DB_XML_DECLARE_VISITOR ( DbXmlVisitor, 0x00001039 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_XML_LIBRARY_BASE_VISITOR_H_
