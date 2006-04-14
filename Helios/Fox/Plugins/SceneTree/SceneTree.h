


#ifndef __SCENE_TREE_H__
#define __SCENE_TREE_H__

#include "FoxTools/Headers/TreeList.h"
#include <map>

namespace FX { class FXTreeList; class FXTreeItem; }
namespace osg { class Drawable; class Node; }

class SceneTree : public FX::FXTreeList
{
public:
  typedef FX::FXTreeList BaseClass;
  typedef std::map < FX::FXTreeItem*, osg::Node* > Nodes;
  typedef std::map < FX::FXTreeItem*, osg::Drawable* > Drawables;

  enum
  {
    ID_TREE_LIST = BaseClass::ID_LAST,
    ID_PROPERTIES,
    ID_SHOW_BOUNDING_BOX,
    ID_LAST
  };

  SceneTree (FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=TREELIST_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  void update ( osg::Node* );

  long onCommandRightClick ( FX::FXObject*, FX::FXSelector, void *data );
  long onCommandLeftClick ( FX::FXObject*, FX::FXSelector, void *data );
  long onCommandProperties ( FX::FXObject*, FX::FXSelector, void *data );

protected:
  void _buildNodeTree  ( osg::Node *node, FX::FXTreeItem *parent );
  void _buildSceneTree ( osg::Node *node, FX::FXTreeItem *parent );
  FX::FXTreeItem *_addTreeItem ( osg::Node *node, FX::FXTreeItem *parent );
  FX::FXTreeItem *_addTreeItem ( osg::Drawable *drawable, FX::FXTreeItem *parent );

private:
  SceneTree () { } //For FOX

  Nodes _nodes;
  Drawables _drawables;

  FX::FXTreeItem *_selectedItem;

  FXDECLARE( SceneTree )
};


#endif // __SCENE_TREE_H__
