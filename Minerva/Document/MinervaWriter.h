
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_WRITER_H__
#define __MINERVA_WRITER_H__

#include "Minerva/Core/Serialize.h"

#include "MinervaDocument.h"

#include <string>

namespace Minerva {
namespace Document {

class MinervaWriter
{
public:
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Construction/Destruction.
  MinervaWriter( const std::string& filename, Unknown* caller, const MinervaDocument* doc );
  ~MinervaWriter();

  void operator()();

private:

  // No copying.
  MinervaWriter ( const MinervaWriter& );
  MinervaWriter & operator=( const MinervaWriter& );

  std::string _filename;
  Unknown::RefPtr _caller;
  MinervaDocument::RefPtr _document;
};

}
}

#endif // __MINERVA_WRITER_H__
