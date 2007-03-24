
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Layers.PostGIS
{
  public partial class Properties : System.Windows.Forms.Form
  {
    public Properties(CadKit.Interfaces.ILayer layer)
    {
      InitializeComponent();

      if (layer is CadKit.Interfaces.IPropertyGridObject)
        _propertyGrid.SelectedObject = ((CadKit.Interfaces.IPropertyGridObject)layer).PropertyGridObject;
    }

    private void _cancel_Click(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.Close();
    }

    private void _ok_Click(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.Close();
    }
  }
}