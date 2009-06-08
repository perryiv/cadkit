
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_VOLUME_MODEL_RAW_READER_WRITER_H__
#define __HELIOS_VOLUME_MODEL_RAW_READER_WRITER_H__

#include "Helios/Plugins/VolumeModel/IReaderWriter.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector3.h"

#include "osg/Image"
#include "osg/ref_ptr"

namespace XmlTree { class Node; }

class RawReaderWriter : public Usul::Base::Object,
                        public IReaderWriter
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  RawReaderWriter();

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, VolumeDocument &doc, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, const VolumeDocument &doc, Unknown *caller = 0x0  ) const;

protected:
  virtual ~RawReaderWriter();

  RawReaderWriter ( const RawReaderWriter& rhs );
  RawReaderWriter& operator= ( const RawReaderWriter& rhs );

  void                        _addTransferFunction ( VolumeDocument& doc, XmlTree::Node& node );

private:
  
  std::string _filename;
  Usul::Math::Vec3ui _size;
};


#endif // __HELIOS_VOLUME_MODEL_RAW_READER_WRITER_H__
