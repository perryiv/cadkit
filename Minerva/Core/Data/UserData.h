
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATAOBJECTS_USER_DATA_H__
#define __DATAOBJECTS_USER_DATA_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/DataObject.h"

#include "osg/Referenced"

namespace Minerva {
namespace Core {
namespace Data {

struct MINERVA_EXPORT UserData : public osg::Referenced
{
  UserData ( const DataObject::ObjectID & id ) : 
    _id ( id ) 
  {
  }
  
  DataObject::ObjectID objectID() const
  {
    return _id;
  }

private:

  DataObject::ObjectID _id;
};

}
}
}

#endif // __DATAOBJECTS_USER_DATA_H__
