
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewRightCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewRightCommand(object caller)
      : base(caller)
    {
      _text = "View &Right";
      _toolTipText = "View Right";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_right_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.RIGHT;
    }
  }
}
