
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
//  SgSelection.cpp: A group node that allows selection of children.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgSelection.h"
#include "SgPath.h"
#include "SgMessage.h"
#include "SgMessageIds.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlAssert.h"
# include <string>
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgSelection,SgSeparator);

typedef SgMessage<SlUint64, std::string, const void *, const void *> SgInternalMessage;


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSelection::SgSelection() : SgSeparator(),
  _clientSelectionCB ( NULL, NULL, NULL ),
  _clientDeselectionCB ( NULL, NULL, NULL ),
  _selectionPolicy ( 0 ),
  _isSelected ( false )
{
  SL_PRINT2 ( "SgSelection::SgSelection(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSelection::~SgSelection()
{
  SL_PRINT2 ( "SgSelection::~SgSelection(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Tell this instance to call any client functions.
//
/////////////////////////////////////////////////////////////////////////////

void SgSelection::_selected ( const SgPath *path )
{
  SL_ASSERT ( this && path );

  if ( _clientSelectionCB.getFunction() )
  {
    (_clientSelectionCB.getFunction()) ( this, path, _clientSelectionCB.getClientData() );
  }

  _isSelected = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Tell this instance to call any client functions.
//
/////////////////////////////////////////////////////////////////////////////

void SgSelection::_deselect()
{
  SL_ASSERT ( this );

  if ( _clientDeselectionCB.getFunction() )
  {
    (_clientDeselectionCB.getFunction()) ( this, NULL, _clientDeselectionCB.getClientData() );
  }

  _isSelected = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  This gets called when a group finds a selection node. It is regestered
//  in SgSelection::deselectAll().
//
/////////////////////////////////////////////////////////////////////////////

void _selectionDeselectionCB ( SgInternalMessage &message )
{
  // Get the node.
  SgNode *node = message.getNode();

  // Should always be true.
  SL_ASSERT ( node && node->isOfType ( SgSelection::getClassType() ) );
  SL_ASSERT ( message.getId() == SG_NODE_FOUND );

  // Make it a selection.
  SgSelection *selection = (SgSelection *) node;

  // Deselect it.
//  selection->_deselect(); TODO
  SL_ASSERT ( 0 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Tell all selection nodes that they are deselected.
//
/////////////////////////////////////////////////////////////////////////////

void SgSelection::_deselectAll ( const SgNode *node )
{
  // Catch bugs.
  if ( !node ) 
  {
    SL_ASSERT ( 0 ); // Heads up.
    return;
  }

  // See if this node is a group.
  if ( !node->isOfType ( SgGroup::getClassType() ) ) 
    return;

  // Make it a group.
  SgGroup *group = (SgGroup *) node;

  // Need these variables below.
  const SlType *type = SgSelection::getClassType();
  bool completeTraversal = true;
  bool exactType = false;
  SgInternalMessage message;

  // Tell the group to call our function when it encounters a selection node.
  // (or one that inherits from selection). We want to traverse all the children.
//  group->findChild ( SgSelection::getClassType(), completeTraversal, exactType, _selectionDeselectionCB, message ); TODO
  SL_ASSERT ( 0 );

  // Lastly, see if the group is itself a selection.
  if ( !group->isOfType ( SgSelection::getClassType() ) ) 
    return;

  // Make it a selection.
  SgSelection *selection = (SgSelection *) group;

  // Deselect it.
  selection->_deselect();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
/////////////////////////////////////////////////////////////////////////////

void SgSelection::setSelectionCallback ( SgSelectionCB *callback, const void *data )
{
  SL_ASSERT ( this && callback );
  _clientSelectionCB.setValue ( callback, data, 0x0 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
/////////////////////////////////////////////////////////////////////////////

void SgSelection::setDeselectionCallback ( SgSelectionCB *callback, const void *data )
{
  SL_ASSERT ( this && callback );
  _clientDeselectionCB.setValue ( callback, data, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell any selection nodes in the path that they were selected.
//
///////////////////////////////////////////////////////////////////////////////

void SgSelection::selectSelectionNodes ( const SgSelection::Flags &reason, const SgPath *path )
{
  // Loop through all the nodes.
  const SgPath::Nodes *nodes = path->getNodes();
  for ( SgPath::Nodes::const_iterator i = nodes->begin(); i != nodes->end(); ++i )
  {
    // Get the current node.
    SgNode *node = i->getValue();

    // If the node is a selection...
    if ( node->isOfType ( SgSelection::getClassType() ) )
    {
      // Get a pointer to the selection.
      SgSelection *selection = static_cast<SgSelection *> ( node );

      // Only call it if the selection policy matches the reason.
      if ( selection->hasSelectionPolicy ( reason ) )
      {
        // Tell the selection node that it has been selected.
        selection->_selected ( path );
      }
    }
  }
}
