
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
//  Quads attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_QUADS_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_QUADS_ATTRIBUTE_CLASS_H_

#include "ModflowModel/Attributes/Attribute.h"
#include "ModflowModel/Constants.h"


namespace Modflow {
namespace Attributes {


class Quads : public Modflow::Attributes::Attribute
{
public:

  // Type information.
  USUL_DECLARE_TYPE_ID ( Quads );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Quads );

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;
  typedef Usul::Pointers::WeakPointer < Quads > WeakPtr;

  // Construction.
  Quads ( const std::string &name, IUnknown *parent, Modflow::Geometry::ID geometry, const RegistryPath & );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::ModflowDocument *document, Modflow::Model::Layer *, IUnknown *caller );

  // Clear the attribute.
  virtual void                clear();

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );

  // Usul::Interfaces::IStringGridGet.
  virtual void                getStringGrid ( IStringGridGet::StringGrid & ) const;

protected:

  // Use reference counting.
  virtual ~Quads();

private:

  // Do not copy.
  Quads ( const Quads & );
  Quads &operator = ( const Quads & );

  void                        _destroy();

  Modflow::Geometry::ID _geometry;
  GroupPtr _root;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_MODEL_QUADS_ATTRIBUTE_CLASS_H_
