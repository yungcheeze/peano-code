// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_HEAP_H_
#define _PEANO_HEAP_HEAP_H_

#include <map>
#include <vector>
#include <list>

#include "peano/utils/PeanoOptimisations.h"

#include "peano/heap/SendReceiveTask.h"
#include "peano/heap/SynchronousDataExchanger.h"
#include "peano/heap/BoundaryDataExchanger.h"

#include "tarch/logging/Log.h"
#include "tarch/services/Service.h"




/**
 * @todo Rename weil packed ja was anderes ist bei Peano
 *
 * @todo einmal erst einschalten so, damit man sieht, was nicht uebersetzt
 */
//#ifndef noPackedEmptyHeapMessages
//  #define PackedEmptyHeapMessages
//#endif



#ifdef ParallelExchangePackedRecordsInHeaps
   #pragma pack (push, 1)
#endif


namespace peano {
  namespace heap {
    template<
      class Data,
      class MasterWorkerExchanger  = SynchronousDataExchanger< Data >,
      class JoinForkExchanger      = SynchronousDataExchanger< Data >,
      class NeighbourDataExchanger = BoundaryDataExchanger< Data >
    >
    class Heap;

    /**
     * Flags to specify which kind of message is sent or
     * received.
     */
    enum MessageType {
      NeighbourCommunication,
      ForkOrJoinCommunication,
      MasterWorkerCommunication
    };
  }
}




/**
 * Heap Singleton - Single Point of Contact for Heap Management
 *
 * HeapData is a template class that manages the storage of
 * vertex data on the heap. The vertex data class has to be generated
 * by DaStGen to provide MPI methods for sending and receiving. The idea of
 * heaps in Peano is that you model your vertex to have one integer index
 * per associated heap object with arbitrary cardinality. Whenever you
 * create a vertex, you ask the heap for a new index and store this index
 * within the vertex. Afterwards, you befill the heap with data.
 *
 * A similar reasoning holds for cells as well.
 *
 * Since a heap is kind of a global thing, this class is a singleton. Please
 * consult init() when you use the heap.
 *
 * !!! MPI Handling
 *
 * The elements stored on the heap have to be modeled due to DaStGen. As a
 * result, we can send individual elements as well as sequences of classes
 * away as MPI data types. This is done whenever the user calls send on a
 * specific heap element, i.e. the user calls send with a heap element index
 * and the heap then sends away all elements associated to this heap. This way,
 * the user has full control which heap elements to distribute to which nodes.
 * However, no heap data is exchanged automatically - you have to do this within
 * the mapping's prepare send and merge operations.
 *
 * If an adapter sends or received data, it has first to inform the heap that
 * is will communicate due to MPI. For this, you have to call
 * startToSendOrReceiveHeapData() - typically in beginIteration(). At the end
 * of the iteration, you have to call finishedToSendOrReceiveHeapData().
 *
 * A send operation triggered due to sendData() for a given heap element is
 * a process with three steps. First, the data is copied into a temporary array
 * and meta information is assembled. Then, the meta information is sent away
 * due to a blocking send. Meta data is modeled by
 * \code
\include dastgen/MetaInformation.def
\endcode (see struct MetaInformation). Then, a non-blocking send for the actual
 * data is launched and the send object is pushed on a stack _sendTasks. When
 * one grid traversal terminates, all send processes are finalised. After an
 * iteration, no send operations are dangling anymore, and all data has left
 * the current MPI node.
 *
 * The receive operation is more complex: Peano runs through the spacetree grid
 * forth and back, i.e. the traversal direction of the automaton is inverted
 * after each iteration. As a consequence, we may not simply trigger receive
 * calls when we need data from the heap, as receives in a subsequent traversal
 * have a mirrored order. Instead, the heap has to receive data from a given
 * node in the background until all data has been received. The received data
 * has to be stored in a temporary buffer that we call receive buffer. When all
 * data is available, the read order is the inverse of the receive order, i.e.
 * data from the receive buffer is deployed to the receiving tree automaton in
 * reserve order.
 *
 * To be able to identify whether all data is received, we assume that the
 * number of outgoing messages equals the number of incoming messages. That
 * does however not mean that the cardinalities of the individual messages
 * are the same. We also do not enforce that the data sent in one step is
 * received in the next step. Several additional adapter runs might be
 * triggered before data actually is used due to receiveData() calls. However,
 * we do not support that multiple send traversals may follow each other
 * without receive traversals in-between. The earliest time when you may send
 * heap data again after you've sent out heap data before is the grid traversal
 * where you also receive data.
 *
 * !!! Efficiency note
 *
 * If a lot of heap data is exchanged, the asynchronous exchange of information
 * can block all MPI buffers. In this case, it is useful to call
 * receiveDanglingMessages() from time to time manually within your mappings.
 *
 * !!! Heap data exchange throughout forks, joins, and master-worker exchange
 *
 * The arguing above (with the inversed read order in most cases) does not hold
 * for the forks and joins, and it also does not hold if a master sends heap
 * data to a worker or the other way round. Here, all data is sent-received
 * in-order and synchronous. For these special cases, all communication is
 * deployed to a separate tag to avoid confusion. You switch to this mode due
 * to the synchronous flag of the send and receive operation.
 *
 * !!! Multithreading
 *
 * The heap class is not thread-safe and does not provide any threading
 * facilities. If you use it within a multithreaded application, you have to
 * ensure all the data consistency - either via semaphores or an a priori
 * exclusion of races.
 *
 * !!! Troubleshooting
 *
 * - Is your receiving mapping also calling startToSendOrReceiveHeapData()?
 * - Does each node send exactly the same number of vertices to the other nodes
 *   that it wants to receive? The size of the heap records doesn't have to be
 *   the same but each vertex that wants to receive data also has to send a
 *   (probably empty) record the the corresponding neighbour.
 *
 * !!! Rationale
 *
 * SynchronousDataExchange hier ist nix symmetrisch. Deshalb koennen wir keine map verwenden - da wuerden uns daten entgehen
 * Stattdessen muessen wir all ranks testen und einfuegen
 *
 * Boundary ist immer symmetrisch
 *
 * @author Kristof Unterweger, Tobias Weinzierl
 */
