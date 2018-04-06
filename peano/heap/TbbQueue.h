#ifndef TBBQUEUE_H
#define TBBQUEUE_H
#include "tbb/concurrent_queue.h"
#include <atomic>

class TbbQueue
{
private:
  std::atomic<unsigned> _size;
  tbb::concurrent_queue<int> _data;
  
public:
  typedef unsigned size_type;

  TbbQueue(): _size(0), _data() {}
  void push(int key);
  int pop();
  size_type size();
  bool empty();

};


#endif /* TBBQUEUE_H */
