
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewBottomCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewBottomCommand(object caller)
      : base(caller)
    {
      _text = "View &Bottom";
      _toolTipText = "View Bottom";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_bottom_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.BOTTOM;
    }
  }
}
