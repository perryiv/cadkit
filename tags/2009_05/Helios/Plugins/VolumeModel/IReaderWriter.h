
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_VOLUME_MODEL_READER_WRITER_H__
#define __HELIOS_VOLUME_MODEL_READER_WRITER_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace osg { class Image; }
class VolumeDocument;

class IReaderWriter : public Usul::Interfaces::IUnknown
{
public:
  typedef Usul::Interfaces::IUnknown            Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReaderWriter );

  /// Id for this interface.
  enum { IID = 3734913963u };

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 ) = 0;

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, VolumeDocument &doc, Unknown *caller = 0x0 ) = 0;

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, const VolumeDocument &doc, Unknown *caller = 0x0  ) const = 0;
};


#endif // __HELIOS_VOLUME_MODEL_READER_WRITER_H__
