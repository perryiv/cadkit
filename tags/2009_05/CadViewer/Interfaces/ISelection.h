
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for selecting and deselecting an object.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_OBJECT_SELECTION_H_
#define _CV_INTERFACE_OBJECT_SELECTION_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Object; };


namespace CV {
namespace Interfaces {


struct ISelection : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISelection );

  /// Id for this interface.
  enum { IID = 1075830381 };

  /// Select the object.
  virtual void            select ( osg::Object * ) = 0;

  /// Unselect the object.
  virtual void            unselect ( osg::Object * ) = 0;

  /// Is the object selected?
  virtual bool            isSelected ( const osg::Object * ) const = 0;
};


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_OBJECT_SELECTION_H_
