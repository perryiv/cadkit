
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_ABSTRACT_VIEW_H__
#define __MINERVA_CORE_DATA_ABSTRACT_VIEW_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/Object.h"

#include "Usul/Math/Matrix44.h"

namespace Minerva { namespace Core { namespace TileEngine { class LandModel; } } }

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT AbstractView : public Minerva::Core::Data::Object
{
public:
  typedef Minerva::Core::Data::Object BaseClass;
  typedef Usul::Math::Matrix44d Matrix;

  USUL_DECLARE_REF_POINTERS ( AbstractView );

  AbstractView();

  virtual Matrix viewMatrix ( Minerva::Core::TileEngine::LandModel* ) const = 0;

protected:

  virtual ~AbstractView();

};

  
}
}
}


#endif // __MINERVA_CORE_DATA_ABSTRACT_VIEW_H__
