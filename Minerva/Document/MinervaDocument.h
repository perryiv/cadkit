
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DOCUMENT_H__
#define __MINERVA_DOCUMENT_H__

#include "Export.h"

#include "Usul/Documents/Document.h"

namespace Minerva {
namespace Document {

class MINERVA_DOCUMENT_EXPORT MinervaDocument : public Usul::Documents::Document
{
public:
  typedef Usul::Documents::Document BaseClass;

  MinervaDocument();

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Get the filters that correspond to what this document can do.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Open the file. Clears any data this document already has.
  virtual void                open ( const std::string &filename, Unknown *caller = 0x0 );

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

protected:
  virtual ~MinervaDocument();

};


}
}

#endif // __MINERVA_DOCUMENT_H__
