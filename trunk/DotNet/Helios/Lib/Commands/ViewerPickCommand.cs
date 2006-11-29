
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewerPickCommand : ViewerModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewerPickCommand(object caller)
      : base(caller)
    {
      _text = "Seek";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/viewer_pick_command.gif");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.ViewMode.PICK;
    }
  }
}
