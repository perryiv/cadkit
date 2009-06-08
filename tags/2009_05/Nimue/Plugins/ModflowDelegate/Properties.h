
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Dialog for most modflow properties.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODFLOW_PROPERTIES_WIDGET_H__
#define __MODFLOW_PROPERTIES_WIDGET_H__

#include "ModflowDelegate/ui_Properties.h"

#include "ModflowModel/Interfaces/IModflowModel.h"

#include "QtGui/QWidget"


namespace Modflow {


class Properties : public QWidget,
                   private Ui::Properties
{
  Q_OBJECT;

public:

  typedef QWidget BaseClass;
  typedef Modflow::Interfaces::IModflowModel IModflowModel;

  Properties ( QWidget *parent = 0x0 );
  virtual ~Properties();

  // Set the document.
  void                        document ( Usul::Interfaces::IUnknown * );

protected slots:

  // Slots for offset.
  void                        _verticalScaleChanged ( double value );
  void                        _cellMarginChanged();

private:

  void                        _slotsConnect();
  void                        _slotsDisconnect();

  IModflowModel::QueryPtr _document;
};


} // namespace Modflow


#endif // __MODFLOW_PROPERTIES_WIDGET_H__
