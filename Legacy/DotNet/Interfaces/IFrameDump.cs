
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IFrameDump : IReferenced
  {
    string Directory { get; set; }
    string BaseFilename { get; set; }
    string Extension { get; set; }
    bool DumpFrames { get; set; }
  }
}
