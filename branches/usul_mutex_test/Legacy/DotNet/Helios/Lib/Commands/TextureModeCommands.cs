
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class TextureMode1DCommand : TextureModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureMode1DCommand(object caller)
      : base(caller)
    {
      _text = "1-D";
      _toolTipText = "Toggle use of 1-D textures";
      _mode = CadKit.Interfaces.TextureMode.Mode.D1;
    }
  }


  public class TextureMode2DCommand : TextureModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureMode2DCommand(object caller)
      : base(caller)
    {
      _text = "2-D";
      _toolTipText = "Toggle use of 1-D textures";
      _mode = CadKit.Interfaces.TextureMode.Mode.D2;
    }
  }


  public class TextureMode3DCommand : TextureModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureMode3DCommand(object caller)
      : base(caller)
    {
      _text = "3-D";
      _toolTipText = "Toggle use of 3-D textures";
      _mode = CadKit.Interfaces.TextureMode.Mode.D3;
    }
  }


  public class TextureModeRectangleCommand : TextureModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureModeRectangleCommand(object caller)
      : base(caller)
    {
      _text = "Rectangle";
      _toolTipText = "Toggle use of rectangle textures";
      _mode = CadKit.Interfaces.TextureMode.Mode.RECTANGLE;
    }
  }


  public class TextureModeCubeMapCommand : TextureModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureModeCubeMapCommand(object caller)
      : base(caller)
    {
      _text = "Cube Map";
      _toolTipText = "Toggle use of cube-mapped textures";
      _mode = CadKit.Interfaces.TextureMode.Mode.CUBE_MAP;
    }
  }


  public class TextureModeAllCommand : TextureModeCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureModeAllCommand(object caller)
      : base(caller)
    {
      _text = "All Modes";
      _toolTipText = "Toggle use of all textures modes";
      _mode = CadKit.Interfaces.TextureMode.Mode.ALL;
    }
  }
}
