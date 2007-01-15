
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.DataExplorer
{
  public partial class DataExplorer : WeifenLuo.WinFormsUI.DockContent
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public DataExplorer()
    {
      InitializeComponent();

      this.DockableAreas =
            WeifenLuo.WinFormsUI.DockAreas.DockBottom |
            WeifenLuo.WinFormsUI.DockAreas.DockTop |
            WeifenLuo.WinFormsUI.DockAreas.DockLeft |
            WeifenLuo.WinFormsUI.DockAreas.DockRight |
            WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockLeft;
      this.HideOnClose = true;

      DT.Minerva.Lib.Controller.Instance.DataSourceAdded += new DT.Minerva.Lib.Controller.DataSourceAddedDelegate(_addConnection);
    }


    /// <summary>
    /// Add a connection.
    /// </summary>
    /// <param name="databaseConnection"></param>
    private void _addConnection(DT.Minerva.Interfaces.IDatabaseConnection databaseConnection)
    {
      string name = databaseConnection.Database + " on " + databaseConnection.Hostname;
      System.Windows.Forms.TreeNode treeNode = new System.Windows.Forms.TreeNode(name);

      DT.Minerva.Interfaces.IDataSource dataSource = databaseConnection as DT.Minerva.Interfaces.IDataSource;
      if (null != dataSource)
      {
        {
          System.Windows.Forms.TreeNode node = new System.Windows.Forms.TreeNode("Points");
          foreach (string table in dataSource.PointTables)
          {
            System.Windows.Forms.TreeNode n = new System.Windows.Forms.TreeNode(table);
            node.Nodes.Add(n);
          }
          treeNode.Nodes.Add(node);
        }

        {
          System.Windows.Forms.TreeNode node = new System.Windows.Forms.TreeNode("Point Time");
          foreach (string table in dataSource.PointTimeTables)
          {
            System.Windows.Forms.TreeNode n = new System.Windows.Forms.TreeNode(table);
            node.Nodes.Add(n);
            System.Windows.Forms.ContextMenuStrip menu = new System.Windows.Forms.ContextMenuStrip();
            menu.ShowImageMargin = false;
            System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
            item.Text = "Show time series.";
            item.Click += new System.EventHandler(_onShowTimeSeries);
            menu.Items.Add(item);
            n.ContextMenuStrip = menu;
          }
          treeNode.Nodes.Add(node);
        }

        {
          System.Windows.Forms.TreeNode node = new System.Windows.Forms.TreeNode("Lines");
          foreach (string table in dataSource.LineTables)
          {
            System.Windows.Forms.TreeNode n = new System.Windows.Forms.TreeNode(table);
            node.Nodes.Add(n);
          }
          treeNode.Nodes.Add(node);
        }

        {
          System.Windows.Forms.TreeNode node = new System.Windows.Forms.TreeNode("Polygons");
          foreach (string table in dataSource.PolygonTables)
          {
            System.Windows.Forms.TreeNode n = new System.Windows.Forms.TreeNode(table);
            node.Nodes.Add(n);
          }
          treeNode.Nodes.Add(node);
        }
      }

      _treeView.Nodes.Add(treeNode);
    }

    void _onShowTimeSeries(object sender, System.EventArgs e)
    {
      //System.Windows.Forms.TreeNode node = _treeView.SelectedNode;
      //while (node.Parent != null)
      //{
      //  node = node.Parent;
      //}
      //int index = _treeView.Nodes.IndexOf(node);
      //DT.Minerva.Interfaces.IDatabaseConnection db = DT.Minerva.Lib.Controller.Instance.DataSoucres[index];

      //string query = "SELECT count(wnv_date), wnv_date FROM wnv_allpoints1 WHERE species='Human' GROUP BY wnv_date ORDER BY wnv_date";
      //DT.Minerva.Interfaces.IQuery iQuery = db as DT.Minerva.Interfaces.IQuery;
      //if (null != iQuery)
      //{
      //  CadKit.Graph.ZedGraphForm graph = new CadKit.Graph.ZedGraphForm();
      //  ZedGraph.MasterPane master = graph.MasterPane;
      //  master.PaneList.Clear();

      //  // Fill the pane background with a color gradient
      //  master.Fill = new Fill(Color.White, Color.MediumSlateBlue, 45.0F);

      //  // Set the margins and the space between panes to 10 points
      //  master.Margin.All = 10;
      //  master.InnerPaneGap = 10;

      //  System.Data.IDataReader dr = iQuery.execteQueryIDataReader(query);
      //  System.Collections.Generic.Dictionary<System.DateTime, int> map = new System.Collections.Generic.Dictionary<System.DateTime, int>();
      //  while (dr.Read())
      //  {
      //    System.Int64 count = (System.Int64)dr[0];
      //    System.DateTime date = (System.DateTime)dr[1];
      //    map.Add(date, (int)count);
      //  }

      //  // Create a new GraphPane
      //  ZedGraph.GraphPane pane = new ZedGraph.GraphPane();
      //  pane.Title.Text = "Cases time series graph";
      //  pane.XAxis.Title.Text = "Date";
      //  pane.YAxis.Title.Text = "Number of Occurences";

      //  // Fill the pane background with a color gradient
      //  pane.Fill = new Fill(Color.White, Color.LightYellow, 45.0F);
      //  // Reduce the base dimension to 6 inches, since these panes tend to be smaller
      //  pane.BaseDimension = 6.0F;

      //  ZedGraph.PointPairList points = new ZedGraph.PointPairList();

      //  System.DateTime current = System.DateTime.Parse("4/11/2004");
      //  System.DateTime max = System.DateTime.Parse("12/31/2006");

      //  while (current < max)
      //  {
      //    int count = 0;
      //    for (int i = 0; i < 7; ++i)
      //    {
      //      if (map.ContainsKey(current))
      //        count += map[current];

      //      current = current.AddDays(1.0);
      //    }
      //    double x = (double)new ZedGraph.XDate(current);
      //    double y = (double)count;
      //    points.Add(x, y);
      //  }

      //  pane.AddCurve("time series", points, System.Drawing.Color.Black, ZedGraph.SymbolType.None);
      //  pane.XAxis.Type = ZedGraph.AxisType.Date;
      //  graph.ZedGraphControl.AxisChange();

      //  // Add the new GraphPane to the MasterPane
      //  master.Add(myPane);

      //  // Tell ZedGraph to auto layout all the panes
      //  using (System.Drawing.Graphics g = base.ZedGraphControl.CreateGraphics())
      //  {
      //    master.SetLayout(g, ZedGraph.PaneLayout.SquareColPreferred);
      //    master.AxisChange(g);
      //  }

      //  graph.Show();
      //}
    }
  }
}