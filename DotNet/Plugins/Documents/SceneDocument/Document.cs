
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
    CadKit.Interfaces.IRead,
    CadKit.Interfaces.IBuildScene
  {
    /// <summary>
    /// Root for the scene.
    /// </summary>
    private CadKit.OSG.Glue.Node _root = null;

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
    /// Return the short name of this type.
    /// </summary>
    protected override string _typeName()
    {
      return CadKit.Plugins.Documents.SceneDocument.Document.TypeName;
    }

    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IRead.read(string name, object caller)
    {
      using (this.Lock.write())
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
          _root = reader.readNodeFile(name, caller);

          // Set document name.
          CadKit.Interfaces.IDocument idoc = this as CadKit.Interfaces.IDocument;
          if (null != idoc)
          {
            idoc.Name = name;
          }

          // Add file to recent-file list.
          CadKit.Interfaces.IRecentFileList recent = caller as CadKit.Interfaces.IRecentFileList;
          if (null != recent)
          {
            recent.add(name);
          }
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
    }


    /// <summary>
    /// Get the scene.
    /// </summary>
    object CadKit.Interfaces.IBuildScene.Scene
    {
      get { using (this.Lock.read()) { return _root; } }
    }
  }
}
