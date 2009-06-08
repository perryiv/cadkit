
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  DbXmlLeaf.h: Abstract light class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_XML_LIBRARY_LIGHT_H_
#define _CADKIT_DATABASE_XML_LIBRARY_LIGHT_H_

#include "DbXmlNode.h"


namespace CadKit
{
class DB_XML_API DbXmlLeaf : public DbXmlNode
{
public:

  DbXmlLeaf();
  DbXmlLeaf ( const char *name, const char *value );

  const std::string &         getValue() const { return _value; }

  void                        setValue ( const char *value ) { _value = ( value ) ? value : ""; }

protected:

  std::string _value;

  virtual ~DbXmlLeaf();

  DB_XML_DECLARE_DYNAMIC_NODE ( DbXmlLeaf, 1032912339 );
};
};

#endif
