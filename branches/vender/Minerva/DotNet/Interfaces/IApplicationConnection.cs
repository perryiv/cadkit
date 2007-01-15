
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

using Strings = System.Collections.Generic.List<string>;

namespace DT.Minerva.Interfaces
{
  public interface IApplicationConnection
  {
    /// Remove layer with given id.
    void removeLayer(DT.Minerva.Interfaces.ILayer layer);

    /// <summary>
    ///  Show a layer.
    /// </summary>
    /// <param name="layer"></param>
    void showLayer(DT.Minerva.Interfaces.ILayer layer);

    /// Modify polygon data.
    void modifyLayer(DT.Minerva.Interfaces.ILayer layer);

    /// Start animation.
    void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays);

    /// Stop Animation.
    void stopAnimation();

  }
}
