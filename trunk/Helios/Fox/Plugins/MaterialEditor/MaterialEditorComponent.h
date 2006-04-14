
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

#ifndef _MATERIAL_EDITOR_COMPONENT_CLASS_H_
#define _MATERIAL_EDITOR_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMaterialEditor.h"

#include "osg/ref_ptr"
#include "osg/Material"

#include <string>
#include <map>

namespace FX { class FXComposite; }


class MaterialEditorComponent : public Usul::Base::Referenced,
                                public FX::FXObject,
                                public Usul::Interfaces::IPlugin,
                                public Usul::Interfaces::IMaterialEditor
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MaterialEditorComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  MaterialEditorComponent();

protected: 

  enum FrameID { ACTION_BUTTONS, COLOR_BUTTONS, COLOR_SELECTOR, MATERIAL_PALETTE };
  typedef std::map < FrameID, FX::FXComposite * > FrameMap;

  // Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Material Editor"; }

  // Usul::Interfaces::IMaterialEditor
  // Run the material editor inside a modal dialog.
  virtual void                runModalDialog();

  // Usul::Interfaces::IMaterialEditor
  // Set the current material.
  virtual void                setCurrentMaterial ( osg::Material * );

  // Do not copy.
  MaterialEditorComponent ( const MaterialEditorComponent & );
  MaterialEditorComponent &operator = ( const MaterialEditorComponent & );

  //// Use reference counting.
  virtual ~MaterialEditorComponent();

  void                        _makeActionButtons   ( FX::FXComposite *parent );
  void                        _makeColorSelector   ( FX::FXComposite *parent );
  void                        _makeColorButtons    ( FX::FXComposite *parent );
  void                        _makeColorButton     ( FX::FXComposite *parent, const std::string &text );
  FrameMap                    _makeFrames          ( FX::FXComposite *parent );
  void                        _makeMaterialPalette ( FX::FXComposite *parent );

private:

  osg::ref_ptr<osg::Material> _mat;
};


#endif // _MATERIAL_EDITOR_COMPONENT_CLASS_H_