template <class Data, class MasterWorkerExchanger, class JoinForkExchanger, class NeighbourDataExchanger>
class peano::heap::Heap: public tarch::services::Service {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    typedef std::map<int, std::vector<Data>*>  HeapContainer;

    /**
     * Map that holds all data that is stored on the heap
     * via this class.
     */
    HeapContainer _heapData;

    std::list<int>   _freedHeapIndices;

    /**
     * Stores the next available index. By now the indices
     * are generated in a linear order, so no attention is
     * payed by now to fragmentation of the index space by
     * deleting data.
     */
    int _nextIndex;

    #ifdef Parallel
    int                                    _neighbourDataExchangerTag;

    MasterWorkerExchanger                  _masterWorkerExchanger;
    JoinForkExchanger                      _joinForkExchanger;
    std::map<int, NeighbourDataExchanger>  _neighbourDataExchanger;
    #endif

    /**
     * Stores the maximum number of heap objects that was stored
     * in this object at any time.
     */
    int _maximumNumberOfHeapEntries;

    /**
     * Stores the number of heap objects that have been allocated within
     * this object during the program's runtime.
     */
    int _numberOfHeapAllocations;

    int _numberOfHeapFrees;

    /**
     * Name for this heap object. Used for plotting statistics.
     */
    std::string _name;

    // @todo raus
    #ifdef PackedEmptyHeapMessages
    /**
     * Stores the number of zero-length messages that should be compressed
     * into one message before the next non-zero-length message is sent.
     *
     * For each send-call with a zero-length message this counter is increased
     * and no message is sent. As soon as a non-zero-length message should be
     * sent or the current iteration ends, this counter is sent as one message
     * and the counter is reset to zero.
     *
     * RLE of zero-length messages for fork/join or master-worker-communication
     * is not implemented, since synchronous communication implies the risk of
     * deadlocks. Here, zero-length messages may be held back until after a
     * message that, logically, should show up afterwards.
     */
    std::map<int, int> _numberOfNeighbourDataCompressedEmptyMessages;
    #endif

