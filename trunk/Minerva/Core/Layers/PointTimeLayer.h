
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_POINT_TIME_LAYER_H__
#define __WNV_LAYERS_POINT_TIME_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Layer.h"

#include "OsgTools/Animate/Date.h"

namespace boost { namespace serialization { class access; } }

#include <map>

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT PointTimeLayer : public Minerva::Core::Layers::Layer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  enum AnimationType
  {
    EVENT,
    ATTRIBUTE
  };

  PointTimeLayer();

  /// Get/Set First date column name.
  void                    firstDateColumn( const std::string& );
  const std::string&      firstDateColumn() const;

  /// Get/Set last date column name.
  void                    lastDateColumn( const std::string& );
  const std::string&      lastDateColumn() const;

  /// Get/Set primitive id.
  void                    primitiveID( unsigned int );
  unsigned int            primitiveID() const;

  /// Get/Set the size.
  void                    size( float );
  float                   size() const;

  /// Get/Set the animation type.
  void                    animationType ( AnimationType type );
  AnimationType           animationType () const;

  /// Get/Set the number of timesteps.  Only valid on attribute animations.
  void                    numberOfTimesteps( unsigned int );
  unsigned int            numberOfTimesteps() const;

  /// Build the scene.
  virtual void            buildScene( osg::Group* parent );

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Modify the data objects.
  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Is this layer temporal.
  virtual bool            isTemporal() const;

  /// Set data members from given layer.
  virtual void            setDataMembers ( Layer * );

protected:
  virtual ~PointTimeLayer();

  void                    _updateMinMax ( const OsgTools::Animate::Date& min, const OsgTools::Animate::Date& max );

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );
 
  unsigned int _primitiveID;
  float _size;
  std::string _firstDateColumn;
  std::string _lastDateColumn;
  
  OsgTools::Animate::Date _minDate;
  OsgTools::Animate::Date _maxDate;

};


template < class Archive >
void PointTimeLayer::serialize( Archive &ar, const unsigned int version )
{
  ar & boost::serialization::make_nvp( "BaseLayer", boost::serialization::base_object< BaseClass >(*this) );
  ar & boost::serialization::make_nvp( "PrimitiveID", _primitiveID );
  ar & boost::serialization::make_nvp( "Size", _size );
  ar & boost::serialization::make_nvp( "FirstDateColumn", _firstDateColumn );
  ar & boost::serialization::make_nvp( "LastDateColumn", _lastDateColumn );
}


}
}
}

#endif // __WNV_LAYERS_POINT_LAYER_H__
