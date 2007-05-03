
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DOCUMENT_KML_WRITER_H__
#define __MINERVA_DOCUMENT_KML_WRITER_H__

#include "MinervaDocument.h"

#include <iosfwd>

namespace Minerva {
namespace Document {

class KmlWriter
{
public:
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Construction/Destruction.
  KmlWriter( const std::string& filename, Unknown* caller, const MinervaDocument* doc );
  ~KmlWriter();

  void operator()();

protected:
  void               _writeHeader( std::ofstream& out ) const;
  void               _writeFooter( std::ofstream& out ) const;
  void               _writeLayer ( std::ofstream& out, Usul::Interfaces::ILayer* layer ) const;
  void               _writeImageLayer     ( std::ofstream& out, Usul::Interfaces::ILayer* layer ) const;
  void               _writeVectorLayer    ( std::ofstream& out, Usul::Interfaces::ILayer* layer ) const;
  void               _writeDataObject     ( std::ofstream& out, Minerva::Core::DataObjects::DataObject * dataObject ) const;
  
private:

  // No copying.
  KmlWriter ( const KmlWriter& );
  KmlWriter & operator=( const KmlWriter& );

  std::string _filename;
  Unknown::RefPtr _caller;
  MinervaDocument::RefPtr _document;
};

}
}

#endif // __MINERVA_DOCUMENT_KML_WRITER_H__
