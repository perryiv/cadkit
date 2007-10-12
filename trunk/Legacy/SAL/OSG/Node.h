
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Node class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_SAL_NODE_CLASS_H_
#define _OSG_SAL_NODE_CLASS_H_

#include "SAL/OSG/CompileGuard.h"
#include "SAL/Interfaces/INode.h"
#include "SAL/Interfaces/IOSG.h"
#include "SAL/Interfaces/IBoundingSphere.h"

#include "Usul/Base/Referenced.h"

namespace osg { class Referenced; class Node; };


namespace SAL {
namespace OSG {


class Node : public Usul::Base::Referenced,
             public SAL::Interfaces::INode,
             public SAL::Interfaces::IOSG,
             public SAL::Interfaces::IBoundingSphereFloat
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef SAL::Interfaces::INode INode;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Node );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Node ( osg::Node * );
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IOSG
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Get the pointer to the base class for all OSG objects.
  virtual const osg::Referenced *       osgReferenced() const;
  virtual osg::Referenced *             osgReferenced();

protected:

  typedef USUL_VALID_REF_POINTER ( osg::Referenced ) ValidOsgRefPtr;
  typedef USUL_VALID_REF_POINTER ( osg::Node ) ValidOsgNodePtr;

  /// No default construction.
  Node();

  /// Do not copy.
  Node ( const Node & );
  Node &operator = ( const Node & );

  /// Use reference counting.
  virtual ~Node();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::INode
  //
  /////////////////////////////////////////////////////////////////////////////

  // Clone the node.
  virtual INode *                       cloneNode();

  /// Get/set the name.
  virtual void                          getNodeName ( std::string &name ) const;
  virtual void                          setNodeName ( const std::string &name );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IBoundingSphereFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the center.
  virtual void                          boundingSphereCenter ( Usul::Math::Vec3f &center ) const;

  // Get the radius.
  virtual void                          boundingSphereRadius ( float &radius ) const;

private:

  ValidOsgNodePtr _node;
};


}; // namespace OSG
}; // namespace SAL


#endif // _OSG_SAL_NODE_CLASS_H_
