
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewerNavigateCommand : ViewerModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewerNavigateCommand(object caller)
      : base(caller)
    {
      _text = "Navigate";
      _toolTipText = _text;
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/viewer_navigate_command.gif");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.ViewMode.NAVIGATE;
    }
  }

  public class ViewerPickCommand : ViewerModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewerPickCommand(object caller)
      : base(caller)
    {
      _text = "Pick";
      _toolTipText = _text;
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/viewer_pick_command.gif");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.ViewMode.PICK;
    }
  }

  public class ViewerSeekCommand : ViewerModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewerSeekCommand(object caller)
      : base(caller)
    {
      _text = "Seek";
      _toolTipText = _text;
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/viewer_seek_command.gif");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.ViewMode.SEEK;
    }
  }
}
