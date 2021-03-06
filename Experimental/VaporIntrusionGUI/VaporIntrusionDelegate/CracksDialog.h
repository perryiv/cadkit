
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VAPORINTRUSION_CRACKS_DIALOG_H__
#define __VAPORINTRUSION_CRACKS_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_CracksDialog.h"

#include "QtGui/QDialog"


class CracksDialog : public QDialog,
                     private Ui::CracksDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;

  // Useful typedefs
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Crack Crack;
  typedef IVPI::Cracks Cracks;
  typedef IVPI::CracksPair CracksPair;

  CracksDialog ( QWidget *parent = 0x0 );
  virtual ~CracksDialog();

  CracksPair cracks();
  void     cracks( CracksPair c );

  void     initialize();


protected:
	void	_xInitialize();
	void	_xClearTable();
  void	_yInitialize();
	void	_yClearTable();

private:
  
	CracksPair _cracks;


private slots:
	void on_xAddButton_clicked();
	void on_xRemoveButton_clicked();
	void on_xUpdateButton_clicked();
  void on_yAddButton_clicked();
	void on_yRemoveButton_clicked();
	void on_yUpdateButton_clicked();
};



#endif // __VAPORINTRUSION_CRACKS_DIALOG_H__
