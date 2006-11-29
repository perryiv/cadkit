
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewFrontCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewFrontCommand(object caller)
      : base(caller)
    {
      _text = "View &Front";
      _toolTipText = "View Front";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_front_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.FRONT;
    }
  }
}
