
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
//  DbXmlWrite: The xml writing class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbXmlPrecompiled.h"
#include "DbXmlWrite.h"
#include "DbXmlGroup.h"
#include "DbXmlLeaf.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

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
