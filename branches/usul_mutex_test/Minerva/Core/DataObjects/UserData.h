
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

#include "osg/Referenced"

namespace Minerva {
namespace Core {
namespace DataObjects {

class DataObject;

struct MINERVA_EXPORT UserData : public osg::Referenced
{
  UserData ( DataObject * object ) : _do ( object ) { }
  DataObject *_do;
};

}
}
}

#endif // __DATAOBJECTS_USER_DATA_H__
