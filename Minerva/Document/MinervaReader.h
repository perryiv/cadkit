
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_READER_H__
#define __MINERVA_READER_H__

#include "MinervaDocument.h"

#include <string>

namespace Minerva {
namespace Document {

class MinervaReader
{
public:
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Construction/Destruction.
  MinervaReader( const std::string& filename, Unknown* caller, MinervaDocument* doc );
  ~MinervaReader();

  void operator()();

private:

  // No copying.
  MinervaReader ( const MinervaReader& );
  MinervaReader & operator=( const MinervaReader& );

  std::string _filename;
  Unknown::RefPtr _caller;
  MinervaDocument::RefPtr _document;
};

}
}

#endif // __MINERVA_READER_H__
