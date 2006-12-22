
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class RenderLoopCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public RenderLoopCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Render Loop";
      _toolTipText = _text;
      _menuIcon = null;
      _toolIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/render_loop_command.png");
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~RenderLoopCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4266305360: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
      if (null != renderLoop)
      {
        renderLoop.UseRenderLoop = !renderLoop.UseRenderLoop;
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
      return (null != renderLoop);
    }


    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;
      return (null == renderLoop) ? false : renderLoop.UseRenderLoop;
    }
  }
}
