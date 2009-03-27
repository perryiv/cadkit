
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_DATA_SOURCES_BINARY_STRING_H__
#define __MINERVA_DATA_SOURCES_BINARY_STRING_H__

#include "Minerva/DataSources/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace Minerva {
namespace DataSources {

class MINERVA_DATA_SOURCES_EXPORT BinaryString : public Usul::Base::Referenced
{
public:
  
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector<unsigned char> Bytes;
  typedef Bytes::const_iterator const_iterator;
  
  USUL_DECLARE_REF_POINTERS ( BinaryString );
  
  BinaryString();
  BinaryString ( unsigned char* bytes, unsigned int length );
  
  const_iterator begin() const;
  const_iterator end() const;

  // Get the underlying byte buffer;
  const Bytes& byteBuffer() const;
  
protected:
  
  virtual ~BinaryString();
  
private:
  
  Bytes _bytes;
  
};

}
}

#endif // __MINERVA_DATA_SOURCES_BINARY_STRING_H__
