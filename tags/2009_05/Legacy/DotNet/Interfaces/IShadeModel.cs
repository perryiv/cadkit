
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class ShadeModel
  {
    public enum Model { NONE, FLAT, SMOOTH };
  }

  public interface IShadeModel : IReferenced
  {
    /// <summary>
    /// Set/get the shade model.
    /// </summary>
    ShadeModel.Model Model { get; set; }
  }
}
