
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to represent line data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECTS_LINE_H__
#define __DATA_OBJECTS_LINE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DataObjects/DataObject.h"

#include "osg/ref_ptr"
#include "osg/Geode"

namespace Minerva {
namespace Core {
namespace DataObjects {

class MINERVA_EXPORT Line : public DataObject
{
public:
  typedef Minerva::Core::DataObjects::DataObject BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Line );

  Line();

  /// Get/Set the width
  float                 width() const;
  void                  width( float );

  /// Build the scene branch for the data object.
  virtual osg::Node*    buildScene();

protected:
  /// Use reference counting.
  virtual ~Line();

private:
  float     _width;
  osg::ref_ptr< osg::Geode > _node;
};

}
}
}

#endif // __DATA_OBJECTS_LINE_H__
