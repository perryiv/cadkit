
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Head levels attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_ATTRIBUTES_HEAD_LEVELS_CLASS_H_
#define _MODFLOW_ATTRIBUTES_HEAD_LEVELS_CLASS_H_

#include "ModflowModel/Attributes/Attribute.h"

#include "Usul/Math/Vector3.h"

#include "osg/ref_ptr"
#include "osg/Switch"

namespace Modflow { namespace Model { class Layer; } }


namespace Modflow {
namespace Attributes {


class HeadLevels : public Modflow::Attributes::Attribute
{
public:

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Pointers::WeakPointer < HeadLevels > WeakPtr;
  typedef osg::ref_ptr<osg::Switch> SwitchPtr;
  typedef osg::ref_ptr<osg::Group> GroupPtr;
  typedef Usul::Math::Vec3d Vec3d;

  // Type information.
  USUL_DECLARE_TYPE_ID ( HeadLevels );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HeadLevels );

  // Construction.
  HeadLevels ( const std::string &name, Modflow::Model::Layer *layer, const RegistryPath & );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::ModflowDocument *, Modflow::Model::Layer *, IUnknown *caller );

  // Clear the attribute.
  virtual void                clear();

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );

  // Two-step construction.
  void                        init ( Modflow::ModflowDocument *, Modflow::Model::Layer * );

  // Update this attribute.
  virtual void                update ( Usul::Interfaces::IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~HeadLevels();

  virtual osg::Node *         _buildTimeStep ( unsigned int timeStep, Modflow::ModflowDocument *document, Modflow::Model::Layer *, bool hasNoData, long noData, const Vec3d &offset, double scale, std::ostream *out = 0x0 ) const;

private:

  // Do not copy.
  HeadLevels ( const HeadLevels & );
  HeadLevels &operator = ( const HeadLevels & );

  void                        _destroy();

  SwitchPtr _switch;
  GroupPtr _steps;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_ATTRIBUTES_HEAD_LEVELS_CLASS_H_

