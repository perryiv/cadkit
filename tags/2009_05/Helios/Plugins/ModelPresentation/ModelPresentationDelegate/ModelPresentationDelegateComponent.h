
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

#ifndef _MODEL_PRESENTATION_DELEGATE_COMPONENT_CLASS_H_
#define _MODEL_PRESENTATION_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Helios/Plugins/ModelPresentation/ModelPresentationLib/ModelPresentationLib.h"
#include "MpdDialogDefinitions.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IInitNewDocument.h"

#include <string>


class ModelPresentationDelegateComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IGUIDelegate,
                               public Usul::Interfaces::IInitNewDocument
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModelPresentationDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ModelPresentationDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Model Presentation Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

  // Usul::Interfaces::IInitNewDocument.
  virtual bool                handlesDocumentType ( Unknown *document );
  virtual void                initNewDocument ( Unknown *document, Unknown *caller );

protected: 

  // Do not copy.
  ModelPresentationDelegateComponent ( const ModelPresentationDelegateComponent & );
  ModelPresentationDelegateComponent &operator = ( const ModelPresentationDelegateComponent & );

  void                        _parseGroups( MpdDialogDefinitions::Set set, MpdWriter* writer );
  void                        _parseModels( MpdDialogDefinitions::ModelList models, MpdWriter* writer );

  /// Use reference counting.
  virtual ~ModelPresentationDelegateComponent();


};


#endif // _MODEL_PRESENTATION_DELEGATE_COMPONENT_CLASS_H_
