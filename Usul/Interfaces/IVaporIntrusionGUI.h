
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
#define __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

namespace Usul {
namespace Interfaces {

struct IVaporIntrusionGUI : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVaporIntrusionGUI );

  /// Id for this interface.
  enum { IID = 1235852807u };

  // Highlight the set of cubes at depth <depth>
  virtual void highlightCells( Usul::Math::Vec3ui set, unsigned int depth ) = 0;
  
  // Set the transparency
  virtual void setAlpha( unsigned int x, unsigned int y, unsigned int z, float alpha ) = 0;

  // Set the transparency
  virtual void setAlpha( float alpha ) = 0;

  // Get/set the dimensions of the space
  virtual Usul::Math::Vec3ui dimensions() = 0;
  virtual void dimensions( Usul::Math::Vec3ui ) = 0;

  // Get/Set the material
  virtual void              setMaterial( unsigned int x, unsigned int y, unsigned int z, Usul::Math::Vec4f c ) = 0;
  virtual Usul::Math::Vec4f getMaterial( unsigned int x, unsigned int y, unsigned int z ) = 0;

  // Request redraw
  virtual void              requestRedraw() = 0;

  virtual void              setValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& value ) = 0;
  virtual void              setNameAt( unsigned int x, unsigned int y, unsigned int z, const std::string& name ) = 0;

}; // struct IVaporIntrusionGUI

}
}

#endif // __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
