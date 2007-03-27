
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_PATENT_LAYER_H__
#define __MINERVA_LAYERS_PATENT_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"

namespace Minerva {
namespace Core {
namespace Layers {

  class MINERVA_EXPORT PatentLayer : public Minerva::Core::Layers::PointTimeLayer
{
public:
  typedef Minerva::Core::Layers::PointTimeLayer BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PatentLayer );

  PatentLayer();

  /// Build the scene.
  virtual void            buildScene( osg::Group* parent );

  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  virtual void            modify ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~PatentLayer();

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version )
  {
    ar & boost::serialization::make_nvp( "PointTimeLayer", boost::serialization::base_object< BaseClass >(*this) );
  }

};

}
}
}


#endif // __MINERVA_LAYERS_PATENT_LAYER_H__
