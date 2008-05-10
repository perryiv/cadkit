
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
#include "Minerva/Layers/Kml/TimePrimitive.h"

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
  
  /// Set/get the name.
	void                   name ( const std::string& );
  const std::string&     name() const;
  
  /// Set/get the visiblity.
	void                   visiblity( bool b );
  bool                   visiblity() const;

	/// Set/get the style url.
	void                   styleUrl ( const std::string& url );
	const std::string&     styleUrl() const;
  
  /// Set/get the time primitive.
  void                   timePrimitive ( TimePrimitive* );
  TimePrimitive*         timePrimitive() const;
  
protected:
  
  virtual ~Feature();
  
private:
  
  std::string _name;
	std::string _styleUrl;
  bool _visiblity;
  LookAt::RefPtr _lookAt;
  TimePrimitive::RefPtr _timePrimitive;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_FEATURE_H__
