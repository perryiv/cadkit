
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _READ_WRITE_TIFF_COMPONENT_CLASS_H_
#define _READ_WRITE_TIFF_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Types/Types.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/IReadTIFF.h"
#include "Usul/Interfaces/IReadProperties.h"
#include "Usul/Interfaces/IGetImageProperties.h"
#include "Usul/Interfaces/IGetImageData.h"

#include <string>
#include <vector>

typedef	struct tiff TIFF;


class TiffComponent : public Usul::Base::Referenced,
                      public Usul::Interfaces::IPlugin,
                      public Usul::Interfaces::IRead,
                      public Usul::Interfaces::IReadTIFF,
                      public Usul::Interfaces::IReadProperties,
                      public Usul::Interfaces::IGetImageProperties,
                      public Usul::Interfaces::IGetImageDataUint8,
                      public Usul::Interfaces::IGetImageDataUint16,
                      public Usul::Interfaces::IGetImageDataUint32,
                      public Usul::Interfaces::IGetImageDataFloat32,
                      public Usul::Interfaces::IGetImageDataFloat64
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::IGetImageDataUint8::Values DataUint8;
  typedef Usul::Interfaces::IGetImageDataUint16::Values DataUint16;
  typedef Usul::Interfaces::IGetImageDataUint32::Values DataUint32;
  typedef Usul::Interfaces::IGetImageDataFloat32::Values DataFloat32;
  typedef Usul::Interfaces::IGetImageDataFloat64::Values DataFloat64;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TiffComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  TiffComponent();

  // Get the image dimensions.
  virtual void                  getImageDimensions ( unsigned int &rows, unsigned int &columns, unsigned int &layers, unsigned int &channels ) const;

  // Get the image values.
  virtual void                  getImageValues ( DataUint8   & ) const;
  virtual void                  getImageValues ( DataUint16  & ) const;
  virtual void                  getImageValues ( DataUint32  & ) const;
  virtual void                  getImageValues ( DataFloat32 & ) const;
  virtual void                  getImageValues ( DataFloat64 & ) const;

  // Get the number of bytes per value.
  virtual unsigned int          getNumBytesPerValue() const;

  // Return the name of the plugin.
  virtual std::string           getPluginName() const;

  // See if the scalar values are integers.
  virtual bool                  isValueInteger() const;

  // Read the file.
  virtual void                  read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Read the image properties.
  virtual void                  readProperties ( const std::string &filename );

protected:

  // Do not copy.
  TiffComponent ( const TiffComponent & );
  TiffComponent &operator = ( const TiffComponent & );

  /// Use reference counting.
  virtual ~TiffComponent();

  void                          _init();

  void                          _readProperties ( ::TIFF *in, const std::string &filename );

private:

  DataUint8   _dataUint8;
  DataUint16  _dataUint16;
  DataUint32  _dataUint32;
  DataFloat32 _dataFloat32;
  DataFloat64 _dataFloat64;
  Usul::Types::Uint16 _bytes;
  Usul::Types::Uint16 _channels;
  bool _integer;
  Usul::Types::Uint32 _rows;
  Usul::Types::Uint32 _columns;
  Usul::Types::Uint32 _layers;
};


#endif // _READ_WRITE_TIFF_COMPONENT_CLASS_H_


