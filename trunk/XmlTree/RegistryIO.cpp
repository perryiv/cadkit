
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that reads and writes Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/RegistryIO.h"
#include "XmlTree/Document.h"
#include "XmlTree/Loader.h"
#include "XmlTree/RegistryBuilder.h"
#include "XmlTree/RegistryVisitor.h"
#include "XmlTree/Writer.h"

#include "Usul/Registry/Database.h"
#include "Usul/Trace/Trace.h"

using namespace XmlTree;


///////////////////////////////////////////////////////////////////////////////
//
//  Read the registry.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryIO::read ( const std::string &file )
{
  USUL_TRACE_SCOPE_STATIC;

  // Call the other one.
  RegistryIO::read ( file, Usul::Registry::Database::instance() );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Read the registry.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryIO::read ( const std::string &file, Usul::Registry::Database& db )
{
  USUL_TRACE_SCOPE_STATIC;

  // Read xml tree.
  XmlTree::Loader loader;
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  loader.load ( file, document.get() );

  // Build registry.
  XmlTree::RegistryBuilder::RefPtr builder ( new XmlTree::RegistryBuilder );
  builder->build ( document.get(), db );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the registry.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryIO::write ( const std::string &file )
{
  USUL_TRACE_SCOPE_STATIC;

  // Call the other one.
  RegistryIO::write ( file, Usul::Registry::Database::instance() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the registry.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryIO::write ( const std::string &file, Usul::Registry::Database& db )
{
  USUL_TRACE_SCOPE_STATIC;

  // Build xml tree.
  XmlTree::RegistryVisitor::RefPtr visitor ( new XmlTree::RegistryVisitor );
  db.accept ( visitor.get() );

  // Write it to disk.
  XmlTree::Writer writer;
  writer.write ( visitor->document(), file );
}
