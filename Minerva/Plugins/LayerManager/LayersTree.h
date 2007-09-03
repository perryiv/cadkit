
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_PLUGINS_LAYERS_TREE_H__
#define __MINERVA_PLUGINS_LAYERS_TREE_H__

#include "QtGui/QTreeWidget"

class LayersTree : public QTreeWidget
{
public:
  typedef QTreeWidget BaseClass;

  LayersTree ( QWidget *parent = 0x0 );
  virtual ~LayersTree ();
};


#endif // __MINERVA_PLUGINS_LAYERS_TREE_H__
