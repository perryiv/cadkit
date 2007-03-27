
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewerModeCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.ViewMode _mode = 0;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected ViewerModeCommand(object caller)
      : base()
    {
      _caller = caller;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~ViewerModeCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1408874650: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IViewerMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IViewerMode;
      if (null != mode)
      {
        mode.Mode = this.Mode;
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IViewerMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IViewerMode;
      return (null != mode);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.IViewerMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IViewerMode;
      return ((null != mode) && (mode.Mode == this.Mode));
    }


    /// <summary>
    /// Return viewer mode.
    /// </summary>
    private CadKit.Interfaces.ViewMode Mode
    {
      get { using (this.Lock.read()) { return _mode; } }
    }
  }
}
