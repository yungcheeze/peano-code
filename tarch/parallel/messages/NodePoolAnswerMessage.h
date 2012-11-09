#ifndef _TARCH_PARALLEL_MESSAGES_NODEPOOLANSWERMESSAGE_H
#define _TARCH_PARALLEL_MESSAGES_NODEPOOLANSWERMESSAGE_H

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

namespace tarch {
   namespace parallel {
      namespace messages {
         class NodePoolAnswerMessage;
         class NodePoolAnswerMessagePacked;
      }
   }
}

/**
 * @author This class is generated by DaStGen
 * 		   DataStructureGenerator (DaStGen)
 * 		   2007-2009 Wolfgang Eckhardt
 * 		   2012      Tobias Weinzierl
 *
 * 		   build date: 17-10-2012 11:04
 *
 * @date   17/10/2012 11:05
 */
class tarch::parallel::messages::NodePoolAnswerMessage { 
   
   public:
      
      typedef tarch::parallel::messages::NodePoolAnswerMessagePacked Packed;
      
      struct PersistentRecords {
         int _newWorker;
         /**
          * Generated
          */
         PersistentRecords();
         
         /**
          * Generated
          */
         PersistentRecords(const int& newWorker);
         
         /**
          * Generated
          */
          int getNewWorker() const ;
         
         /**
          * Generated
          */
          void setNewWorker(const int& newWorker) ;
         
         
      };
      
   private: 
      PersistentRecords _persistentRecords;
      
   public:
      /**
       * Generated
       */
      NodePoolAnswerMessage();
      
      /**
       * Generated
       */
      NodePoolAnswerMessage(const PersistentRecords& persistentRecords);
      
      /**
       * Generated
       */
      NodePoolAnswerMessage(const int& newWorker);
      
      /**
       * Generated
       */
      virtual ~NodePoolAnswerMessage();
      
      /**
       * Generated
       */
       int getNewWorker() const ;
      
      /**
       * Generated
       */
       void setNewWorker(const int& newWorker) ;
      
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
      NodePoolAnswerMessagePacked convert() const;
      
      
   #ifdef Parallel
      protected:
         static tarch::logging::Log _log;
         
         int _senderRank;
         
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
         
         void send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
         
         void receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
         
         static bool isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
         
         int getSenderRank() const;
         
   #endif
      
   };
   
   /**
    * @author This class is generated by DaStGen
    * 		   DataStructureGenerator (DaStGen)
    * 		   2007-2009 Wolfgang Eckhardt
    * 		   2012      Tobias Weinzierl
    *
    * 		   build date: 17-10-2012 11:04
    *
    * @date   17/10/2012 11:05
    */
   class tarch::parallel::messages::NodePoolAnswerMessagePacked { 
      
      public:
         
         struct PersistentRecords {
            int _newWorker;
            /**
             * Generated
             */
            PersistentRecords();
            
            /**
             * Generated
             */
            PersistentRecords(const int& newWorker);
            
            /**
             * Generated
             */
             int getNewWorker() const ;
            
            /**
             * Generated
             */
             void setNewWorker(const int& newWorker) ;
            
            
         };
         
      private: 
         PersistentRecords _persistentRecords;
         
      public:
         /**
          * Generated
          */
         NodePoolAnswerMessagePacked();
         
         /**
          * Generated
          */
         NodePoolAnswerMessagePacked(const PersistentRecords& persistentRecords);
         
         /**
          * Generated
          */
         NodePoolAnswerMessagePacked(const int& newWorker);
         
         /**
          * Generated
          */
         virtual ~NodePoolAnswerMessagePacked();
         
         /**
          * Generated
          */
          int getNewWorker() const ;
         
         /**
          * Generated
          */
          void setNewWorker(const int& newWorker) ;
         
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
         NodePoolAnswerMessage convert() const;
         
         
      #ifdef Parallel
         protected:
            static tarch::logging::Log _log;
            
            int _senderRank;
            
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
            
            void send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
            
            void receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
            
            static bool isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise);
            
            int getSenderRank() const;
            
      #endif
         
      };
      
      #endif
      
