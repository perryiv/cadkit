
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class PolygonsFillCommand : PolygonModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public PolygonsFillCommand(object caller)
      : base(caller)
    {
      _text = "Filled";
      _toolTipText = "Draw polygons filled";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/polygons_fill_command.png");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.PolygonMode.Mode.FILL;
      _face = CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK;
    }
  }


  public class PolygonsLineCommand : PolygonModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public PolygonsLineCommand(object caller)
      : base(caller)
    {
      _text = "Lines";
      _toolTipText = "Draw polygons as wireframe lines";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/polygons_lines_command.png");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.PolygonMode.Mode.LINE;
      _face = CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK;
    }
  }


  public class PolygonsPointsCommand : PolygonModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public PolygonsPointsCommand(object caller)
      : base(caller)
    {
      _text = "Points";
      _toolTipText = "Draw polygons as points";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/polygons_points_command.png");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.PolygonMode.Mode.POINT;
      _face = CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK;
    }
  }
}
