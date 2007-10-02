
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_APPLICATION_H_
#define _CADKIT_HELIOS_QT_CORE_APPLICATION_H_

#include "Helios/Qt/Core/Export.h"

#include "QtGui/QApplication"


namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT Application : public QApplication
{
public:

  // Useful typedefs.
  typedef QApplication BaseClass;

  // Constructor and destructor.
  Application ( int argc, char **argv );
  ~Application();

  // Notification of every event.
  virtual bool          notify ( QObject *, QEvent * );
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif // _CADKIT_HELIOS_QT_CORE_APPLICATION_H_
