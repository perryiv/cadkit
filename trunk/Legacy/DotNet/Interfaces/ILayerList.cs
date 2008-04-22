
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface ILayerList : IReferenced
  {
    CadKit.Interfaces.ILayer[] Layers { get; }

    void addLayer(CadKit.Interfaces.ILayer layer, object caller);
    void modifyLayer(CadKit.Interfaces.ILayer layer, object caller);
    void hideLayer(CadKit.Interfaces.ILayer layer, object caller);
    void showLayer(CadKit.Interfaces.ILayer layer, object caller);
    void removeLayer(CadKit.Interfaces.ILayer layer, object caller);
    void viewLayerExtents(CadKit.Interfaces.ILayer layer);
    
    void addToFavorites(CadKit.Interfaces.ILayer layer);
    CadKit.Interfaces.ILayer createFavorite(string name);
    string[] Favorites { get; }
  }
}
