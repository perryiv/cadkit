
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class InsertDocumentTask : ReadDocumentTask
  {
    /// <summary>
    /// Local data members.
    /// </summary>
    private CadKit.Interfaces.IDocument _document = CadKit.Documents.Manager.Instance.ActiveDocument;


    /// <summary>
    /// Constructor.
    /// </summary>
    public InsertDocumentTask(string file, object caller) : base ( file, caller )
    {
    }


    /// <summary>
    /// Called when the thread starts.
    /// </summary>
    protected override void _startJob(CadKit.Threads.Jobs.Job job)
    {
      // Should be true.
      System.Diagnostics.Debug.Assert(false == CadKit.Threads.Tools.MainThread.Instance.IsMainThread);

      // Make sure there is a document.
      if (null == this.Document)
        return;

      // See if the document can insert files.
      CadKit.Interfaces.IFileInsert insert = this.Document as CadKit.Interfaces.IFileInsert;
      if (null == insert)
        return;

      // Feedback.
      System.Console.WriteLine(System.String.Format("Inserting file '{0}' into '{1}'", this.File, this.Document.Name));

      // Insert the file.
      insert.insert(this.File, this);

      // The document is now modified.
      this.Document.Modified = true;
    }


    /// <summary>
    /// Get the document.
    /// </summary>
    protected CadKit.Interfaces.IDocument Document
    {
      get { using (this.Lock.read()) { return _document; } }
    }
  }
}
