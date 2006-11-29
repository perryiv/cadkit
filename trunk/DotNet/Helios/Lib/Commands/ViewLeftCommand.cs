
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewLeftCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewLeftCommand(object caller)
      : base(caller)
    {
      _text = "View &Left";
      _toolTipText = "View Left";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_left_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.LEFT;
    }
  }
}
