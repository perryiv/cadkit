
namespace CadKit.Interfaces
{
  public interface ILayer : IReferenced
  {
    string Name { get; set; }
    bool Shown { get; }

    void show();
    void modify();
    void hide();
  }
}
