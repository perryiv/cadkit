
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewBackCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewBackCommand(object caller)
      : base(caller)
    {
      _text = "View &Back";
      _toolTipText = "View Back";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_back_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.BACK;
    }
  }
}
