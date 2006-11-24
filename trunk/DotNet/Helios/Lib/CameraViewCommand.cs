
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class CameraViewCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.CameraOption _option = 0;

    /// <summary>
    /// Constructor.
    /// </summary>
    public CameraViewCommand(object caller)
    {
      _caller = caller;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      lock (_mutex)
      {
        CadKit.Interfaces.ICamera camera = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ICamera;
        if (null != camera)
        {
          camera.camera(_option);
        }
      }
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (_mutex)
      {
        CadKit.Interfaces.ICamera camera = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ICamera;
        return (null != camera);
      }
    }
  }
}
