
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

#ifndef _CADKIT_DATABASE_XML_LIBRARY_WRITER_H_
#define _CADKIT_DATABASE_XML_LIBRARY_WRITER_H_

#include "DbXmlVisitor.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlBoundingBox.h"
# include "Standard/SlMatrix44.h"
# include <fstream>
# include <string>
#endif


namespace CadKit
{
class DB_XML_API DbXmlWrite : public DbXmlVisitor
{
public:

  enum WriteMode
  {
    WRITE_NAME_AND_VALUE, // Default.
    WRITE_NAME_ONLY,
  };
  
  DbXmlWrite();

  // Set the mode.
  void                    setMode ( const WriteMode &mode ) { _mode = mode; }

  // The visit functions.
  virtual bool            visit ( DbXmlNode & );
  virtual bool            visit ( DbXmlGroup & );
  virtual bool            visit ( DbXmlLeaf & );

  // Write the output file.
  bool                    write ( const DbXmlNode &node, const char *filename );
  bool                    write ( const DbXmlNode &node, std::ostream &out );

protected:

  std::string _indention;
  std::ostream *_out;
  WriteMode _mode;

  virtual ~DbXmlWrite();

  DB_XML_DECLARE_DYNAMIC_VISITOR ( DbXmlWrite, 0x00001013 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_XML_LIBRARY_WRITER_H_
