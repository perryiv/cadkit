
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class TextureEnvironmentCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.TextureEnvironment.Mode _mode = CadKit.Interfaces.TextureEnvironment.Mode.NONE;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected TextureEnvironmentCommand(object caller)
      : base()
    {
      _caller = caller;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~TextureEnvironmentCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1914748809: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.ITextureEnvironment mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ITextureEnvironment;
      if (null != mode)
      {
        mode.Mode = (this.Mode == mode.Mode) ? CadKit.Interfaces.TextureEnvironment.Mode.NONE : this.Mode;
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.ITextureEnvironment mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ITextureEnvironment;
      return (null != mode);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.ITextureEnvironment mode = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.ITextureEnvironment;
      return ((null != mode) && (mode.Mode == this.Mode));
    }


    /// <summary>
    /// Return texture environment mode.
    /// </summary>
    private CadKit.Interfaces.TextureEnvironment.Mode Mode
    {
      get { using (this.Lock.read()) { return _mode; } }
    }
  }
}
