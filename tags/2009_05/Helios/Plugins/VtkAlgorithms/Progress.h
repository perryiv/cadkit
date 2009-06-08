
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTK_TOOLS_COMMANDS_PROGRESS_H__
#define __VTK_TOOLS_COMMANDS_PROGRESS_H__

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "vtkCommand.h"

namespace VTKTools {

class Progress : public vtkCommand
{
public:

  Progress();

  void progressBar ( Usul::Interfaces::IProgressBar * );
  
  /// vtkCommand::Execute
  virtual void Execute ( vtkObject *caller, unsigned long eventId, void *callData );

protected:

  virtual ~Progress();

private:
  Usul::Interfaces::IProgressBar::RefPtr _progress;
};

}

#endif // __VTK_TOOLS_COMMANDS_PROGRESS_H__
