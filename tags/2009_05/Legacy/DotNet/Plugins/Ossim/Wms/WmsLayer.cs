
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Layers.Ossim.Wms
{
  public class WmsSettings
  {
    public string Server;
    public string CacheDirectory;
    public string ImageType;

    public override bool Equals(object obj)
    {
      if (obj is WmsSettings)
      {
        WmsSettings ws = (WmsSettings)obj;
        return ws.Server == this.Server;
      }

      return base.Equals(obj);
    }
    public override int GetHashCode()
    {
      return base.GetHashCode();
    }
  }

  public class WmsLayer : System.Windows.Forms.UserControl
  {
    private System.Collections.Generic.List<WmsSettings> _recentServers = new System.Collections.Generic.List<WmsSettings>();

    private CadKit.Interfaces.IOptionsPage _page;
    private System.Windows.Forms.TextBox _cacheDirectory;
    private System.Windows.Forms.TextBox _server;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Button _clearButton;
    private System.Windows.Forms.ComboBox _recent;
    private System.Windows.Forms.CheckBox _png;
    private System.Windows.Forms.CheckBox _jpeg;
    

    public WmsLayer(CadKit.Interfaces.IOptionsPage page)
    {
      this.InitializeComponent();

      this.Load += this._load;

      _page = page;
      if (null != _page)
        _page.Apply += this._apply;

      this._deserialize();

      _recent.Items.Clear();

      foreach (WmsSettings settings in _recentServers)
        _recent.Items.Add(settings.Server);

      _jpeg.Checked = true;
    }

    #region Windows Designer Generated Code

    private void InitializeComponent()
    {
      this.panel1 = new System.Windows.Forms.Panel();
      this._clearButton = new System.Windows.Forms.Button();
      this._recent = new System.Windows.Forms.ComboBox();
      this._cacheDirectory = new System.Windows.Forms.TextBox();
      this._server = new System.Windows.Forms.TextBox();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this._jpeg = new System.Windows.Forms.CheckBox();
      this._png = new System.Windows.Forms.CheckBox();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this._png);
      this.panel1.Controls.Add(this._jpeg);
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
      // _recent
      // 
      this._recent.FormattingEnabled = true;
      this._recent.Location = new System.Drawing.Point(123, 15);
      this._recent.Name = "_recent";
      this._recent.Size = new System.Drawing.Size(450, 21);
      this._recent.TabIndex = 4;
      this._recent.SelectedIndexChanged += new System.EventHandler(this._recent_SelectedIndexChanged);
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
      // _jpeg
      // 
      this._jpeg.AutoSize = true;
      this._jpeg.Location = new System.Drawing.Point(123, 133);
      this._jpeg.Name = "_jpeg";
      this._jpeg.Size = new System.Drawing.Size(49, 17);
      this._jpeg.TabIndex = 6;
      this._jpeg.Text = "Jpeg";
      this._jpeg.UseVisualStyleBackColor = true;
      // 
      // _png
      // 
      this._png.AutoSize = true;
      this._png.Location = new System.Drawing.Point(123, 156);
      this._png.Name = "_png";
      this._png.Size = new System.Drawing.Size(45, 17);
      this._png.TabIndex = 7;
      this._png.Text = "Png";
      this._png.UseVisualStyleBackColor = true;
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


    private void _deserialize()
    {
      if (this.hasRecentServerData())
      {
        System.IO.Stream stream = null;
        try
        {
          stream = System.IO.File.Open(this.Filename, System.IO.FileMode.OpenOrCreate);
          System.Xml.Serialization.XmlSerializer formatter = new System.Xml.Serialization.XmlSerializer(typeof(System.Collections.Generic.List<WmsSettings>));
          //System.Runtime.Serialization.Formatters.Binary.BinaryFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
          _recentServers = (System.Collections.Generic.List<WmsSettings>)formatter.Deserialize(stream);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 2270499180: Exception caught while trying to deserialize recent servers.");
          System.Console.WriteLine("Message: {0}", e.Message);
        }
        finally
        {
          if (null != stream)
            stream.Close();
        }
      }
    }


    private void _serialize()
    {
      System.IO.Stream stream = null;
      try
      {
        stream = System.IO.File.Open(this.Filename, System.IO.FileMode.Create);
        System.Xml.Serialization.XmlSerializer formatter = new System.Xml.Serialization.XmlSerializer(typeof(System.Collections.Generic.List<WmsSettings>));
        //System.Runtime.Serialization.Formatters.Binary.BinaryFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

        formatter.Serialize(stream, _recentServers);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2619561680: Exception caught while trying to serialize recent servers.");
        System.Console.WriteLine("Message: {0}", e.Message);
      }
      finally
      {
        if (null != stream)
          stream.Close();
      }
    }


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

      // Make sure serialize is called after apply.
      this._serialize();
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

        if (_jpeg.Checked)
          wmsLayer.ImageType = "image/jpeg";
        else if (_png.Checked)
          wmsLayer.ImageType = "image/png";

        layerList.addLayer(wmsLayer, this);

        WmsSettings settings = new WmsSettings();
        settings.Server = wmsLayer.Server;
        settings.CacheDirectory = wmsLayer.CacheDirectory;
        settings.ImageType = wmsLayer.ImageType;

        if (false == _recentServers.Contains(settings))
        {
          _recentServers.Add(settings);
          _recent.Items.Add(settings.Server);
        }
      }
    }


    /// <summary>
    /// Clear the recent servers list.
    /// </summary>
    private void _clearButton_Click(object sender, System.EventArgs e)
    {
      _recent.Items.Clear();
      _recentServers.Clear();
    }


    /// <summary>
    /// Set the text box to value in combo box.
    /// </summary>
    private void _recent_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      WmsSettings settings = _recentServers[_recent.SelectedIndex];
      _server.Text = settings.Server;
      _cacheDirectory.Text = settings.CacheDirectory;

      _jpeg.Checked = false;
      _png.Checked = false;

      if (settings.ImageType.Contains("jpeg"))
        _jpeg.Checked = true;
      else if (settings.ImageType.Contains("png"))
        _png.Checked = true;
    }


    /// <summary>
    /// Filename to save infomation.
    /// </summary>
    protected string Filename
    {
      get
      {
        return System.Windows.Forms.Application.UserAppDataPath + "\\recent_wms_servers.xml";
      }
    }


    /// <summary>
    /// See if there is saved server data.
    /// </summary>
    protected bool hasRecentServerData()
    {
      if (System.IO.File.Exists(this.Filename))
      {
        System.IO.FileInfo info = new System.IO.FileInfo(this.Filename);
        if (info.Length > 0)
          return true;
      }

      return false;
    }
  }
}
