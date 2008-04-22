
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class TextureEnvironmentAddCommand : TextureEnvironmentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureEnvironmentAddCommand(object caller)
      : base(caller)
    {
      _text = "Add";
      _toolTipText = "Add texture environment";
      _mode = CadKit.Interfaces.TextureEnvironment.Mode.ADD;
    }
  }


  public class TextureEnvironmentBlendCommand : TextureEnvironmentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureEnvironmentBlendCommand(object caller)
      : base(caller)
    {
      _text = "Blend";
      _toolTipText = "Blend texture environment";
      _mode = CadKit.Interfaces.TextureEnvironment.Mode.BLEND;
    }
  }


  public class TextureEnvironmentDecalCommand : TextureEnvironmentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureEnvironmentDecalCommand(object caller)
      : base(caller)
    {
      _text = "Decal";
      _toolTipText = "Decal texture environment";
      _mode = CadKit.Interfaces.TextureEnvironment.Mode.DECAL;
    }
  }


  public class TextureEnvironmentModulateCommand : TextureEnvironmentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureEnvironmentModulateCommand(object caller)
      : base(caller)
    {
      _text = "Modulate";
      _toolTipText = "Modulate texture environment";
      _mode = CadKit.Interfaces.TextureEnvironment.Mode.MODULATE;
    }
  }


  public class TextureEnvironmentReplaceCommand : TextureEnvironmentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public TextureEnvironmentReplaceCommand(object caller)
      : base(caller)
    {
      _text = "Replace";
      _toolTipText = "Replace texture environment";
      _mode = CadKit.Interfaces.TextureEnvironment.Mode.REPLACE;
    }
  }
}
