
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public enum ViewMode
  {
    NAVIGATE = 0,
    PICK,
    SEEK
  };

  public interface IViewerMode : IReferenced
  {
    ViewMode Mode { get; set; }
  }
}
