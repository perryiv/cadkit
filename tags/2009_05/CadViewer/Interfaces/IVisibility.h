
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for hiding and showing nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_NODE_VISIBILITY_H_
#define _CV_INTERFACE_NODE_VISIBILITY_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; };


namespace CV {
namespace Interfaces {


struct IVisibility : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVisibility );

  /// Id for this interface.
  enum { IID = 1075830382 };

  /// Hide the node.
  virtual void            hide ( osg::Node * ) = 0;

  /// Show the node.
  virtual void            show ( osg::Node * ) = 0;

  /// Is the node visible?
  virtual bool            isVisible ( const osg::Node * ) const = 0;
};


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_NODE_VISIBILITY_H_
