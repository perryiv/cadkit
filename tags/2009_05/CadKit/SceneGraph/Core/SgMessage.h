
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgMessage.h: Encapsulation of a message from the Sg library.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_MESSAGE_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_MESSAGE_H_

#include "SgGroup.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlMessage.h"
# include "Standard/SlPrint.h"
#endif


namespace CadKit
{
template<SL_MCA> class SgMessage : public SlMessage<SL_MFA>
{
public:

  SgMessage();
  SgMessage ( const SgMessage<SL_MFA> &message );

  const SgNode *            getNode() const { return _node.getValue(); }
  SgNode *                  getNode()       { return _node.getValue(); }

  const SgGroup *           getParent() const { return _parent.getValue(); }
  SgGroup *                 getParent()       { return _parent.getValue(); }

  SgMessage<SL_MFA> &       operator = ( const SgMessage<SL_MFA> &message );

  void                      setValue ( const SgMessage<SL_MFA> &message );
  void                      setParent ( SgGroup *parent ) { _parent = parent; }
  void                      setNode ( SgNode *node ) { _node = node; }

protected:

  SgNode::Ptr _node;
  SgGroup::Ptr _parent;
};


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SgMessage<SL_MFA>::SgMessage() : SlMessage<SL_MFA>(), 
  _node ( 0x0 ), 
  _parent ( 0x0 )
{
  SL_PRINT2 ( "SgMessage::SgMessage(), this = %X\n", this );
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SgMessage<SL_MFA>::SgMessage ( const SgMessage<SL_MFA> &message ) : SlMessage<SL_MFA> ( message ),
  _node ( message._node ),
  _parent ( message._parent )
{
  SL_PRINT2 ( "SgMessage::SgMessage(), this = %X\n", this );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline void SgMessage<SL_MFA>::setValue ( const SgMessage<SL_MFA> &message )
{
  // Call the base class's function.
  this->SlMessage<SL_MFA>::setValue ( message );

  // Set the data members of this class.
  _node = message._node;
  _parent = message._parent;
}


//////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SgMessage<SL_MFA> &SgMessage<SL_MFA>::operator = ( const SgMessage<SL_MFA> &message )
{
  this->setValue ( message );
  return *this;
}


}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_MESSAGE_H_
