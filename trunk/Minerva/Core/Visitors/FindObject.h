
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VISITORS_FIND_OBJECT_H__
#define __MINERVA_VISITORS_FIND_OBJECT_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Visitor.h"
#include "Minerva/Core/Data/Container.h"

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Core {
namespace Visitors {

class MINERVA_EXPORT FindObject : public Minerva::Core::Visitor
{
public:
  typedef Minerva::Core::Visitor BaseClass;
  typedef Minerva::Core::Data::Container::ObjectID ObjectID;
  typedef Usul::Interfaces::IUnknown IUnknown;
  
  USUL_DECLARE_REF_POINTERS ( FindObject );
  
  FindObject ( const ObjectID& objectID );
  
  virtual void    visit ( Minerva::Core::Data::Container &container );
  
  /// Get the unknown (may be null).
  IUnknown::RefPtr object() const;
  
protected:
  
  /// Use reference counting.
  virtual ~FindObject();
  
private:
  
  ObjectID _objectID;
  IUnknown::RefPtr _unknown;
};


}
}
}


#endif // __MINERVA_VISITORS_FIND_OBJECT_H__
