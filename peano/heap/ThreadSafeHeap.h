#ifndef _PEANO_THREADSAFE_HEAP_H
#define _PEANO_THREADSAFE_HEAP_H

#include "tarch/logging/Log.h"
#include "tarch/logging/CommandLineLogger.h"

#include <vector>
#include <cstddef>

namespace peano {
    namespace heap {

        class ThreadSafeHeap
        {
        public:
            typedef std::vector<double>::iterator iterator;
            typedef std::vector<double>::const_iterator const_iterator;
            typedef std::vector<double>::size_type size_type;

            // constructors
            ThreadSafeHeap(size_type count); //TODO consider noexcept variant
            ~ThreadSafeHeap();

            //indexing
            double& operator[](size_type index);
            const double& operator[](size_type index) const;

            //iterators
            iterator begin();
            const_iterator begin() const;

            iterator end();
            const_iterator end() const;

            //data: return pointer to underlying array
            double* data();
            const double* data() const;

            // memory management
            void clear();
            void shrink_to_fit();
            void resize(size_type count);
            void reserve(size_type count);


        protected:

        private:
            static tarch::logging::Log _log;
            static std::size_t _numObj;

            std::vector<double> _data;
        };

    }  // heap
}  // peano


#endif /* _PEANO_THREADSAFE_HEAP_H */
