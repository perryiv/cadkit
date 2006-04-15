
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_FOX_LIGHT_EDITOR_H__
#define __OSG_FOX_LIGHT_EDITOR_H__

#include "FoxTools/Headers/Object.h"
#include "FoxTools/Headers/DialogBox.h"

#include "Usul/Interfaces/IViewer.h"

#include "osg/ref_ptr"
#include "osg/Light"

#include <string>

namespace FX { class FXWindow; class FXSlider; class FXTextField; class FXTabBook; }

namespace FoxTools { namespace Widgets { class SliderTextField; } }

namespace osg { class Vec4f; typedef Vec4f Vec4; }

namespace Helios {
namespace Views {

class LightEditor : public FX::FXDialogBox
{
  FXDECLARE  ( LightEditor )
public:
  typedef FX::FXDialogBox BaseClass;

  enum
  {
    ID_AMBIENT = 1,
    ID_DIFFUSE,
    ID_SPECULAR,
    ID_LAST
  };

  LightEditor ( FX::FXWindow *parent, osg::Light* light, Usul::Interfaces::IUnknown *caller );

  void show ( );

  long  onCommandAmbient  ( FX::FXObject *, FX::FXSelector, void * );
  long  onCommandDiffuse  ( FX::FXObject *, FX::FXSelector, void * );
  long  onCommandSpecular ( FX::FXObject *, FX::FXSelector, void * );

protected:
  
  //Needed by FOX.  Do not use
  LightEditor () {}

  void  _buildTab ( const std::string& name, FX::FXTabBook *tabBook, FoxTools::Widgets::SliderTextField* sliders[], unsigned int id );
  void  _update   ( const osg::Vec4& vec, FoxTools::Widgets::SliderTextField* sliders[] );

  osg::Vec4 _getVec4 ( FoxTools::Widgets::SliderTextField* sliders[] );

private:
  osg::ref_ptr< osg::Light > _light;
  
  FoxTools::Widgets::SliderTextField *    _ambientSliders[4];
  FoxTools::Widgets::SliderTextField *    _diffuseSliders[4];
  FoxTools::Widgets::SliderTextField *    _specularSliders[4];

  Usul::Interfaces::IViewer::QueryPtr     _viewer;
};

} // namespace Views
} // namespace Helios

#endif // __OSG_FOX_LIGHT_EDITOR_H__
