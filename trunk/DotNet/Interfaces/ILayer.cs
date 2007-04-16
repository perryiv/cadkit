
namespace CadKit.Interfaces
{
  public interface ILayer : IReferenced
  {
    string Guid { get; }
    string Name { get; }
    bool Shown { get; }
  }
}