    /**
     * Private constructor to hide the possibility
     * to instantiate an object of this class.
     */
    Heap();

    /**
     * Private destructor to free the MPI datatypes.
     */
    ~Heap();


//    std::vector< Data > receiveNeighbourData(
//      int fromRank,
//      const tarch::la::Vector<DIMENSIONS, double>&  position,
//      int                                           level
//    );

//    /**
//     *
//     * !!! Implementation remarks
//     *
//     * The implementation is somehow redundant as we could skip the first if
//     * statement and jump directly into the while loop. However, this routine
//     * is very sensitiv to (software) latency and thus we avoid the deadlock
//     * timing, e.g., if the message is already available. Only if the message
//     * isn't available yet, we do the standard polling/deadlock check loop.
//     */
//    std::vector< Data > receiveMasterWorkerOrForkJoinData(
//      int fromRank,
//      const tarch::la::Vector<DIMENSIONS, double>&  position,
//      int                                           level,
//      MessageType                                   messageType
//    );

//    void receiveDanglingMessages(int tag, std::vector<SendReceiveTask<Data> >& taskQueue);

    /**
     * Returns the correct MPI tag for the given message type.
     *
     * Operation is not const as it updates internal statistics.
     */
//    int getTagForMessageType(MessageType messageType, bool isSendTask, int messageSize);
//
//    /**
//     * Compresses one zero-length message for sending it later.
//     */
//    void compressZeroLengthMessage(
//      int toRank
//    );

//    /**
//     * Sends all zero-length messages that have been accumulated for the given message
//     * type and resets the according counter.
//     */
//    void sendCompressedEmptyMessages(
//      int                                           toRank,
//      const tarch::la::Vector<DIMENSIONS, double>&  position,
//      int                                           level
//    );

    /**
     * Sends all zero-length messages that have not been sent, yet.
     */
//    void sendAllCompressedEmptyMessages();
  public:
    /**
     * The HeapData class is a singleton, thus one needs to
     * use the getInstance() method to retrieve the single
     * instance of this class.
     */
    static Heap& getInstance();

    /**
     * Retrieves the data that corresponds to the given index.
     */
    std::vector<Data>& getData(int index);

    /**
     * Retrieves the data that corresponds to the given index.
     */
    const std::vector<Data>& getData(int index) const;

    /**
     * Creates new data on the heap and returns the
     * corresponding index.
     *
     * @param numberOfEntries If you know a priori how many elements you'll
     *        gonna store for this heap entry, use this constructor. You can
     *        always add more elements later, but using this parameter should
     *        be faster and leads to a lower memory fragmentation
     * @return The index return is always a non-negativ number.
     */
    int createData(int numberOfEntries=0);

    /**
     * Returns, if the given index is a known index and, thus,
     * refers to a valid heap data object.
     */
    bool isValidIndex(int index) const;

    /**
     * Deletes the data with the given index and remove index from heap.
     * First, all data associated to this index is cleared. Afterwards, we
     * clear the heap entry. The user however is responsible not to use
     * index anymore. This is important, as the heap might decide to reuse
     * index already for the next createData() call.
     */
    void deleteData(int index);

    /**
     * Deletes all data that has been allocated by the application
     * by means of the createData() method.
     */
    void deleteAllData();

    /**
     * Returns the number of entries being held by this object.
     */
    int getNumberOfAllocatedEntries() const;

    /**
     * Takes data from fromIndex and reassings it to toIndex.
     *
     * If toIndex already points to data, the data from fromIndex is appended
     * to the original data, i.e. no data is erased at toIndex. The operation
     * does not remove fromIndex though fromIndex will afterwards identify an
     * empty data container.
     */
    void moveData( int toIndex, int fromIndex );

