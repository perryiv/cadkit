
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOLUME_MODEL_COMPONENT_CLASS_H_
#define _VOLUME_MODEL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/INewDocumentCreate.h"

#include <string>

class VolumeDocument;


class VolumeComponent : public Usul::Base::Referenced,
                        public Usul::Interfaces::IPlugin,
                        public Usul::Interfaces::IDocumentCreate,
                        public Usul::Interfaces::INewDocumentCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VolumeComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  VolumeComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Volume Model"; }

  /// Usul::Interfaces::IDocumentCreate
  virtual Document *          createDocument ( Unknown *caller = 0x0 );

  /// Usul::Interfaces::INewDocumentCreate
  virtual std::string         documentTypeName();

protected: 

  friend class VolumeDocument;

  // Do not copy.
  VolumeComponent ( const VolumeComponent & );
  VolumeComponent &operator = ( const VolumeComponent & );

  /// Use reference counting.
  virtual ~VolumeComponent();
};


#endif // _VOLUME_MODEL_COMPONENT_CLASS_H_


