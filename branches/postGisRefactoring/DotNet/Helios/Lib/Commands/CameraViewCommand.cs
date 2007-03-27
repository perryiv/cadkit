
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
    protected CameraViewCommand(object caller)
      : base()
    {
      _caller = caller;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~CameraViewCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2302901667: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.ICamera camera = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ICamera;
      if (null != camera)
      {
        camera.camera(this.Option);
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.ICamera camera = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ICamera;
      return (null != camera);
    }


    /// <summary>
    /// Return viewer mode.
    /// </summary>
    private CadKit.Interfaces.CameraOption Option
    {
      get { using (this.Lock.read()) { return _option; } }
    }
  }
}
