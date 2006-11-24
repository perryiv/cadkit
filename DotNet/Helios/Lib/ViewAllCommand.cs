
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewAllCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewAllCommand(object caller)
      : base(caller)
    {
      _text = "View &All";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_all_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.FIT;
    }
  }
}
