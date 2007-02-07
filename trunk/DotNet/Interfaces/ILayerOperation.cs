using System;
using System.Collections.Generic;
using System.Text;

namespace CadKit.Interfaces
{
  public interface ILayerOperation
  {
    void setLayerOperation( String opType, int val, int[] layers );
    void setLayerOperation(String opType, int val, int layer);
  }
}
