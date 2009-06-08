
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

#ifndef _SCENE_STATISTICS_OSG_COMPONENT_CLASS_H_
#define _SCENE_STATISTICS_OSG_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IPlugin.h"

#include <ostream>
#include <map>
#include <typeinfo>

namespace osg { class Node; class Geode; class MatrixTransform; class Referenced; };


namespace SceneStats {
namespace OSG {


class SceneStatsOSGComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IMenuAdd,
                               public Usul::Interfaces::ICommand,
                               public Usul::Interfaces::IPlugin
{
public:


  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::type_info TypeInfo;
  typedef std::map < const TypeInfo *,       unsigned long > Objects;
  typedef std::map < unsigned long,          unsigned long > Modes;
  typedef std::map < const osg::Referenced*, unsigned long > Referenceds;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneStatsOSGComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  SceneStatsOSGComponent();

protected:

  // Do not copy.
  SceneStatsOSGComponent ( const SceneStatsOSGComponent & );
  SceneStatsOSGComponent &operator = ( const SceneStatsOSGComponent & );

  /// Use reference counting.
  virtual ~SceneStatsOSGComponent();

  ///  Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Scene Stats";  }

  ///  Usul::Interfaces::IMenuAdd
	virtual void            menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

  ///  Usul::Interfaces::ICommand
  virtual void            execute ( Usul::Interfaces::IUnknown *caller );

  void                    _accumulateStatsGeode ( osg::Geode * );
  void                    _accumulateStatsMT ( osg::MatrixTransform * );
  void                    _print ( std::ostream & );

private:

  Objects _objects;
  Modes _modes;
  Referenceds _vertices;
  Referenceds _normals;
  Referenceds _colors;
};


} // namespace OSG
} // namespace SceneStats


#endif // _SCENE_STATISTICS_OSG_COMPONENT_CLASS_H_