    /**
     * This method discards all heap data and prepares the HeapData
     * management to handle new data requests. After calling this
     * method all indices retrieved earlier are invalid.
     */
    void restart();

    /**
     * Shuts down the memory management for heap data and frees all allocated
     * memory.
     *
     * There is a slight difference between
     * terminate() and shutdown(). The counterpart of shutdown() is init() and
     * both operations are called once throughout the whole application
     * lifetime. In contrast, terminate() and restart() are called several
     * times.
     */
    void shutdown();

    /**
     * Assign the heap a name (identifier) such that messages from the heap can
     * be assigned to the heap type. In particular important if you handle
     * multiple heaps, so you can distinguish error messages, e.g.
     */
    void setName(std::string name);

    /**
     * Sends heap data associated to one index to one rank.
     *
     * Please note that these sends are assynchronous, i.e. if you change the
     * vertices afterwards, you might run into problems
     */
    void sendData(
      int                                           index,
      int                                           toRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    void sendData(
      const std::vector<Data>&                      data,
      int                                           toRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    /**
     * Receive heap data associated to one index from one rank.
     *
     * Wrapper forwarding to the other receiveData() operation with default
     * values. Operation should be used in release mode, as all additional
     * attributes of the overloaded receiveData() operation are used for
     * validation purposes.
     *
     * !!! Rationale
     *
     * Though the operation only deploys data that has been received before, it
     * is not const as it frees data of the local buffers.
     *
     * This operation is not const, as local handles might be deleted
     * afterwards.
     *
     * @see Heap
     * @see receiveData(int)
     */
    std::vector< Data > receiveData(
      int                                           fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    /**
     * Receive data and append it to local data.
     *
     * @see receiveData()
     * @return Number of appended entries
     */
    int receiveData(
      int                                           index,
      int                                           fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    /**
     * @see Heap
     */
    virtual void receiveDanglingMessages();

    std::string toString() const;

    /**
     * Start to send data
     *
     * Please hand in the state's traversal bool that informs the heap about
     * the direction of the Peano space-filling curve. This operation is
     * typically called in beginIteration(). However, please be aware that the
     * data from the master is received and merged before.
     *
     * This operation is idempotent, i.e. you may call it several times.
     *
     * This operation is to be re-called in each traversal and should be
     * followed by a finishedToSendData() call in the each traversal as well.
     *
     * You are not allowed to send heap data before this operation has been
     * called.
     */
    void startToSendData(bool isTraversalInverted);

    /**
     * Stop to send data
     *
     * Counterpart of startToSendData(). Should be called around in each
     * traversal where you've also called startToSendData(). When this
     * operation is called, you are not allowed to send heap data anymore.
     *
     * This operation runs through all sent master-worker and join-fork
     * messages and waits for each sent
     * message until the corresponding non-blocking MPI request is freed, i.e.
     * until the message has left the system. As the underlying MPI_Test
     * modifies the MPI handles, the operation is not const. The method also
     * has some deadlock detection.
     *
     * This operation is idempotent, i.e. you may call it several times.
     *
     * !!! Frequently Done Bug
     *
     * Please note that the event prepareSendToMaster() is invoked by Peano
     * after endIteration() is called. Many heap users send data to the master
     * in prepareSendToMaster(). For such codes, you may not call
     * finishedToSendData() in endIteration(). Instead, you have to notify the
     * heap that you finished to send in prepareSendToMaster().
     *
     * This induces an error on the global master where prepareSendToMaster()
     * is never called. So, you have to call finish in endIteration() as well -
     * but if and only if you are on the global master.
     *
     * @see releaseSentMessages()
     */
    void finishedToSendData();

    /**
     * Plots statistics for this heap data.
     */
    void plotStatistics() const;

    void clearStatistics();
};


#ifdef ParallelExchangePackedRecordsInHeaps
#pragma pack (pop)
#endif


#include "peano/heap/Heap.cpph"

#endif
