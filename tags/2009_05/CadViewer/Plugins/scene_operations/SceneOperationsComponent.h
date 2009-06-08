
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_SCENE_OPERATIONS_COMPONENT_CLASS_H_
#define _CV_PLUGIN_SCENE_OPERATIONS_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "CadViewer/Interfaces/IMaterialStack.h"
#include "CadViewer/Interfaces/ISelection.h"
#include "CadViewer/Interfaces/IVisibility.h"

namespace CV {
namespace Plugins {


class Component : public Usul::Base::Referenced,
                  public CV::Interfaces::IMaterialStack,
                  public CV::Interfaces::ISelection,
                  public CV::Interfaces::IVisibility,
                  public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Component );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Component();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IMaterialStack
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Push the material.
  virtual void            pushMaterial ( const osg::Material *mat, osg::Node *node );

  /// Pop the material.
  virtual void            popMaterial  ( osg::Node *node );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::ISelection
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Select the object.
  virtual void            select ( osg::Object * );

  /// Unselect the object.
  virtual void            unselect ( osg::Object * );

  /// Is the object selected?
  virtual bool            isSelected ( const osg::Object * ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IVisibility
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Hide the node.
  virtual void            hide ( osg::Node * );

  /// Show the node.
  virtual void            show ( osg::Node * );

  /// Is the node visible?
  virtual bool            isVisible ( const osg::Node * ) const;

protected:

  // Do not copy.
  Component ( const Component & );
  Component &operator = ( const Component & );

  /// Use reference counting.
  virtual ~Component();

  /// Usul::Interfaces::IPlugin.
  virtual std::string getPluginName() const;
};


}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_SCENE_OPERATIONS_COMPONENT_CLASS_H_
