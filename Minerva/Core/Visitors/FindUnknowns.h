
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_VISITORS_FIND_UNKNOWNS_H__
#define __MINERVA_CORE_VISITORS_FIND_UNKNOWNS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Math/MinMax.h"

#include "osg/Vec2d"

#include <vector>

namespace Minerva {
namespace Core {
namespace Visitors {


template <class UnknownType>
class MINERVA_EXPORT FindUnknowns : public Minerva::Core::Visitor
{
public:
  
  typedef Minerva::Core::Visitor BaseClass;
  typedef typename UnknownType::QueryPtr UnknownTypePtr;
  typedef std::vector<UnknownTypePtr> ContainerType;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  
  USUL_DECLARE_REF_POINTERS ( FindUnknowns );
  
  FindUnknowns ( const Extents& extents, ContainerType& container );
  
  virtual void visit ( Feature &feature );
  virtual void visit ( Minerva::Core::Layers::RasterGroup & );
  
protected:
  
  virtual ~FindUnknowns();
  
private:
  
  Extents _extents;
  ContainerType& _container;
  
};

#define ThisClass FindUnknowns<UnknownType>

template<class UnknownType>
ThisClass::FindUnknowns ( const Extents& extents, ContainerType& container ) : BaseClass(),
  _extents ( extents ),
  _container ( container )
{
}

template<class UnknownType>
ThisClass::~FindUnknowns()
{
}

template<class UnknownType>
void ThisClass::visit ( Feature &feature )
{
  typename UnknownType::QueryPtr unknown ( feature.queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  if ( unknown.valid() )
    _container.push_back ( unknown.get() );
}

template<class UnknownType>
void ThisClass::visit ( Minerva::Core::Layers::RasterGroup &group )
{
  if ( group.extents().intersects ( _extents ) )
  {
    // Reserve enough room.
    _container.reserve ( Usul::Math::maximum ( _container.capacity(), _container.size() ) + group.size() );
    
    // Traverse.
    group.traverse ( *this );
  }
}

#undef ThisClass
  
}
}
}

#endif // __MINERVA_CORE_VISITORS_FIND_UNKNOWNS_H__
