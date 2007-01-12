
namespace CadKit.Interfaces
{
  public interface ILayer
  {
    string Name { get; set; }
    bool Shown { get; }
    bool NeedsUpdate { get; set; }

    void show();
    void modify();
    void hide();
  }
}
