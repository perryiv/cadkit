
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Slider for toolbar.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_TOOLBAR_SLIDER_H_
#define _FOX_TOOLS_TOOLBAR_SLIDER_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Command.h"

#include <string>

namespace FX { class FXSlider; }


namespace FoxTools {
namespace ToolBar {

class Bar;

class FOX_TOOLS_EXPORT Slider : public FoxTools::Items::Command
{
public:
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Slider );

  Slider ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0  );

  // Clear the button
  virtual void      clear();

  // Get the slider
  FX::FXSlider*   slider() { return _slider; }

protected:

  // Use reference counting.
  virtual ~Slider();

  virtual void          _build ( FX::FXComposite *parent );

  // Create
  virtual void          _create ();

private:

  friend class Bar;

  // No copying.
  Slider ( const Slider & );
  Slider &operator = ( const Slider & );

  FX::FXSlider *_slider;

};


} // namespace ToolBar
} // namespace FoxTools


#endif // _FOX_TOOLS_TOOLBAR_SLIDER_H_

