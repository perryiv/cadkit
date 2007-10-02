
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Quads attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_QUADS_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_QUADS_ATTRIBUTE_CLASS_H_

#include "Helios/Plugins/ModflowModel/Attributes/Attribute.h"


namespace Modflow {
namespace Attributes {


class Quads : public Modflow::Attributes::Attribute
{
public:

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Quads );

  // Construction.
  Quads ( const std::string &name, IUnknown *parent, unsigned int sides );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::Model::Layer * );

protected:

  // Use reference counting.
  virtual ~Quads();

private:

  // Do not copy.
  Quads ( const Quads & );
  Quads &operator = ( const Quads & );

  void                        _destroy();

  unsigned int _sides;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_MODEL_QUADS_ATTRIBUTE_CLASS_H_
