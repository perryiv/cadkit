
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The state attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTES_H_
#define _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTES_H_

#include "GSG/Core/Material.h"


namespace GSG {


class GSG_CORE_EXPORT Attributes : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Attributes );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Attributes, Referenced );

  explicit Attributes();
  Attributes ( const Attributes &a );

  // Get/set the front material.
  const Material *      frontMaterial() const;
  Material *            frontMaterial();
  void                  frontMaterial ( Material *m );

  // Get/set the back material.
  const Material *      backMaterial() const;
  Material *            backMaterial();
  void                  backMaterial ( Material *m );

protected:

  virtual ~Attributes();

private:

  Material::Ptr _frontMaterial;
  Material::Ptr _backMaterial;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_STATE_ATTRIBUTES_H_
