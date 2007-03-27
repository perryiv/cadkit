
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IPlugin : IReferenced
  {
    /// <summary>
    /// Get the name of this plugin.
    /// </summary>
    string Name { get; }

    /// <summary>
    /// Get the plugin's description.
    /// </summary>
    string Description { get; }

    /// <summary>
    /// Called when the plugin is loaded.
    /// </summary>
    void start(object caller);

    /// <summary>
    /// Called when use of the plugin is finished.
    /// </summary>
    void finish(object caller);
  }
}
