
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

#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Data/Object.h"
#include "Minerva/Core/Data/TimePrimitive.h"
#include "Minerva/Core/Data/LookAt.h"

#include "Usul/Containers/Unknowns.h"
#include "Usul/Interfaces/IDataChangedListener.h"
#include "Usul/Interfaces/IDataChangedNotify.h"
#include "Usul/Interfaces/ILayerExtents.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT Feature : public Minerva::Core::Data::Object,
                               public Usul::Interfaces::IDataChangedNotify,
                               public Usul::Interfaces::ILayerExtents
{
public:
  typedef Minerva::Core::Data::Object         BaseClass;
  typedef Minerva::Core::Data::TimePrimitive  TimePrimitive;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;

  USUL_DECLARE_QUERY_POINTERS ( Feature );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Set/get the description.
  void                   description ( const std::string& );
  const std::string&     description() const;

  /// Set/get the extents.
  void                   extents ( const Extents& e );
  Extents                extents() const;

  /// Set/get the look at.
  void                   lookAt ( LookAt* );
  LookAt*                lookAt() const;

  /// Set/get the name.
	void                   name ( const std::string& );
  const std::string&     name() const;

  /// Get the min latitude and min longitude (ILayerExtents).
  virtual double         minLon() const;
  virtual double         minLat() const;
  
  /// Get the max latitude and max longitude (ILayerExtents).
  virtual double         maxLon() const;
  virtual double         maxLat() const;

	/// Set/get the style url.
	void                   styleUrl ( const std::string& url );
	const std::string&     styleUrl() const;

  /// Set/get the time primitive.
  void                   timePrimitive ( TimePrimitive* );
  TimePrimitive*         timePrimitive() const;

  /// Set/get the visiblity.
	virtual void           visibility( bool b );
  bool                   visibility() const;
  
protected:

  Feature();
  Feature ( const Feature& rhs );
  virtual ~Feature();

  /// Notify data changed listeners.
  void                        _notifyDataChnagedListeners();

  // Add the listener.
  virtual void                addDataChangedListener ( Usul::Interfaces::IUnknown *caller );

  // Remove the listener.
  virtual void                removeDataChangedListener ( Usul::Interfaces::IUnknown *caller );

private:

  typedef Usul::Interfaces::IDataChangedListener      IDataChangedListener;
  typedef Usul::Containers::Unknowns<IDataChangedListener> DataChangedListeners;

  std::string _description;
  std::string _name;
	std::string _styleUrl;
  bool _visibility;
  LookAt::RefPtr _lookAt;
  TimePrimitive::RefPtr _timePrimitive;
  Extents _extents;
  DataChangedListeners _dataChangedListeners;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_FEATURE_H__
