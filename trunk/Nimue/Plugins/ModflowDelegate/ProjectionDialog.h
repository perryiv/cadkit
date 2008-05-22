
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for projection system.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_PROJECTION_DIALOG_H__
#define __MODFLOW_PROJECTION_DIALOG_H__

#include "ModflowDelegate/ui_ProjectionDialog.h"

#include "QtGui/QDialog"

#include <map>

class QTreeWidgetItem;
class OGRSpatialReference;

namespace Modflow {

class ProjectionDialog : public QDialog,
                         private Ui::ProjectionDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  ProjectionDialog ( QWidget *parent = 0x0 );
  virtual ~ProjectionDialog();

  // Get the selected coordinate system.
  OGRSpatialReference* coordinateSystem() const;

private:
  typedef std::map<QTreeWidgetItem*, OGRSpatialReference*> CoordinateSystems;

  void _buildTree();
  void _addItem ( QTreeWidgetItem *parent, const std::string& name, OGRSpatialReference* );

  CoordinateSystems _coordinateSystems;
};


} // namespace Modflow

#endif //__MODFLOW_PROJECTION_DIALOG_H__
