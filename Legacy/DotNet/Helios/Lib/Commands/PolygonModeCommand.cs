
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class PolygonModeCommand : 
    CadKit.Commands.Command,
    System.IDisposable
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.PolygonMode.Face _face = CadKit.Interfaces.PolygonMode.Face.FRONT | CadKit.Interfaces.PolygonMode.Face.BACK;
    protected CadKit.Interfaces.PolygonMode.Mode _mode = CadKit.Interfaces.PolygonMode.Mode.FILLED;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected PolygonModeCommand(object caller)
      : base()
    {
      _caller = caller;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~PolygonModeCommand()
    {
      this._localCleanup();
    }


    /// <summary>
    /// Called by the system to clean this instance.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      this._localCleanup();
    }


    /// <summary>
    /// Clean this instance.
    /// </summary>
    private void _localCleanup()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1737229553: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IPolygonMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IPolygonMode;
      if (null != mode)
      {
        mode.Face = this.Face;
        mode.Mode = (this.Mode == mode.Mode) ? CadKit.Interfaces.PolygonMode.Mode.NONE : this.Mode;
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IPolygonMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IPolygonMode;
      return (null != mode);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.IPolygonMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IPolygonMode;
      return ((null != mode) && (mode.Mode == this.Mode));
    }


    /// <summary>
    /// Return polygon-mode face.
    /// </summary>
    private CadKit.Interfaces.PolygonMode.Face Face
    {
      get { using (this.Lock.read()) { return _face; } }
    }


    /// <summary>
    /// Return polygon-mode mode.
    /// </summary>
    private CadKit.Interfaces.PolygonMode.Mode Mode
    {
      get { using (this.Lock.read()) { return _mode; } }
    }
  }
}
