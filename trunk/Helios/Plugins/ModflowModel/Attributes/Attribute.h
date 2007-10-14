
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for attributes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_

#include "Helios/Plugins/ModflowModel/CompileGuard.h"

#include "Usul/Base/Observed.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Pointers/WeakPointer.h"
#include "Usul/Properties/Attribute.h"

#include "osg/Group"

#include <list>

namespace Modflow { namespace Model { class Layer; } }


namespace Modflow {
namespace Attributes {


class Attribute : public Usul::Base::Observed,
                  public Usul::Interfaces::ITreeNode,
                  public Usul::Interfaces::IBooleanState,
                  public Usul::Interfaces::IDirtyState
{
public:

  // Type information.
  USUL_DECLARE_TYPE_ID ( Attribute );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Attribute );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Usul::Base::Observed BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef osg::ref_ptr < osg::Group > GroupPtr;
  typedef Usul::Pointers::WeakPointer < Attribute > WeakPtr;
  typedef Usul::Properties::Attribute < WeakPtr, osg::Referenced > UserData;
  typedef std::pair < std::string, std::string > StringPair;
  typedef std::list < StringPair > StringData;

  // Construction.
  Attribute ( const std::string &name, IUnknown *parent );

  // Build the scene.
  virtual osg::Group *        buildScene ( Modflow::Model::Layer * );

  // Clear the attribute.
  void                        clear();

  // Set/get the dirty flag.
  virtual void                dirtyState ( bool );
  virtual bool                dirtyState() const;

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

  // Append internal state to list.
  virtual void                appendStringData ( StringData & ) const;

protected:

  // Use reference counting.
  virtual ~Attribute();

  void                        _setScene ( osg::Group * );

private:

  // Do not copy.
  Attribute ( const Attribute & );
  Attribute &operator = ( const Attribute & );

  void                        _destroy();

  unsigned int _flags;
  GroupPtr _scene;
  IUnknown *_parent;
};


} // namespace Attributes
} // namespace Modflow


#endif // _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
