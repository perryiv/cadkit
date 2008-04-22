
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class TextureModeCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.TextureMode.Mode _mode = CadKit.Interfaces.TextureMode.Mode.D2;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected TextureModeCommand(object caller)
      : base()
    {
      _caller = caller;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~TextureModeCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2861832582: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.ITextureMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ITextureMode;
      if (null != mode)
      {
        bool state = mode.mode(this.Mode);
        mode.mode(this.Mode, !state);
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.ITextureMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ITextureMode;
      return (null != mode);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.ITextureMode mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ITextureMode;
      return ((null != mode) && (true == mode.mode(this.Mode)));
    }


    /// <summary>
    /// Return texture environment mode.
    /// </summary>
    private CadKit.Interfaces.TextureMode.Mode Mode
    {
      get { using (this.Lock.read()) { return _mode; } }
    }
  }
}
