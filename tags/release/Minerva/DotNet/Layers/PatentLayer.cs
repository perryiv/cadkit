
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Controls
{
  class PatentLayer : BaseLayer
  {
    private DT.Minerva.Glue.PatentLayerGlue _layer = new DT.Minerva.Glue.PatentLayerGlue();

    public PatentLayer()
      : base()
    {
      this.Layer = _layer;
      this.Name = "Patent Data";
    }
  }
}
