
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
#include "osg/Group"

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

  /// Accept the visitor.
  virtual void          accept ( Minerva::Core::Visitor& visitor );

  /// Get/Set the width
  float                 width() const;
  void                  width( float );

protected:
  /// Use reference counting.
  virtual ~Line();

  /// Build the scene branch for the data object.
  virtual osg::Node*    _preBuildScene( Usul::Interfaces::IUnknown* caller = 0x0 );

private:
  float     _width;
};

}
}
}

#endif // __DATA_OBJECTS_LINE_H__
