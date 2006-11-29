
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewTopCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewTopCommand(object caller)
      : base(caller)
    {
      _text = "View &Top";
      _toolTipText = "View Top";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_top_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.TOP;
    }
  }
}
