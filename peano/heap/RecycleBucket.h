#ifndef PEANO_HEAP_RECYCLEBUCKET_H
#define PEANO_HEAP_RECYCLEBUCKET_H

#include <list>
#include <vector>

namespace peano {
  namespace heap {
    class RecycleBucket {

    private:
      typedef std::list<int> recycleList;
      std::vector<recycleList> _data;

      int get_bucket_index(const int& heap_size); //return 0 if heapsize < 0
      //TODO consider running size

    public:
      typedef recycleList::size_type size_type;
      typedef recycleList::reference reference;

      RecycleBucket(): _data(10){}
      // virtual ~RecycleBucket(); //use default

      bool empty() const;
      bool empty(const int& heap_size);
      size_type size() const;
      // void remove(const int value); // search through all buckets
      void remove(const int& heap_size, const int& value); //search in specific bucket
      void push_back(const int& heap_size, const int& value);
      reference front(const int& heap_size);
      void pop_front(const int& heap_size);

    };

  } //peano
} //heap

#endif /* PEANO_HEAP_RECYCLEBUCKET_H */
