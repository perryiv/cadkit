
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_GROUP_H__
#define __USUL_INTERFACES_GROUP_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace osg { class Group; };

namespace Usul {
namespace Interfaces {

struct IGroup : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGroup );

  /// Id for this interface.
  enum { IID = 3425358715u };

  virtual osg::Group*  getGroup    ( const std::string& ) = 0;
  virtual void         removeGroup ( const std::string& ) = 0;
  virtual bool         hasGroup    ( const std::string& ) = 0;

}; 

}

}


#endif // __USUL_INTERFACES_GROUP_H__

