
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
//  SgSelection.h: A group node that allows selection of children.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SELECTION_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SELECTION_H_

#include "SgSeparator.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlCallback.h"
#endif


namespace CadKit
{
class SgSelection;
class SgPath;

typedef void SgSelectionCB ( SgSelection *selection, const SgPath *path, const void *clientData );

class SG_API SgSelection : public SgSeparator
{
public:

  // Possible selection policies.
  enum
  {
    MOUSE_CLICK_DOUBLE_LEFT   = (1L<<0),
    MOUSE_CLICK_DOUBLE_MIDDLE = (1L<<1),
    MOUSE_CLICK_DOUBLE_RIGHT  = (1L<<2),
    MOUSE_CLICK_SINGLE_LEFT   = (1L<<3),
    MOUSE_CLICK_SINGLE_MIDDLE = (1L<<4),
    MOUSE_CLICK_SINGLE_RIGHT  = (1L<<5),
  };

  SgSelection();

  bool              isSelected() const { return _isSelected; }

  const void *      getClientSelectionData() const { return _clientSelectionCB.getClientData(); }
  const void *      getClientDeselectionData() const { return _clientDeselectionCB.getClientData(); }

  static void       selectSelectionNodes ( const unsigned int &reason, const SgPath *path );

  void              setSelectionCallback ( SgSelectionCB *callback, const void *data );
  void              setDeselectionCallback ( SgSelectionCB *callback, const void *data );

protected:

  typedef SlCallback<SgSelectionCB *, const void *, const void *> ClientCallback;
  ClientCallback _clientSelectionCB;
  ClientCallback _clientDeselectionCB;
  unsigned int _selectionPolicy;
  bool _isSelected;

  virtual ~SgSelection();

  void              _deselect();
  static void       _deselectAll ( const SgNode *node );
  void              _selected ( const SgPath *path );

  SG_DECLARE_DYNAMIC_NODE ( SgSelection, 0x00001029 );
  SL_DECLARE_BITMASK_FUNCTIONS ( SelectionPolicy, unsigned int, _selectionPolicy );
};
};

#endif
