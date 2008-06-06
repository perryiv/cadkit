
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_LOOK_AT_H__
#define __MINERVA_LAYERS_KML_LOOK_AT_H__

#include "Minerva/Layers/Kml/Object.h"

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {


class LookAt : public Minerva::Layers::Kml::Object
{
public:
  typedef Minerva::Layers::Kml::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( LookAt );

  LookAt();
  LookAt ( const XmlTree::Node& node );

protected:

  virtual ~LookAt();

private:
  double _longitude;
  double _latitude;
  double _altitude;
  double _range;
  double _heading;
};

  
}
}
}


#endif // __MINERVA_LAYERS_KML_LOOK_AT_H__
