
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_VIEWS_EDIT_BACKGROUND_H__
#define __CADKIT_HELIOS_QT_VIEWS_EDIT_BACKGROUND_H__

#include "Helios/Qt/Views/OSG/Export.h"
#include "OsgTools/Render/Viewer.h"

#include "QtGui/QDialog"

class QCheckBox;

namespace CadKit {
namespace Helios {
namespace Views {
namespace OSG {


class HELIOS_QT_VIEWS_OSG_EXPORT EditBackground : public QDialog
{
  Q_OBJECT;

public:

  // Useful typedefs.
  typedef QDialog BaseClass;
  typedef OsgTools::Render::Viewer::Corners Corners;

  EditBackground ( OsgTools::Render::Viewer* viewer, QWidget* parent = 0x0 );
  virtual ~EditBackground();

protected:

  QWidget* _initChecks();

protected slots:

  void _bottomLeftChanged ( int state );
  void _bottomRightChanged ( int state );

  void _colorChanged ( float r, float g, float b, float a );

  void _topLeftChanged ( int state );
  void _topRightChanged ( int state );

private:

  OsgTools::Render::Viewer::RefPtr _viewer;
  QCheckBox *_topLeft;
  QCheckBox *_topRight;
  QCheckBox *_bottomLeft;
  QCheckBox *_bottomRight;
};


}
}
}
}


#endif // __CADKIT_HELIOS_QT_VIEWS_EDIT_BACKGROUND_H__
