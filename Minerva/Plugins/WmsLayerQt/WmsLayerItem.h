
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_PLUGINS_WMS_LAYER_ITEM_H__
#define __MINERVA_PLUGINS_WMS_LAYER_ITEM_H__

#include "Minerva/Core/Extents.h"

#include "XmlTree/Node.h"

#include "QtGui/QTreeWidgetItem"

#include "osg/Vec2d"

#include <vector>
#include <string>

class WmsLayerItem : public QTreeWidgetItem
{
public:
  typedef QTreeWidgetItem BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  
  WmsLayerItem ( XmlTree::Node* node, const Extents& defaultExtents, QTreeWidget *parent = 0x0 );
  virtual ~WmsLayerItem();
  
  /// Get the extents.
  Extents         extents () const;
  
  /// Parse the extents
  static Extents  parseExtents ( const XmlTree::Node & node, const Extents& defaultExtents = Extents ( -180, -90, 180, 90 )  );
  
  /// Get the name.
  std::string     name() const;
  
  /// Get the style.
  std::string     style() const;
  
private:
  XmlTree::Node::RefPtr _node;
  Extents _extents;
};

#endif //__MINERVA_PLUGINS_WMS_LAYER_ITEM_H__
