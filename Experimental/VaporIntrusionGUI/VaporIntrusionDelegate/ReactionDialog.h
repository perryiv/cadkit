
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

#ifndef __VAPORINTRUSION_REACTION_DIALOG_H__
#define __VAPORINTRUSION_REACTION_DIALOG_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector3.h"

#include "ui_ReactionDialog.h"

#include "QtGui/QDialog"
#include "QtGui/QComboBox"


class ReactionDialog : public QDialog,
                       private Ui::ReactionDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;
  typedef IVPI::Reaction  Reaction;
  typedef IVPI::Reactions Reactions;
  // Useful typedefs  

  ReactionDialog ( QWidget *parent = 0x0 );
  virtual ~ReactionDialog();

  void                initialize();
  Reactions           reactions();
  void                reactions( Reactions c );

	void								finalize();


protected:

  void                _initialize();
  void                _clearTable();

private:
  Reactions						_reactions;

private slots:

};



#endif // __VAPORINTRUSION_REACTION_DIALOG_H__
