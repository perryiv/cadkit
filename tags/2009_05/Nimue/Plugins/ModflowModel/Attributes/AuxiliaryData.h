
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
//  Auxiliary data attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_ATTRIBUTES_AUXILIARY_DATA_CLASS_H_
#define _MODFLOW_ATTRIBUTES_AUXILIARY_DATA_CLASS_H_

#include "ModflowModel/Attributes/Attribute.h"

namespace osg { class Group; }


namespace Modflow {
namespace Attributes {


class AuxiliaryData : public Modflow::Attributes::Attribute
{
public:

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Pointers::WeakPointer < AuxiliaryData > WeakPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( AuxiliaryData );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AuxiliaryData );

  // Construction.
  AuxiliaryData ( const std::string &name, IUnknown *parent, osg::Group *scene, const RegistryPath & );

  // Hide/show.
  virtual void                setBooleanState ( bool );

protected:

  // Use reference counting.
  virtual ~AuxiliaryData();

private:

  // Do not copy.
  AuxiliaryData ( const AuxiliaryData & );
  AuxiliaryData &operator = ( const AuxiliaryData & );

  void                        _destroy();

  GroupPtr _root;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_ATTRIBUTES_AUXILIARY_DATA_CLASS_H_
