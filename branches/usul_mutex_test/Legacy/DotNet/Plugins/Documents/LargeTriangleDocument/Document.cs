
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.LargeTriangleDocument
{
  public class Document :
    CadKit.Documents.Document,
    CadKit.Interfaces.IFileOpen,
    CadKit.Interfaces.IFiltersInsert
  {
    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Large Triangle Document";


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
      return CadKit.Plugins.Documents.LargeTriangleDocument.Document.TypeName;
    }


    /// <summary>
    /// Return the open-filters.
    /// </summary>
    public static CadKit.Interfaces.Filters FiltersOpen
    {
      get
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("All Triangle Files (*.tdf *.stl *.r3d)", "*.tdf;*.stl;*.r3d"));
        filters.Add(new CadKit.Interfaces.Filter("Triangle Document Format (*.tdf)", "*.tdf"));
        filters.Add(new CadKit.Interfaces.Filter("Stereolithography (*.stl)", "*.stl"));
        filters.Add(new CadKit.Interfaces.Filter("RoboMet 3D (*.r3d)", "*.r3d"));
        return filters;
      }
    }


    /// <summary>
    /// Return the insert-filters.
    /// </summary>
    public static CadKit.Interfaces.Filters FiltersInsert
    {
      // We can insert the same formats that we open.
      get { return CadKit.Plugins.Documents.LargeTriangleDocument.Document.FiltersOpen; }
    }


    /// <summary>
    /// Return the insert-filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IFiltersInsert.Filters
    {
      get { return CadKit.Plugins.Documents.LargeTriangleDocument.Document.FiltersInsert; }
    }


    /// <summary>
    /// Read the file.
    /// </summary>
    void CadKit.Interfaces.IFileOpen.open(string name, object caller)
    {
      using (this.Lock.write())
      {
        CadKit.Interfaces.IProgressBar progress = caller as CadKit.Interfaces.IProgressBar;
        if (null != progress)
        {
          int total = progress.Range;
          for (int i = 0; i < total; ++i)
          {
            progress.Value = i;
            System.Threading.Thread.Sleep(1000);
          }
          progress.Value = total;
        }
      }
    }
  }
}
