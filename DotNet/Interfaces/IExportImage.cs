
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IExportImage
  {
    Filters Filters { get; }
    bool exportImage(string filename);
    bool exportImage(string filename, int width, int height);
  }
}
