
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class TextureMode
  {
    public enum Mode { ALL, D1, D2, D3, RECTANGLE, CUBE_MAP };
  }

  public interface ITextureMode : IReferenced
  {
    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool OneD { get; set; }
    bool TwoD { get; set; }
    bool ThreeD { get; set; }
    bool Rectangle { get; set; }
    bool CubeMap { get; set; }

    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    void mode(CadKit.Interfaces.TextureMode.Mode mode, bool state);
    bool mode(CadKit.Interfaces.TextureMode.Mode mode);
  }
}
