#ifndef _graph_stream_visitor_h_
#define _graph_stream_visitor_h_

#include <ostream>
#include "osg/NodeVisitor"

class GraphStreamVisitor : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;
  enum DescribeType { NONE, ALL };

  GraphStreamVisitor(std::ostream& s):
    BaseClass(BaseClass::NODE_VISITOR, BaseClass::TRAVERSE_ALL_CHILDREN),
    _stream(s), _indent(0), _node(" (node)"), _group(" (group)"), _describe(NONE)
  {
  }

  void apply(osg::Node& node)
  {
    /// TODO: optimize this loop with STL
    for(unsigned int i=0; i<_indent; i++)
      _stream << " ";

    // TODO: add this to loop optimization
    _stream << " ";  // one extra space for alignment with parent names

    _stream << node.getName();

    switch( _describe )
    {
    case NONE: {} break;
    case ALL: { _stream << _node; } break;
    default: {} break;  /// TODO: error checking here?
    }

    _stream << std::endl;
  }

  void apply(osg::Group& group)
  {
    for(unsigned int i=0; i<_indent; i++)
      _stream << " ";

    _stream << "-" << group.getName();

    switch( _describe )
    {
    case NONE: {} break;
    case ALL: { _stream << _group; } break;
    default: {} break; ///TODO: error checking here?
    }

    _stream << std::endl;

    _indent++;  // increase the indentation
    traverse(group);  // continue with scene traversal
    _indent--;  // decrease the indentation
  }

  void describe(DescribeType d) { _describe = d; }
  DescribeType describe() const { return _describe; }

  void node_description(const std::string& n) { _node = n; }
  const std::string& node_description() { return _node; }

  void group_description(const std::string& n) { _group = n; }
  const std::string& group_description() { return _group; }

protected:
  virtual ~GraphStreamVisitor() {}

private:
  GraphStreamVisitor(const GraphStreamVisitor&); /// TODO: copy not implemented yet
  std::ostream& _stream;
  unsigned int _indent;
  std::string _node, _group;
  DescribeType _describe;
};

#endif
