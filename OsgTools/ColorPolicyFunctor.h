#ifndef _osg_tools_color_policy_functor_h_
#define _osg_tools_color_policy_functor_h_

namespace osg
{
  class Node;
}

namespace OsgTools
{

  template<class ColorPolicy>
  class ColorPolicyFunctor
  {
  public:
    typedef ColorPolicy CP;
    ColorPolicyFunctor(): _cp() {}
    ColorPolicyFunctor(const ColorPolicyFunctor& c): _cp(c._cp) {}
    virtual ~ColorPolicyFunctor() {}

    ColorPolicyFunctor& operator =(const ColorPolicyFunctor& c) { _cp = c._cp; }

    virtual osg::Node* operator()() = 0;

    void color_policy(const ColorPolicy& c) { _cp = c; }
    const ColorPolicy& color_policy() const { return _cp; }
    ColorPolicy& color_policy()             { return _cp; }

  protected:
    CP _cp;
  };

};

#endif
