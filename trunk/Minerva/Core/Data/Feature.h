
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_FEATURE_H__
#define __MINERVA_CORE_DATA_FEATURE_H__

#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Data/Object.h"
#include "Minerva/Core/Data/TimePrimitive.h"
#include "Minerva/Core/Data/LookAt.h"
#include "Minerva/Interfaces/IFeature.h"

#include "Usul/Containers/Unknowns.h"
#include "Usul/Interfaces/IDataChangedListener.h"
#include "Usul/Interfaces/IDataChangedNotify.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Threads/Atomic.h"
#include "Usul/Threads/Object.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
  
class Visitor;
  
namespace Data {


class MINERVA_EXPORT Feature : public Minerva::Core::Data::Object,
                               public Usul::Interfaces::IDataChangedNotify,
                               public Usul::Interfaces::ILayerExtents,
                               public Usul::Interfaces::ITreeNode,
                               public Minerva::Interfaces::IFeature
{
public:
  typedef Minerva::Core::Data::Object         BaseClass;
  typedef Minerva::Core::Data::TimePrimitive  TimePrimitive;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;

  USUL_DECLARE_QUERY_POINTERS ( Feature );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Get this as an IUnknown.
	virtual IUnknown*      asUnknown();

  /// Accept the visitor.
  virtual void           accept ( Minerva::Core::Visitor& visitor );

  /// Get/Set the default cache directory.
  static void            defaultCacheDirectory ( const std::string& );
  static std::string     defaultCacheDirectory();
  
  /// Set/get the description.
  void                   description ( const std::string& );
  std::string            description() const;

  /// Set/get the extents.
  void                   extents ( const Extents& e );
  Extents                extents() const;

  /// Set/get the look at.
  void                   lookAt ( LookAt* );
  LookAt*                lookAt() const;

  /// Set/get the name.
	void                   name ( const std::string& );
  std::string            name() const;

  /// Get the min latitude and min longitude (ILayerExtents).
  virtual double         minLon() const;
  virtual double         minLat() const;
  
  /// Get the max latitude and max longitude (ILayerExtents).
  virtual double         maxLon() const;
  virtual double         maxLat() const;

	/// Set/get the style url.
	void                   styleUrl ( const std::string& url );
	std::string            styleUrl() const;

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

  /// Set the name.
  void                        _nameSet ( const std::string& name );

  /// Notify data changed listeners.
  void                        _notifyDataChangedListeners();
  
  /// Update the extents.
  void                        _updateExtents ( Usul::Interfaces::IUnknown* unknown );

  // Add the listener (IDataChangedNotify).
  virtual void                addDataChangedListener ( Usul::Interfaces::IUnknown *caller );

  // Remove the listener (IDataChangedNotify).
  virtual void                removeDataChangedListener ( Usul::Interfaces::IUnknown *caller );

  // Get the feature.
  virtual Feature*            feature();
  
  // Get the number of children (ITreeNode).
  virtual unsigned int        getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode::RefPtr   getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;

private:

  typedef Usul::Interfaces::IDataChangedListener      IDataChangedListener;
  typedef Usul::Containers::Unknowns<IDataChangedListener> DataChangedListeners;
  typedef Usul::Threads::Object<std::string,Usul::Threads::MutexTraits<Usul::Threads::Mutex> > String;
  typedef Usul::Threads::Atomic<bool> Boolean;

  std::string _description;
  String _name;
	std::string _styleUrl;
  Boolean _visibility;
  LookAt::RefPtr _lookAt;
  TimePrimitive::RefPtr _timePrimitive;
  Extents _extents;
  DataChangedListeners _dataChangedListeners;
};


}
}
}

#endif // __MINERVA_CORE_DATA_FEATURE_H__
