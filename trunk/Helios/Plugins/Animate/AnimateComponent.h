
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATE_COMPONENT_CLASS_H_
#define _ANIMATE_COMPONENT_CLASS_H_

#include "Helios/Plugins/Animate/CompileGuard.h"
#include "Helios/Plugins/Animate/Path.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include "MenuKit/Menu.h"

#include <vector>

namespace Animate {


class AnimateComponent : public Usul::Base::Object,
                         public Usul::Interfaces::IPlugin,
                         public Usul::Interfaces::IMenuAdd
{
public:

  /// Typedefs.
  typedef Usul::Base::Object              BaseClass;
  typedef Usul::Interfaces::IUnknown      IUnknown;
  typedef Animate::Path                   Path;
  typedef std::vector < Path::RefPtr >    Paths;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AnimateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  AnimateComponent();

  //  Usul::Interfaces::IPlugin
  virtual std::string       getPluginName (  ) const { return "Animate"; }

  /// Get/Set the current path.
  Path*                   currentPath ();
  const Path*             currentPath () const;
  void                    setCurrentPath ( Path::RefPtr );
  bool                    isCurrentPath ( Path::RefPtr ) const;

  /// Create new key frame path.
  void                    createKeyFramePath ();

  /// Create new recorded path.
  void                    createRecordedPath ();

  /// Append current camera.
  void                    appendCamera ();

  /// Start the animation.
  void                    startAnimation ();

  /// Clear animation.
  void                    clearAnimation ();

  /// Get/Set flag to record path.
  void                    recordPath ( bool b );
  bool                    recordPath ( ) const;

  /// Do we have a valid path.
  bool                    ensureValidPath () const;
  bool                    ensureRecordedPath () const;
  bool                    ensureKeyFramePath () const;
protected:

  /// Build the menu for the current paths.
  void                       _buildMenu ();

  /// Usul::Interfaces::IMenuAdd.
  virtual void               menuAdd ( MenuKit::Menu& menu );


  // Do not copy.
  AnimateComponent ( const AnimateComponent & );
  AnimateComponent &operator = ( const AnimateComponent & );

  /// Use reference counting.
  virtual ~AnimateComponent();

private:
  Paths                                  _paths;
  Path::RefPtr                           _currentPath;
  MenuKit::Menu::RefPtr                  _pathsMenu;
};


}; // namespace Animate


#endif // _ANIMATE_COMPONENT_CLASS_H_
