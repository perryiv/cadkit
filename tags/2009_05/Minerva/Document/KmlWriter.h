
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

#include "Minerva/Document/MinervaDocument.h"
#include "Minerva/Core/Visitor.h"

#include <fstream>

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

  class Visitor : public Minerva::Core::Visitor
  {
  public:
    typedef Minerva::Core::Visitor BaseClass;

    USUL_DECLARE_REF_POINTERS ( Visitor );

    Visitor ( const std::string & filename );

    virtual void visit ( Minerva::Core::Data::Container& layer );

  protected:
    virtual ~Visitor ();

  private:
    std::string _filename;
    std::ofstream _out;
  };

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
