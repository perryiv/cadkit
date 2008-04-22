
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ShadeModelSmoothCommand : ShadeModelCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ShadeModelSmoothCommand(object caller)
      : base(caller)
    {
      _text = "Smooth";
      _toolTipText = "Draw polygons with smooth shading";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/shade_model_smooth_command.gif");
      _toolIcon = _menuIcon;
      _model = CadKit.Interfaces.ShadeModel.Model.SMOOTH;
    }
  }


  public class ShadeModelFlatCommand : ShadeModelCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public ShadeModelFlatCommand(object caller)
      : base(caller)
    {
      _text = "Flat";
      _toolTipText = "Draw polygons with flat shading";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/shade_model_flat_command.gif");
      _toolIcon = _menuIcon;
      _model = CadKit.Interfaces.ShadeModel.Model.FLAT;
    }
  }
}
