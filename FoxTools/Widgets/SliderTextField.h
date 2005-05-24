
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOX_TOOLS_WIDGETS_SLIDER_TEXT_FIELD_H__
#define __FOX_TOOLS_WIDGETS_SLIDER_TEXT_FIELD_H__

#include "FoxTools/Export/Export.h"

#include "FoxTools/Headers/HorizontalFrame.h"

namespace FX { class FXSlider; class FXTextField; }

namespace FoxTools {
namespace Widgets {

class FOX_TOOLS_EXPORT SliderTextField : public FX::FXHorizontalFrame
{
  FXDECLARE( SliderTextField )
public:
  typedef FX::FXHorizontalFrame BaseClass;

  SliderTextField ( FX::FXComposite *parent, FX::FXObject* tgt = 0x0, FX::FXSelector sel = 0, int start = 0, int finish = 255 );
  SliderTextField ( FX::FXComposite *parent, char *label, FX::FXObject* tgt = 0x0, FX::FXSelector sel = 0, int start = 0, int finish = 255 );

  virtual ~SliderTextField ();

  enum
  {
    ID_SLIDER = BaseClass::ID_LAST,
    ID_TEXT,
    ID_LAST
  };

  int getValue() const;
  void setValue ( int value );

  long onCommandSlider   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandText   ( FX::FXObject *, FX::FXSelector, void * );
  

  long onChangeSlider ( FX::FXObject *, FX::FXSelector, void * );


private:
  SliderTextField() { }

  int _start;
  int _finish;
  FX::FXSlider * _slider;
  FX::FXTextField * _textField;
};

}
}


#endif // __FOX_TOOLS_WIDGETS_SLIDER_TEXT_FIELD_H__