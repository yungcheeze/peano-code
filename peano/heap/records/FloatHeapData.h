#ifndef _PEANO_HEAP_RECORDS_FLOATHEAPDATA_H
#define _PEANO_HEAP_RECORDS_FLOATHEAPDATA_H

#include "peano/utils/Globals.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "peano/utils/PeanoOptimisations.h"
#ifdef Parallel
	#include "tarch/parallel/Node.h"
#endif
#ifdef Parallel
	#include <mpi.h>
#endif
#include "tarch/logging/Log.h"
#include "tarch/la/Vector.h"
#include <bitset>
#include <complex>
#include <string>
#include <iostream>

namespace peano {
   namespace heap {
      namespace records {
         class FloatHeapData;
         class FloatHeapDataPacked;
      }
   }
}

/**
 * @author This class is generated by DaStGen
 * 		   DataStructureGenerator (DaStGen)
 * 		   2007-2009 Wolfgang Eckhardt
 * 		   2012      Tobias Weinzierl
 *
 * 		   build date: 09-02-2014 14:40
 *
 * @date   11/03/2015 11:53
 */
class peano::heap::records::FloatHeapData { 
   
   public:
      
      typedef peano::heap::records::FloatHeapDataPacked Packed;
      
      struct PersistentRecords {
         float _u;
         /**
          * Generated
          */
         PersistentRecords();
         
         /**
          * Generated
          */
         PersistentRecords(const float& u);
         
         /**
          * Generated
          */
          float getU() const ;
         
         /**
          * Generated
          */
          void setU(const float& u) ;
         
         
      };
      
   private: 
      public:

      PersistentRecords _persistentRecords;
      private:

      
   public:
      /**
       * Generated
       */
      FloatHeapData();
      
      /**
       * Generated
       */
      FloatHeapData(const PersistentRecords& persistentRecords);
      
      /**
       * Generated
       */
      FloatHeapData(const float& u);
      
      /**
       * Generated
       */
      ~FloatHeapData();
      
      /**
       * Generated
       */
       float getU() const ;
      
      /**
       * Generated
       */
       void setU(const float& u) ;
      
      /**
       * Generated
       */
      std::string toString() const;
      
      /**
       * Generated
       */
      void toString(std::ostream& out) const;
      
      
      PersistentRecords getPersistentRecords() const;
      /**
       * Generated
       */
      FloatHeapDataPacked convert() const;
      
      
   #ifdef Parallel
      protected:
         static tarch::logging::Log _log;
         
      public:
         
         /**
          * Global that represents the mpi datatype.
          * There are two variants: Datatype identifies only those attributes marked with
          * parallelise. FullDatatype instead identifies the whole record with all fields.
          */
         static MPI_Datatype Datatype;
         static MPI_Datatype FullDatatype;
         
         /**
          * Initializes the data type for the mpi operations. Has to be called
          * before the very first send or receive operation is called.
          */
         static void initDatatype();
         
         static void shutdownDatatype();
         
         /**
          * @param communicateSleep -1 Data exchange through blocking mpi
          * @param communicateSleep  0 Data exchange through non-blocking mpi, i.e. pending messages are received via polling until MPI_Test succeeds
          * @param communicateSleep >0 Same as 0 but in addition, each unsuccessful MPI_Test is follows by an usleep
          */
         void send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, int communicateSleep);
         
         void receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, int communicateSleep);
         
         static bool isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
         
         #endif
            
         };
         
         /**
          * @author This class is generated by DaStGen
          * 		   DataStructureGenerator (DaStGen)
          * 		   2007-2009 Wolfgang Eckhardt
          * 		   2012      Tobias Weinzierl
          *
          * 		   build date: 09-02-2014 14:40
          *
          * @date   11/03/2015 11:53
          */
         class peano::heap::records::FloatHeapDataPacked { 
            
            public:
               
               struct PersistentRecords {
                  float _u;
                  /**
                   * Generated
                   */
                  PersistentRecords();
                  
                  /**
                   * Generated
                   */
                  PersistentRecords(const float& u);
                  
                  /**
                   * Generated
                   */
                   float getU() const ;
                  
                  /**
                   * Generated
                   */
                   void setU(const float& u) ;
                  
                  
               };
               
            private: 
               PersistentRecords _persistentRecords;
               
            public:
               /**
                * Generated
                */
               FloatHeapDataPacked();
               
               /**
                * Generated
                */
               FloatHeapDataPacked(const PersistentRecords& persistentRecords);
               
               /**
                * Generated
                */
               FloatHeapDataPacked(const float& u);
               
               /**
                * Generated
                */
               ~FloatHeapDataPacked();
               
               /**
                * Generated
                */
                float getU() const ;
               
               /**
                * Generated
                */
                void setU(const float& u) ;
               
               /**
                * Generated
                */
               std::string toString() const;
               
               /**
                * Generated
                */
               void toString(std::ostream& out) const;
               
               
               PersistentRecords getPersistentRecords() const;
               /**
                * Generated
                */
               FloatHeapData convert() const;
               
               
            #ifdef Parallel
               protected:
                  static tarch::logging::Log _log;
                  
               public:
                  
                  /**
                   * Global that represents the mpi datatype.
                   * There are two variants: Datatype identifies only those attributes marked with
                   * parallelise. FullDatatype instead identifies the whole record with all fields.
                   */
                  static MPI_Datatype Datatype;
                  static MPI_Datatype FullDatatype;
                  
                  /**
                   * Initializes the data type for the mpi operations. Has to be called
                   * before the very first send or receive operation is called.
                   */
                  static void initDatatype();
                  
                  static void shutdownDatatype();
                  
                  /**
                   * @param communicateSleep -1 Data exchange through blocking mpi
                   * @param communicateSleep  0 Data exchange through non-blocking mpi, i.e. pending messages are received via polling until MPI_Test succeeds
                   * @param communicateSleep >0 Same as 0 but in addition, each unsuccessful MPI_Test is follows by an usleep
                   */
                  void send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, int communicateSleep);
                  
                  void receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, int communicateSleep);
                  
                  static bool isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
                  
                  #endif
                     
                  };
                  
                  #endif
                  
