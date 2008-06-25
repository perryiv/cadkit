
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
//  Cylinders attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_ATTRIBUTES_CYLINDERS_CLASS_H_
#define _MODFLOW_ATTRIBUTES_CYLINDERS_CLASS_H_

#include "ModflowModel/Attributes/Attribute.h"

#include "OsgTools/ShapeFactory.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Types/Types.h"

#include "osg/ref_ptr"
#include "osg/Switch"

#include "boost/shared_ptr.hpp"

#include <vector>

namespace Modflow { namespace Model { class Layer; } }
namespace OsgTools { class ShapeFactory; }


namespace Modflow {
namespace Attributes {


class Cylinders : public Modflow::Attributes::Attribute
{
public:

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;
  typedef osg::ref_ptr<osg::Switch> SwitchPtr;
  typedef osg::ref_ptr<osg::Group> GroupPtr;
  typedef Usul::Types::Uint64 TimeStamp;
  typedef std::vector<double> Values;
  typedef boost::shared_ptr<Values> ValuesPtr;
  typedef std::map<TimeStamp,ValuesPtr> ValuesMap;
  typedef Usul::Math::Vec3d Vec3d;
  typedef osg::ref_ptr<OsgTools::ShapeFactory> ShapeFactoryPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Cylinders );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Cylinders );

  // Construction.
  Cylinders ( const std::string &name, bool needToInit, const RegistryPath & );

  // Add the data. Throws if there are already values at the given time.
  void                        addValues ( const TimeStamp &start, const Values &v, const TimeStamp &num = 1 );
  void                        addValues ( const TimeStamp &start, ValuesPtr v,     const TimeStamp &num = 1 );

  // Sort the triangles?
  void                        sortTriangles ( const std::string & );
  void                        sortTriangles ( bool );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::ModflowDocument *, Modflow::Model::Layer *, IUnknown *caller );

  // Clear the attribute.
  virtual void                clear();

  // Fill in missing values for all time-stamps starting with the first one.
  void                        fill ( const TimeStamp &last );

  // Set the height scale. Default is 1.
  void                        heightScale ( const std::string & );
  void                        heightScale ( double );

  // Set the radius scale. Default is 1.
  void                        radiusScale ( const std::string & );
  void                        radiusScale ( double );

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );

  // Update this attribute.
  virtual void                update ( Usul::Interfaces::IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~Cylinders();

  osg::Node *                 _buildTimeStep ( unsigned int timeStep, Modflow::ModflowDocument *, const Vec3d &offset, double scale ) const;
  void                        _buildTimeSteps ( Modflow::ModflowDocument * );

  const ValuesPtr             _getValues ( const TimeStamp &stamp ) const;

private:

  // Do not copy.
  Cylinders ( const Cylinders & );
  Cylinders &operator = ( const Cylinders & );

  void                        _destroy();

  SwitchPtr _switch;
  GroupPtr _steps;
  ValuesMap _values;
  ShapeFactoryPtr _shapes;
  bool _needToInit;
  double _heightScale;
  double _radiusScale;
  bool _sortTriangles;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_ATTRIBUTES_CYLINDERS_CLASS_H_
