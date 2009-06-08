
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class TextureEnvironment
  {
    public enum Mode { NONE, DECAL, MODULATE, BLEND, REPLACE, ADD };
  }

  public interface ITextureEnvironment : IReferenced
  {
    /// <summary>
    /// Set/get the texture environment
    /// </summary>
    TextureEnvironment.Mode Mode { get; set; }
  }
}
