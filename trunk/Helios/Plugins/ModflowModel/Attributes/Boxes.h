
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Boxes attribute.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BOX_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_BOX_ATTRIBUTE_CLASS_H_

#include "Helios/Plugins/ModflowModel/Attributes/Quads.h"

#include <vector>

namespace Modflow {
namespace Attributes {


class Boxes : public Modflow::Attributes::Attribute
{
public:

  // Useful typedefs.
  typedef Modflow::Attributes::Attribute BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef std::vector<Quads::RefPtr> Sides;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Boxes );

  // Construction.
  Boxes ( const std::string &name, IUnknown *parent );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::Model::Layer * );

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int        getNumChildNodes() const;
  virtual const ITreeNode *   getChildNode ( unsigned int which ) const;
  virtual ITreeNode *         getChildNode ( unsigned int which );

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );

protected:

  // Use reference counting.
  virtual ~Boxes();

private:

  // Do not copy.
  Boxes ( const Boxes & );
  Boxes &operator = ( const Boxes & );

  void                        _destroy();

  Sides _sides;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_MODEL_BOX_ATTRIBUTE_CLASS_H_
