#ifndef _menukit_detail_stack_h_
#define _menukit_detail_stack_h_

#include <vector>

namespace MenuKit
{
  namespace Detail
  {
    template<class TypeName>
    class Stack
    {
    public:
      typedef TypeName value_type;
      typedef std::vector<value_type> DataVec;

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

      void pop()
      {
        if( !_data.empty() )
          _data.pop_back();
      }

      const TypeName& top() { return *(--_data.end()); }  // TODO: test

      void data(const DataVec& d) { _data = d; }
      const DataVec& data() const { return _data; }

    private:
      DataVec _data;
    };
  };
};

#endif
