
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

#ifndef _PERFORMER_SAL_NODE_CLASS_H_
#define _PERFORMER_SAL_NODE_CLASS_H_

#include "SAL/Performer/CompileGuard.h"
#include "SAL/Interfaces/INode.h"
#include "SAL/Interfaces/IPerformer.h"
#include "SAL/Interfaces/IBoundingSphere.h"

#include "Usul/Base/Referenced.h"

class pfMemory;
class pfNode;


namespace SAL {
namespace Performer {


class Node : public Usul::Base::Referenced,
             public SAL::Interfaces::INode,
             public SAL::Interfaces::IPerformer,
             public SAL::Interfaces::IBoundingSphereFloat
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Node );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Node ( ::pfNode * );

  // Get the pfNode from the IUnknown.
  static ::pfNode *             getNode ( Usul::Interfaces::IUnknown * );

protected:

  typedef USUL_VALID_REF_POINTER ( ::pfMemory ) ValidPfMemoryPtr;
  typedef USUL_VALID_REF_POINTER ( ::pfNode ) ValidPfNodePtr;

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
  virtual INode *               cloneNode();

  /// Get/set the name.
  virtual void                  getNodeName ( std::string &name ) const;
  virtual void                  setNodeName ( const std::string &name );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IPerformer
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Get the pointer to the base class for all Performer objects.
  virtual const ::pfMemory *    getPfMemory() const;
  virtual ::pfMemory *          getPfMemory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IBoundingSphereFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the center.
  virtual void                  boundingSphereCenter ( Usul::Math::Vec3f &center ) const;

  // Get the radius.
  virtual void                  boundingSphereRadius ( float &radius ) const;

private:

  ValidPfNodePtr _node;
};


}; // namespace Performer
}; // namespace SAL


#endif // _PERFORMER_SAL_NODE_CLASS_H_
