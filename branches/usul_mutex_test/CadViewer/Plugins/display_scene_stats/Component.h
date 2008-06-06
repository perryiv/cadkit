
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

#ifndef _CV_PLUGIN_DISPLAY_SCENE_STATS_COMPONENT_CLASS_H_
#define _CV_PLUGIN_DISPLAY_SCENE_STATS_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "VRV/Interfaces/IPostModelLoad.h"

#include <vector>
#include <iosfwd>
#include <string>

namespace osg { class Node; class Geode; class MatrixTransform; };


namespace CV {
namespace Plugins {


class Component : public Usul::Base::Referenced,
                  public VRV::Interfaces::IPostModelLoad,
                  public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector < unsigned long > Counts;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Component );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Component();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IPostModelLoad
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Display some statistical information about the model.
  virtual void            postProcessModelLoad ( const std::string &filename, osg::Node *model );

protected:

  // Do not copy.
  Component ( const Component & );
  Component &operator = ( const Component & );

  /// Use reference counting.
  virtual ~Component();

  void                    _accumulateStatsGeode ( osg::Geode * );
  void                    _accumulateStatsMT ( osg::MatrixTransform * );
  void                    _print ( const std::string &filename, std::ostream & ) const;

  /// Usul::Interface::IPlugin
  virtual std::string getPluginName() const;

private:

  Counts _counts;
};


}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_DISPLAY_SCENE_STATS_COMPONENT_CLASS_H_
