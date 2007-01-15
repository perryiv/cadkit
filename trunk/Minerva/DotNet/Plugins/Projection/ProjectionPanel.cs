
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Projection
{
  public partial class ProjectionPanel : System.Windows.Forms.UserControl
  {
    public ProjectionPanel(CadKit.Interfaces.IOptionsPage page ) : base()
    {
      InitializeComponent();

      this.radioButton1.Checked = true;
    }

    private void radioButton1_CheckedChanged(object sender, System.EventArgs e)
    {
      this.checkBox1.Enabled = false;
      DT.Minerva.Lib.Controller.Instance.transfromProjection(false);
    }

    private void radioButton2_CheckedChanged(object sender, System.EventArgs e)
    {
      this.checkBox1.Enabled = true;
      DT.Minerva.Lib.Controller.Instance.transfromProjection(true);
      DT.Minerva.Lib.Controller.Instance.projectionID(4326); // 4326 is WGS 84.
    }

    private void checkBox1_CheckedChanged(object sender, System.EventArgs e)
    {
      DT.Minerva.Lib.Controller.Instance.convertFromSphericalCoordinates(this.checkBox1.Checked);
    }

    private void radioButton3_CheckedChanged(object sender, System.EventArgs e)
    {
      DT.Minerva.Lib.Controller.Instance.convertToFirstProjection(this.radioButton3.Checked);
    }
  }
}
