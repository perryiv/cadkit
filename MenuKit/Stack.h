#ifndef _menukit_detail_stack_h_
#define _menukit_detail_stack_h_

#include <list>

namespace Detail
{

  template<class TypeName>
  class Stack
  {
  public:
    typedef std::list<TypeName> DataVec;
    Stack(): _data() {}
    Stack(const Stack& s): _data(s._data) {}
    ~Stack() {}

    Stack& operator =(const Stack& s)
    {
      _data = s._data;
      return (*this);
    }

    void push(const TypeName& data)
    {
      _data.push_back(data);
    }

    void pop()  // TODO: test this member
    {
      if( !_data.empty() )
        _data.remove( *(--_data.end()) );  // .remove only valid for list<>
    }

    const TypeName& top() { return *(--_data.end()); }  // TODO: test

    void data(const DataVec& d) { _data = d; }
    const DataVec& data() const { return _data; }

  private:
    DataVec _data;
  };

};

#endif
