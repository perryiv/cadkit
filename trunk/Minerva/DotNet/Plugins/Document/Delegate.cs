
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Document
{
  class Delegate :
    CadKit.Delegates.Delegate,
    CadKit.Interfaces.IGuiDelegate
  {
    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Minerva Delegate";

    /// <summary>
    /// Construct a delegate.
    /// </summary>
    public Delegate()
    {
    }

    /// <summary>
    /// Set the document.
    /// </summary>
    object CadKit.Interfaces.IGuiDelegate.Document
    {
      get { lock (this.Mutex) { return this.Document; } }
      set { lock (this.Mutex) { this.Document = value as CadKit.Interfaces.IDocument; } }
    }


    /// <summary>
    /// Create the default gui.
    /// </summary>
    void CadKit.Interfaces.IGuiDelegate.create(object caller)
    {
      lock (this.Mutex)
      {
        if (null == this.Document)
          return;

        // Make new viewer.
        CadKit.Viewer.Viewer view = new CadKit.Viewer.Viewer();
        view.Shown += new System.EventHandler(view_Shown);
        view.Icon = System.Windows.Forms.Application.OpenForms[0].Icon;
        view.Text = this.Document.Name;

        // Attach viewer and document to each other.
        view.Document = this.Document;
        view.Document.add(view);

        // Set up the document.
        Document doc = this.Document as Document;
        if (null != doc)
        {
          doc.Viewer = view.HeliosViewer;
          view.SizeChanged += new System.EventHandler(doc.resize);

          //doc.connect();
          //Minerva.Plugins.App.Distributed.SessionControl sessionDialog = new Minerva.Plugins.App.Distributed.SessionControl(doc.Sessions);
          //sessionDialog.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;

          //if (System.Windows.Forms.DialogResult.OK == sessionDialog.ShowDialog())
          //{
          //  parent.FormClosed += new System.Windows.Forms.FormClosedEventHandler(parent_FormClosed);
          //  doc.connectToSession(sessionDialog.sessionName());
          //}
        }

        // Get dock-panel and show.
        CadKit.Interfaces.IDockPanel getPanel = caller as CadKit.Interfaces.IDockPanel;
        WeifenLuo.WinFormsUI.DockPanel panel = (null != getPanel) ? (getPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel) : null;
        if (null != panel)
        {
          if (panel.DocumentStyle == WeifenLuo.WinFormsUI.DocumentStyles.SystemMdi)
          {
            view.MdiParent = caller as System.Windows.Forms.Form;
            CadKit.Tools.Size.mdiChild(panel.DocumentRectangle.Size, view);
            view.Show();
          }
          else
          {
            view.Show(panel);
          }
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void view_Shown(object sender, System.EventArgs e)
    {
      CadKit.Viewer.Viewer view = sender as CadKit.Viewer.Viewer;
      if (null != view)
        view.computeNearFar(false);
    }

    void parent_FormClosed(object sender, System.Windows.Forms.FormClosedEventArgs e)
    {
      Document doc = this.Document as Document;
      if (null != doc)
      {
        doc.deleteSession();
        doc.disconnect();
      }
    }


    /// <summary>
    /// Returns true if the document type is handled.
    /// </summary>
    bool CadKit.Interfaces.IGuiDelegate.doesHandle(string type)
    {
      lock (this.Mutex)
      {
        return (type == "Minerva Document");
      }
    }
  }
}
