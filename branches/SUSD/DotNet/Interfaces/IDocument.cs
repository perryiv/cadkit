
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IDocument : IReferenced
  {
    /// <summary>
    /// Set/get the gui-delegate.
    /// </summary>
    CadKit.Interfaces.IGuiDelegate GuiDelegate { get; set; }

    /// <summary>
    /// Set/get the flag that says if this document has been modified 
    /// after the last time it was saved.
    /// </summary>
    bool Modified { get; set; }

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
    /// True if a name has not been assigned (e.g., "Untitled1").
    /// </summary>
    bool HasDefaultName { get; }

    /// <summary>
    /// Close the document and all it's views.
    /// </summary>
    void close();

    /// <summary>
    /// Add a view.
    /// </summary>
    void add(CadKit.Interfaces.IDocumentView view);

    /// <summary>
    /// Remove a view.
    /// </summary>
    void remove(CadKit.Interfaces.IDocumentView view);

    /// <summary>
    /// Does the document contain the view?
    /// </summary>
    bool contains(CadKit.Interfaces.IDocumentView view);

    /// <summary>
    /// Get the array of views.
    /// </summary>
    CadKit.Interfaces.IDocumentView[] views();
  }
}
