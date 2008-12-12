
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

#include "Minerva/Qt/Widgets/Export.h"

#include "Minerva/Core/Extents.h"

#include "XmlTree/Node.h"

#include "QtGui/QTreeWidgetItem"

#include "osg/Vec2d"

#include <vector>
#include <string>

namespace Minerva {
namespace Widgets {

      
class MINERVA_QT_WIDGETS_EXPORT WmsLayerItem : public QTreeWidgetItem
{
public:
  typedef QTreeWidgetItem BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  
  WmsLayerItem ( const std::string& name, const std::string& title, const Extents& extents, QTreeWidget *parent = 0x0 );
  virtual ~WmsLayerItem();
  
  /// Get the extents.
  Extents         extents () const;
  
  /// Get the name.
  std::string     name() const;

  /// Get the style.
  std::string     style() const;

  /// Get the title.
  std::string     title() const;
  
private:
  Extents _extents;
};


}
}
  
#endif //__MINERVA_PLUGINS_WMS_LAYER_ITEM_H__
