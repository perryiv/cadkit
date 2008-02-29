
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_FEATURE_H__
#define __MINERVA_LAYERS_KML_FEATURE_H__

#include "Minerva/Layers/Kml/Object.h"
#include "Minerva/Layers/Kml/LookAt.h"


namespace Minerva {
namespace Layers {
namespace Kml {
      

class Feature : public Minerva::Layers::Kml::Object
{
public:
  typedef Minerva::Layers::Kml::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Feature );
  
  Feature();
  Feature ( const XmlTree::Node& node );
  
  /// Get/set the name.
  const std::string&     name() const;
  void                   name ( const std::string& );
  
  /// Get/set the visiblity.
  bool                   visiblity() const;
  void                   visiblity( bool b );
  
protected:
  
  virtual ~Feature();
  
private:
  
  std::string _name;
  bool _visiblity;
  LookAt::RefPtr _lookAt;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_FEATURE_H__
