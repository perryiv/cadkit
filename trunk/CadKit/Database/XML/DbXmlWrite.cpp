
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbXmlWrite: The xml writing class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbXmlPrecompiled.h"
#include "DbXmlWrite.h"
#include "DbXmlGroup.h"
#include "DbXmlLeaf.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlAssert.h"
#endif

using namespace CadKit;

DB_XML_IMPLEMENT_DYNAMIC_VISITOR(DbXmlWrite,DbXmlVisitor);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbXmlWrite::DbXmlWrite() : DbXmlVisitor(),
  _indention ( "" ),
  _out ( NULL ),
  _mode ( WRITE_NAME_AND_VALUE )
{
  SL_PRINT2 ( "In DbXmlWrite::DbXmlWrite(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbXmlWrite::~DbXmlWrite()
{
  SL_PRINT2 ( "In DbXmlWrite::~DbXmlWrite(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
///////////////////////////////////////////////////////////////////////////////

bool DbXmlWrite::write ( const DbXmlNode &node, const char *filename )
{
  // Handle trivial case.
  if ( NULL == filename )
    return false;

  // Try to open the file for writing.
  std::ofstream out ( filename );

  // See if it opened.
  if ( false == out.is_open() )
    return false;

  // Call the other one.
  return this->write ( node, out );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
///////////////////////////////////////////////////////////////////////////////

bool DbXmlWrite::write ( const DbXmlNode &node, std::ostream &out )
{
  // Assign the output stream.
  _out = &out;

  // Initialize the indention.
  _indention = "";

  // Visit this node. I break the "const" rule here because it doesn't change.
  return ((DbXmlNode &) node).accept ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool DbXmlWrite::visit ( DbXmlNode & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); // Heads up. Remve this assertion if you intend to be here.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool DbXmlWrite::visit ( DbXmlGroup &group )
{
  SL_ASSERT ( this );

  // If we don't have any kids...
  if ( 0 == group.getNumChildren() )
  {
    // Just write the name.
    (*_out) << _indention << "<" << group.getName() << " />"<< std::endl;
  }

  // Otherwise...
  else
  {
    // Write the start of the node.
    (*_out) << _indention << "<" << group.getName() << ">" << std::endl;

    // Increment the indention.
    _indention += "  ";

    // Traverse the children.
    if ( false == group.traverse ( *this ) )
      return false;

    // Decrement the indention.
    _indention.resize ( _indention.size() - 2 );

    // Write the end of the node.
    (*_out) << _indention << "</" << group.getName() << ">" << std::endl;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool DbXmlWrite::visit ( DbXmlLeaf &leaf )
{
  SL_ASSERT ( this );

  // If we don't have a value, or if we are in the "write only name" mode...
  if ( WRITE_NAME_ONLY == _mode || leaf.getValue().empty() )
  {
    // Just write the name.
    (*_out) << _indention << "<" << leaf.getName() << " />" << std::endl;
  }

  // Otherwise...
  else
  {
    // Write the name and value.
    (*_out) << _indention << "<" << leaf.getName() << ">" << leaf.getValue() << "</" << leaf.getName() << ">" << std::endl;
  }

  // It worked.
  return true;
}
