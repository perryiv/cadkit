
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IDocument
  {
    /// <summary>
    /// Set/get the gui-delegate.
    /// </summary>
    object GuiDelegate { get; set; }

    /// <summary>
    /// Return true if this document has been modified 
    /// after the last time it was saved.
    /// </summary>
    bool Modified { get; }

    /// <summary>
    /// Get/set the name of this document. This may be the default 
    /// name (e.g., "Untitled1").
    /// </summary>
    string Name { get; set; }

    /// <summary>
    /// Get the name of this document type.
    /// </summary>
    string TypeName { get; }

    /// <summary>
    /// True if a name has not been assigned (e.g., "Untitled1".
    /// </summary>
    bool HasDefaultName { get; }
  }
}
