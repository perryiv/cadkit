
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

#include "Minerva/Core/Data/Object.h"
#include "Minerva/Core/Data/TimePrimitive.h"

#include "Minerva/Core/Data/LookAt.h"

namespace Minerva {
namespace Core {
namespace Data {
      

class MINERVA_EXPORT Feature : public Minerva::Core::Data::Object
{
public:
  typedef Minerva::Core::Data::Object BaseClass;
  typedef Minerva::Core::Data::TimePrimitive TimePrimitive;
  
  USUL_DECLARE_REF_POINTERS ( Feature );
  
  /// Set/get the description.
  void                   description ( const std::string& );
  const std::string&     description() const;
  
  /// Set/get the look at.
  void                   lookAt ( LookAt* );
  LookAt*                lookAt() const;
  
  /// Set/get the name.
	void                   name ( const std::string& );
  const std::string&     name() const;
  
  /// Set/get the visiblity.
	virtual void           visibility( bool b );
  bool                   visibility() const;

	/// Set/get the style url.
	void                   styleUrl ( const std::string& url );
	const std::string&     styleUrl() const;
  
  /// Set/get the time primitive.
  void                   timePrimitive ( TimePrimitive* );
  TimePrimitive*         timePrimitive() const;
  
protected:
  
  Feature();
  virtual ~Feature();
  
private:
  
  std::string _description;
  std::string _name;
	std::string _styleUrl;
  bool _visibility;
  LookAt::RefPtr _lookAt;
  TimePrimitive::RefPtr _timePrimitive;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_FEATURE_H__
