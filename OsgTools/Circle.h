#ifndef _osg_tools_circle_builder_h_
#define _osg_tools_circle_builder_h_

namespace OsgTools
{

  template<class Vec2, class Float>
  class Circle
  {
  public:
    Circle(): _radius(10.0), _points(20) {}
    Circle(Float r, unsigned int p): _radius(r), _points(p) {}

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

    void points(unsigned int p) { _points = p; }
    unsigned int points() const { return p; }

    std::vector<Vec2> operator()() const
    {
      std::vector<osg::Vec2> pts;
      for(unsigned int i=0; i<_points; i++)
      {
        Float angle = 360.0 / _points;
        Float x = cos( angle );
        Float y = sin( angle );
        pts.push_back( Vec2(x,y) );
      }

      return pts;
    }

  private:
    Float _radius;
    unsigned int _points;
  };

};

#endif
