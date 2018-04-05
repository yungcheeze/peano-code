#ifndef PEANO_HEAP_RECYCLEBUCKET_H
#define PEANO_HEAP_RECYCLEBUCKET_H

#include <list>
#include <vector>
#include <map>
#include <atomic>
#include "tarch/multicore/BooleanSemaphore.h"

namespace peano {
  namespace heap {
    class RecycleBucket {

    private:
      typedef std::list<int> recycleList;
      std::map<int, recycleList> _data;
      std::map<int, recycleList> _dummy;
      struct RefCounter { bool lockAcquired; int count; };
      std::atomic<RefCounter> _lookupCounter;
      tarch::multicore::BooleanSemaphore _dataMapSemaphore;


      void incrementRefCount();
      void decrementRefCount();
      void setLockAquiredFlag();
      void unsetLockAquiredFlag();
      //TODO nifty way decrementing RefCount after return (put in destructor)
      // struct LockWait {
      //   RefCounter _expected;
      //   RefCounter _new_val;
      //   LockWait();
      //   ~LockWait();
      // };

      /*
       * Mapping Function: maps heap_size to key in _data map.
       * currently a one to one mapping i.e. key = heap_size;
       */
      int get_bucket_index(const int& heap_size);

      /*
       * gets recycleList located at index.
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

      bool empty();
      bool empty(const int& heap_size);
      void remove(const int& heap_size, const int& value);
      void push_back(const int& heap_size, const int& value); 
      reference front(const int& heap_size);
      void pop_front(const int& heap_size);
      size_type size();

    };

  } //peano
} //heap

#endif /* PEANO_HEAP_RECYCLEBUCKET_H */
