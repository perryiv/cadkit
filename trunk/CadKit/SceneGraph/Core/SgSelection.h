
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
    MOUSE_CLICK_DOUBLE_LEFT   = 0x00000001,
    MOUSE_CLICK_DOUBLE_MIDDLE = 0x00000002,
    MOUSE_CLICK_DOUBLE_RIGHT  = 0x00000004,
    MOUSE_CLICK_SINGLE_LEFT   = 0x00000008,
    MOUSE_CLICK_SINGLE_MIDDLE = 0x00000010,
    MOUSE_CLICK_SINGLE_RIGHT  = 0x00000020,
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

  SG_DECLARE_DYNAMIC_NODE ( SgSelection, 1033948673 );
  SL_DECLARE_BITMASK_FUNCTIONS ( SelectionPolicy, unsigned int, _selectionPolicy );
};
};

#endif
