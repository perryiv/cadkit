
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

using System.Windows.Forms.Design;

namespace DT.Minerva.Layers.Controls
{
  public class ColorUITypeEditor : System.Drawing.Design.UITypeEditor
  {
    public ColorUITypeEditor()
    {
    }

    public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(System.ComponentModel.ITypeDescriptorContext context)
    {
      return System.Drawing.Design.UITypeEditorEditStyle.DropDown;
    }

    public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, System.IServiceProvider provider, object value)
    {
      IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
      if (edSvc != null)
      {
        ColorChooser cc = new ColorChooser(context.Instance, edSvc);
        edSvc.DropDownControl(cc);
      }
      
      return base.EditValue(context, provider, value);
    }
  }
}
