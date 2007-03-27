
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class FrameDumpCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public FrameDumpCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Frame Dump";
      _toolTipText = _text;
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/frame_dump_command.gif");
      _toolIcon = _menuIcon;
      CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~FrameDumpCommand()
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveViewChanged -= this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3772175117: {0}", e.Message);
      }
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IFrameDump frameDump = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IFrameDump;
      if (null != frameDump)
      {
        if (frameDump.DumpFrames)
        {
          frameDump.DumpFrames = false;
        }
        else
        {
          CadKit.Helios.Lib.FrameDump frameDumpDialog = new CadKit.Helios.Lib.FrameDump();
          if (frameDumpDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
          {
            frameDump.Directory = frameDumpDialog.Directory;
            frameDump.Extension = frameDumpDialog.Extension;
            frameDump.BaseFilename = frameDumpDialog.Filename;
            frameDump.DumpFrames = true;
          }
        }
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IFrameDump frameDump = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IFrameDump;
      return (null != frameDump);
    }

    /// <summary>
    /// Determine if the button is checked.
    /// </summary>
    protected override bool _isChecked()
    {
      CadKit.Interfaces.IFrameDump frameDump = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IFrameDump;
      return ((null != frameDump) && (frameDump.DumpFrames));
    }
  }
}
