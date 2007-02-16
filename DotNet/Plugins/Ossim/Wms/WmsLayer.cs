
namespace CadKit.Plugins.Layers.Ossim.Wms
{
  public class WmsLayer : System.Windows.Forms.UserControl
  {
    private CadKit.Interfaces.IOptionsPage _page;
    private System.Windows.Forms.TextBox _cacheDirectory;
    private System.Windows.Forms.TextBox _server;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Panel panel1;

    /// <summary>
    /// Local constants.
    /// </summary>
    private const string REGISTRY_SECTION = "WmsLayer";
    private const string SERVER = "Server";
    private System.Windows.Forms.Button _clearButton;
    private System.Windows.Forms.ComboBox _recent;
    private const string CACHE_DIR = "CacheDir";

    public WmsLayer(CadKit.Interfaces.IOptionsPage page)
    {
      this.InitializeComponent();

      this.Load += this._load;

      _page = page;
      if (null != _page)
        _page.Apply += this._apply;

      string recentServers = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, SERVER, "");
      string[] servers = recentServers.Split(new char[] { '|' });
      foreach (string s in servers)
        _recent.Items.Add(s);

      _cacheDirectory.Text = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, CACHE_DIR, "");
    }

    #region Windows Designer Generated Code

    private void InitializeComponent()
    {
      this.panel1 = new System.Windows.Forms.Panel();
      this._cacheDirectory = new System.Windows.Forms.TextBox();
      this._server = new System.Windows.Forms.TextBox();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this._recent = new System.Windows.Forms.ComboBox();
      this._clearButton = new System.Windows.Forms.Button();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this._clearButton);
      this.panel1.Controls.Add(this._recent);
      this.panel1.Controls.Add(this._cacheDirectory);
      this.panel1.Controls.Add(this._server);
      this.panel1.Controls.Add(this.label2);
      this.panel1.Controls.Add(this.label1);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(0, 0);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(677, 199);
      this.panel1.TabIndex = 2;
      // 
      // _cacheDirectory
      // 
      this._cacheDirectory.Location = new System.Drawing.Point(123, 92);
      this._cacheDirectory.Name = "_cacheDirectory";
      this._cacheDirectory.Size = new System.Drawing.Size(450, 20);
      this._cacheDirectory.TabIndex = 3;
      // 
      // _server
      // 
      this._server.Location = new System.Drawing.Point(123, 52);
      this._server.Name = "_server";
      this._server.Size = new System.Drawing.Size(450, 20);
      this._server.TabIndex = 2;
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(13, 92);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(83, 13);
      this.label2.TabIndex = 1;
      this.label2.Text = "Cache Directory";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 52);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(38, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "Server";
      // 
      // _recent
      // 
      this._recent.FormattingEnabled = true;
      this._recent.Location = new System.Drawing.Point(123, 15);
      this._recent.Name = "_recent";
      this._recent.Size = new System.Drawing.Size(450, 21);
      this._recent.TabIndex = 4;
      this._recent.SelectedIndexChanged += new System.EventHandler(this._recent_SelectedIndexChanged);
      // 
      // _clearButton
      // 
      this._clearButton.Location = new System.Drawing.Point(579, 15);
      this._clearButton.Name = "_clearButton";
      this._clearButton.Size = new System.Drawing.Size(75, 23);
      this._clearButton.TabIndex = 5;
      this._clearButton.Text = "Clear";
      this._clearButton.UseVisualStyleBackColor = true;
      this._clearButton.Click += new System.EventHandler(this._clearButton_Click);
      // 
      // WmsLayer
      // 
      this.BackColor = System.Drawing.SystemColors.Control;
      this.Controls.Add(this.panel1);
      this.Name = "WmsLayer";
      this.Size = new System.Drawing.Size(677, 199);
      this.panel1.ResumeLayout(false);
      this.panel1.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion


    /// <summary>
    /// Called when the form is loaded.
    /// </summary>
    void _load(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent)
        parent.FormClosing += this._formClosing;
    }

    /// <summary>
    /// Called when the parent form is closing.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent)
        parent.FormClosing -= this._formClosing;

      if (null != _page)
        _page.Apply -= this._apply;
    }


    /// <summary>
    /// Called when the apply (or ok) button is pressed.
    /// </summary>
    private void _apply(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.IOptionsPage page)
    {
      CadKit.Interfaces.ILayerList layerList = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.ILayerList;

      if (null != layerList && _server.Text.Length > 0 && _cacheDirectory.Text.Length > 0)
      {
        CadKit.OSSIMPlanet.Glue.WmsLayer wmsLayer = new CadKit.OSSIMPlanet.Glue.WmsLayer();
        wmsLayer.Server = _server.Text;
        wmsLayer.CacheDirectory = _cacheDirectory.Text;

        layerList.addLayer(wmsLayer, this);

        // Save in registry.
        string recentServers = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, SERVER, "");
        CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, SERVER, recentServers + "|" + _server.Text);
        CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, CACHE_DIR, _cacheDirectory.Text);
      }
    }


    /// <summary>
    /// Clear the recent servers list.
    /// </summary>
    private void _clearButton_Click(object sender, System.EventArgs e)
    {
      CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, SERVER, "");
      _recent.Items.Clear();
    }


    /// <summary>
    /// Set the text box to value in combo box.
    /// </summary>
    private void _recent_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      _server.Text = (string)_recent.SelectedItem;
    }
  }
}
