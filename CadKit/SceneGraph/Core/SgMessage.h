
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
