
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes for attribute wrappers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_

#include "Helios/Plugins/ModflowModel/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Trace/Trace.h"

#include "osg/Node"


namespace Modflow {
namespace Attributes {


///////////////////////////////////////////////////////////////////////////////
//
//  A simple attribute wrapper.
//
///////////////////////////////////////////////////////////////////////////////

class Attribute : public Usul::Base::Object,
                      public Usul::Interfaces::ITreeNode,
                      public Usul::Interfaces::IBooleanState
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef Usul::Interfaces::IBooleanState IBooleanState;
  typedef osg::ref_ptr < osg::Node > NodePtr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Attribute );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Attribute ( const std::string &name );

  // Set/get the scene.
  void                        scene ( osg::Node * );
  osg::Node *                 scene();

  // Clear the attribute.
  void                        clear();

  // Set/get the dirty flag.
  void                        dirty ( bool );
  bool                        dirty() const;

  // Set/get the visible flag.
  void                        visible ( bool );
  bool                        visible() const;

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int        getNumChildNodes() const;
  virtual const ITreeNode *   getChildNode ( unsigned int which ) const;
  virtual ITreeNode *         getChildNode ( unsigned int which );
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;

  // Usul::Interfaces::IBooleanState.
  virtual void                setBooleanState ( bool );
  virtual bool                getBooleanState() const;

protected:

  // Use reference counting.
  virtual ~Attribute();

private:

  // Do not copy.
  Attribute ( const Attribute & );
  Attribute &operator = ( const Attribute & );

  void                        _destroy();

  unsigned int _flags;
  NodePtr _scene;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_



#if 0

  Should this attribute class hold the visible flag? 

  That way it would be trivial to set/get it. 

  Then, simply rebuilding the scene means asking every attribute for its name 
  and looking (in a factory) for the proper scene-builder class (which do not 
  exist yet, but would work like the readers).

  This would also eliminate the need for the templated getters and setters.

  Maybe this class even holds the group node for the scene...

#endif
