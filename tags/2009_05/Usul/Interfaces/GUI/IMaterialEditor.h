
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with a material editor
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MATERIAL_EDITOR_H_
#define _USUL_INTERFACE_MATERIAL_EDITOR_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Material; }


namespace Usul {
namespace Interfaces {


struct IMaterialEditor : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMaterialEditor );

  // Id for this interface.
  enum { IID = 2606275294u };

  // Run the material editor inside a modal dialog.
  virtual void                runModalDialog() = 0;

  // Set the current material.
  virtual void                setCurrentMaterial ( osg::Material * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MATERIAL_EDITOR_H_
