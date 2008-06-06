
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ExportModelCommand : ExportSceneCommand
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public ExportModelCommand(object caller)
      : base(caller)
    {
      _text = "Export Model";
      _option = CadKit.Interfaces.SceneExport.Option.MODEL_ONLY;
    }
  }
}
