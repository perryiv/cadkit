using System;
using System.Collections.Generic;
using System.Text;

namespace CadKit.Interfaces
{
  public interface ILayerOperation : IReferenced
  {
    void setLayerOperation(String opType, int val, CadKit.Interfaces.ILayer layer);
  }
}
