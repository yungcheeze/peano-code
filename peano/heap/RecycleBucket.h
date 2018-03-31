#ifndef PEANO_HEAP_RECYCLEBUCKET_H
#define PEANO_HEAP_RECYCLEBUCKET_H

#include <list>
#include <vector>
#include <map>

namespace peano {
  namespace heap {
    class RecycleBucket {

    private:
      typedef std::list<int> recycleList;
      std::map<int, recycleList> _data;

      /*
       * Mapping Function: maps heap_size to key in _data map.
       * currently a one to one mapping i.e. key = heap_size;
       */
      int get_bucket_index(const int& heap_size);

      /*
       * gets recycleList located at index.
       * if list doesn't exist, returns recycleList stored at index -1;
       */
      recycleList& get_bucket(const int& index);

      /*
       * inserts new recycleList at index.
       */
      void add_bucket(const int& index);

    public:
      typedef recycleList::size_type size_type;
      typedef recycleList::reference reference;
      typedef recycleList::value_type value_type;

      RecycleBucket();

      bool empty() const;
      bool empty(const int& heap_size);
      size_type size() const;
      void remove(const int& heap_size, const int& value); 
      void push_back(const int& heap_size, const int& value);
      reference front(const int& heap_size);
      void pop_front(const int& heap_size);

    };

  } //peano
} //heap

#endif /* PEANO_HEAP_RECYCLEBUCKET_H */
