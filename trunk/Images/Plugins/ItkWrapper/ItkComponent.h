
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
#include "Usul/Interfaces/IImageToGrayScale.h"


class ItkComponent : public Usul::Base::Referenced,
                     public Usul::Interfaces::IPlugin,
                     public Usul::Interfaces::IImageToGrayScaleUint8,
                     public Usul::Interfaces::IImageToGrayScaleUint16,
                     public Usul::Interfaces::IImageToGrayScaleUint32,
                     public Usul::Interfaces::IImageToGrayScaleFloat32,
                     public Usul::Interfaces::IImageToGrayScaleFloat64
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::vector<Usul::Types::Uint8> DataUint8;
  typedef std::vector<Usul::Types::Uint16> DataUint16;
  typedef std::vector<Usul::Types::Uint32> DataUint32;
  typedef std::vector<Usul::Types::Float32> DataFloat32;
  typedef std::vector<Usul::Types::Float64> DataFloat64;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ItkComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  ItkComponent();

  // Return the name of the plugin.
  virtual std::string           getPluginName() const;

  // Get the image values.
  virtual void                  toGrayScale ( unsigned int channels, DataUint8 & ) const;
  virtual void                  toGrayScale ( unsigned int channels, DataUint16 & ) const;
  virtual void                  toGrayScale ( unsigned int channels, DataUint32 & ) const;
  virtual void                  toGrayScale ( unsigned int channels, DataFloat32 & ) const;
  virtual void                  toGrayScale ( unsigned int channels, DataFloat64 & ) const;
protected:

  // Do not copy.
  ItkComponent ( const ItkComponent & );
  ItkComponent &operator = ( const ItkComponent & );

  // Use reference counting.
  virtual ~ItkComponent();

  void                          _init();

private:

};


#endif // _ITK_WRAPPER_COMPONENT_CLASS_H_


