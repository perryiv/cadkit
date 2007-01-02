
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IImageLayer
  {
    void addImageLayer(string filename);
    void hideImageLayer(string filename);
    void showImageLayer(string filename);
    void removeImageLayer(string filename);
  }
}
