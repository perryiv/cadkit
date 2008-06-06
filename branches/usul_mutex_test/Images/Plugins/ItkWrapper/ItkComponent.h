
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

#ifndef _ITK_WRAPPER_COMPONENT_CLASS_H_
#define _ITK_WRAPPER_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/IReadImage.h"
#include "Usul/Interfaces/IReadProperties.h"
#include "Usul/Interfaces/IGetImageProperties.h"
#include "Usul/Interfaces/IGetImageData.h"


class ItkComponent : public Usul::Base::Referenced,
                     public Usul::Interfaces::IPlugin,
                     public Usul::Interfaces::IRead,
                     public Usul::Interfaces::IReadImage,
                     public Usul::Interfaces::IReadProperties,
                     public Usul::Interfaces::IGetImageProperties,
                     public Usul::Interfaces::IGetImageDataUint8,
                     public Usul::Interfaces::IGetImageDataUint16,
                     public Usul::Interfaces::IGetImageDataUint32,
                     public Usul::Interfaces::IGetImageDataFloat32,
                     public Usul::Interfaces::IGetImageDataFloat64
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::IGetImageDataUint8::Values DataUint8;
  typedef Usul::Interfaces::IGetImageDataUint16::Values DataUint16;
  typedef Usul::Interfaces::IGetImageDataUint32::Values DataUint32;
  typedef Usul::Interfaces::IGetImageDataFloat32::Values DataFloat32;
  typedef Usul::Interfaces::IGetImageDataFloat64::Values DataFloat64;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ItkComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  ItkComponent();

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
  virtual void                  read ( const std::string &filename, Unknown *caller = 0x0 );

  // Read the image properties.
  virtual void                  readProperties ( const std::string &filename );

protected:

  // Do not copy.
  ItkComponent ( const ItkComponent & );
  ItkComponent &operator = ( const ItkComponent & );

  // Use reference counting.
  virtual ~ItkComponent();

  void                          _init();

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


#endif // _ITK_WRAPPER_COMPONENT_CLASS_H_


