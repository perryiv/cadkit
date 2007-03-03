
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class SceneExport
  {
    public enum Option
    {
      ENTIRE_SCENE,
      MODEL_ONLY
    }
  }

  public interface IExportScene : IReferenced
  {
    Filters Filters { get; }
    void export(string filename, SceneExport.Option option);
  }
}
