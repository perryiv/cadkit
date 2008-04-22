
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ViewHomeCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewHomeCommand(object caller)
      : base(caller)
    {
      _text = "View &Home";
      _toolTipText = "View Home";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_home_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.RESET;
    }
  }

  public class ViewFitCommand : CameraViewCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ViewFitCommand(object caller)
      : base(caller)
    {
      _text = "View &Fit";
      _toolTipText = "View All";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/view_all_command.gif");
      _toolIcon = _menuIcon;
      _option = CadKit.Interfaces.CameraOption.FIT;
    }
  }

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
