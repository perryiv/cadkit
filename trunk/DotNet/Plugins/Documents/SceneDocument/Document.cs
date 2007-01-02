
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.SceneDocument
{
  public class Document :
    CadKit.Documents.Document,
    CadKit.Interfaces.IFileOpen,
    CadKit.Interfaces.IFileInsert,
    CadKit.Interfaces.IBuildScene,
    CadKit.Interfaces.IFiltersInsert
  {
    /// <summary>
    /// Root for the scene.
    /// </summary>
    private CadKit.OSG.Glue.Node _root = new CadKit.OSG.Glue.Group();


    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Scene Document";


    /// <summary>
    /// Construct a document.
    /// </summary>
    public Document()
    {
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Document()
    {
      this._clearScene();
    }


    /// <summary>
    /// Clear the scene.
    /// </summary>
    private void _clearScene()
    {
      if (null != _root)
      {
        _root.clear();
        _root = null;
      }
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    protected override string _typeName()
    {
      return CadKit.Plugins.Documents.SceneDocument.Document.TypeName;
    }


    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IFileOpen.open(string file, object caller)
    {
      this.open(file, caller);
    }


    /// <summary>
    /// Read the file and add it to the scene.
    /// </summary>
    public void open(string file, object caller)
    {
      // Make sure we're empty.
      this._clearScene();

      // Insert the new file.
      this.insert(file, caller);

      // Set document name.
      this.Name = file;

      // Add file to recent-file list.
      CadKit.Interfaces.IRecentFileList recent = caller as CadKit.Interfaces.IRecentFileList;
      if (null != recent)
      {
        recent.add(file);
      }
    }


    /// <summary>
    /// Insert the file's contents.
    /// </summary>
    void CadKit.Interfaces.IFileInsert.insert(string file, object caller)
    {
      this.insert(file, caller);
    }


    /// <summary>
    /// Insert the file's contents.
    /// </summary>
    public void insert(string file, object caller)
    {
      CadKit.OSG.Glue.Node node = CadKit.Plugins.Documents.SceneDocument.Document._read(file, caller);
      this._updateRoot(node);
    }


    /// <summary>
    /// Read the file and return the node.
    /// </summary>
    private static CadKit.OSG.Glue.Node _read(string name, object caller)
    {
      CadKit.OSG.Glue.ReadFile reader = null;
      string cwd = null;
      try
      {
        // Make the path the current working directory. 
        // This will increase the chances of textures loading.
        cwd = System.IO.Directory.GetCurrentDirectory();
        System.IO.FileInfo info = new System.IO.FileInfo(name);
        System.IO.Directory.SetCurrentDirectory(info.DirectoryName);

        // Read the file.
        reader = new CadKit.OSG.Glue.ReadFile();
        return reader.readNodeFile(name, caller);
      }
      finally
      {
        // Always reset the current working directory.
        if (null != cwd && System.IO.Directory.Exists(cwd))
        {
          System.IO.Directory.SetCurrentDirectory(cwd);
        }

        // Always clear the reader.
        if (null != reader)
        {
          reader.clear();
        }
      }
    }


    /// <summary>
    /// Update the scene root. If the root is empty then the given node 
    /// becomes the new root. If the root already has children then the new 
    /// node is added.  
    /// </summary>
    public void _updateRoot(CadKit.OSG.Glue.Node node)
    {
      using (this.Lock.write())
      {
        _root = CadKit.OSG.Glue.Tools.organizeScene(_root, node);
      }
    }


    /// <summary>
    /// Get the scene.
    /// </summary>
    object CadKit.Interfaces.IBuildScene.Scene
    {
      get { using (this.Lock.read()) { return _root; } }
    }


    /// <summary>
    /// Return the open-filters.
    /// </summary>
    public static CadKit.Interfaces.Filters FiltersOpen
    {
      get { return CadKit.OSG.Glue.ReadFile.filters(); }
    }


    /// <summary>
    /// Return the insert-filters.
    /// </summary>
    public static CadKit.Interfaces.Filters FiltersInsert
    {
      // We can insert the same formats that we open.
      get { return CadKit.Plugins.Documents.SceneDocument.Document.FiltersOpen; }
    }


    /// <summary>
    /// Return the insert-filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersInsert.Filters
    {
      get { return CadKit.Plugins.Documents.SceneDocument.Document.FiltersInsert; }
    }
  }
}
