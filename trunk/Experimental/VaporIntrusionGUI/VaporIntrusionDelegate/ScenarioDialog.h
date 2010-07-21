
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

#ifndef __VAPORINTRUSION_SCENARIO_DIALOG_H__
#define __VAPORINTRUSION_SCENARIO_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_ScenarioDialog.h"

#include "QtGui/QDialog"
#include "QtGui/QComboBox"


class ScenarioDialog : public QDialog,
                       private Ui::ScenarioDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
	typedef IVPI::VPIScenarioEntry  ScenarioEntry;
  typedef IVPI::VPIScenario  Scenario;
  typedef IVPI::VPIScenario Scenario;
  // Useful typedefs  

  ScenarioDialog ( QWidget *parent = 0x0 );
  virtual ~ScenarioDialog();

  void                initialize();
  Scenario            scenario();
  void                scenario( Scenario s );

	void								finalize();


protected:

  void                _initialize();
  void                _clearTable();

private:
  Scenario						_scenario;

private slots:

};



#endif // __VAPORINTRUSION_SCENARIO_DIALOG_H__
