
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
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/polygons_filled_command.png");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.PolygonMode.Mode.FILLED;
      _face = CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK;
    }
  }


  public class PolygonsHiddenLinesCommand : PolygonModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public PolygonsHiddenLinesCommand(object caller)
      : base(caller)
    {
      _text = "Hidden Lines";
      _toolTipText = "Draw polygons as hidden lines";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/polygons_hidden_lines_command.png");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.PolygonMode.Mode.HIDDEN_LINES;
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
      _text = "Wireframe";
      _toolTipText = "Draw polygons as wireframe lines";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/polygons_wire_frame_command.png");
      _toolIcon = _menuIcon;
      _mode = CadKit.Interfaces.PolygonMode.Mode.WIRE_FRAME;
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
      _mode = CadKit.Interfaces.PolygonMode.Mode.POINTS;
      _face = CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK;
    }
  }
}
