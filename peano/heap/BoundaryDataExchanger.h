// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_BOUNDARY_DATA_EXCHANGER_H_


namespace peano {
  namespace heap {
    template<class Data>
    class BoundaryDataExchanger;
  }
}


template<class Data>
class peano::heap::BoundaryDataExchanger {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

  protected:
    const std::string    _identifier;
    const int            _tag;
    const int            _rank;

    std::vector<SendReceiveTask<Data> >   _sendTasks;
    std::vector<SendReceiveTask<Data> >   _receiveTasks[2];

    int  _numberOfSentMessages;
    int  _numberOfSentRecords;
    int  _numberOfReceivedMessages;
    int  _numberOfReceivedRecords;

    /**
     * Is either 0 or 1 and identifies which element of _receiveDeployTasks
     * currently is the receive buffer and which one is the deploy buffer.
     */
    int _currentReceiveBuffer;


    /**
     * Shall the deploy buffer data be read in reverse order?
     *
     * Usually, the elements of the deploy buffer are delivered in reverse
     * order compared to the order they are received. See the class
     * documentation for the rationale and a reasoning. That means that this
     * flag usually is set. However, if you send out data in an adapter, then
     * do an iteration without communication, and then receive the data from
     * the heap, this flag has to be set to false.
     *
     * As a consequence, this flag is by default true. If you finishedToSendOrReceiveHeapData()
     * and the content of the receive buffer is moved to the buffer (due
     * to a transition of _currentReceiveBuffer to 1 or 0, respectively), this
     * flag also is set true. If no switching is performed in
     * relesaseMessages() as the buffer still was filled but the receive
     * buffer was empty, solely this flag is inverted.
     */
    bool  _readDeployBufferInReverseOrder;

    bool  _wasTraversalInvertedThroughoutLastSendReceiveTraversal;

    #ifdef Asserts
    bool _isCurrentlySending;
    #endif

    /**
     * Wait until number of received messages equals sent messages
     *
     * The operation is a big while loop around receiveDanglingMessages() with
     * some deadlocking aspects added, i.e. it can time out. It is not const as
     * receiveDanglingMessages() cannot be const.
     *
     * Besides waiting for MPI to release some handles, the operation also
     * invokes all the services to receive any dangling messages.
     */
    void waitUntilNumberOfReceivedNeighbourMessagesEqualsNumberOfSentMessages(int numberOfMessagesSentThisIteration);

    /**
     * Switches receive and deploy buffer between two iterations.
     *
     * This method switches receive and deploy buffer for heap data
     * exchanged between neighbours. This happens between two iterations
     * in which heap data is exchanged and results in a situation where
     * the former receive buffer is the deploy buffer afterwards and vice
     * versa.
     *
     * Due to the polling mechanism in receiveDanglingMessages(...) it
     * might happen that the receive buffer before the switch contains
     * messages which belong to the following iteration, since a
     * neighbouring rank may already be an iteration ahead. These
     * messages need to reside in the new receive buffer (i.e. the
     * former deploy buffer). Thus, after switching both buffers the
     * information of the statistics map is used to copy all messages
     * that do not belong to the previous iteration to the new receive
     * buffer.
     *
     * !!! Realisation
     *
     * We assume that all message in the receive buffer are already unpacked/in
     * a state that we can directly extract them from the receive buffer. If
     * you use sophisticated packing/coding mechanisms, all data in the receive
     * buffer is assumed to be uncompressed. See receiveDanglingMessages() for
     * details. This operation is really based upon the actual value of the
     * field _receiveTasks[1-_currentReceiveBuffer].size().
     *
     * @param numberOfMessagesSentThisIteration The switch mechanism has to be
     *   know how many data have to be in the new receive buffer. If there are
     *   more messages available, those additional ones belong to next
     *   traversal already and have to be places back.
     */
    void switchReceiveAndDeployBuffer(int numberOfMessagesSentThisIteration);

    /**
     * Release Requests for Received Messages
     *
     * This operation runs through all received messages. For each one it
     * waits until the receive operation has been finished. The operation
     * basically should be const. However, it calls MPI_Test/MPI_Wait on the
     * request objects associated to the heap. This test modifies the request
     * object which makes the operation (basically via a callback) non-const.
     *
     * Besides waiting for MPI to release some handles, the operation also
     * invokes all the services to receive any dangling messages.
     */
    void releaseReceivedNeighbourMessagesRequests();

    /**
     * Release all sent messages
     *
     * This operation waits until all messages are successfully sent. Then, it
     * clears the send buffer.
     */
    void releaseSentMessages();

    /**
     * Exchanger has to know how many messages should be in the receive buffer at least.
     */
    virtual int getNumberOfSentMessages() const = 0;

    /**
     * Hook in for finishToSendData(). Is called when the abstract superclass
     * has finished its operatios.
     */
    virtual void postprocessFinishedToSendData() = 0;

    virtual void postprocessStartToSendData() = 0;

    /**
     * There has been a receive task in the mpi queue. This one is received and
     * then passed to this operation.
     */
    virtual void handleAndQueueReceivedTask( SendReceiveTask<Data> receivedTask ) = 0;

    /**
     * No mpi operation done yet.
     */
    virtual void handleAndQueueSendTask( SendReceiveTask<Data> sendTask, const std::vector<Data>& data ) = 0;
  public:
    BoundaryDataExchanger();

    /**
     *
     * !!! The parameter _isCurrentlySending
     *
     * _isCurrentlySending primarily acts as assertion. If we try to send
     * something while the flag is not set, Peano quits with an assertion.
     * This however is only half of the story.
     *
     * We have to know when a new traversal starts to be able to take all
     * data received, remove those that belong to the last iteration (while
     * we might already have received data from the current iteration), and
     * move them to the deploy buffer. For the deploy buffer, we have to know
     * whether to read it FIFO or LIFO. For this, we analyse the traversal
     * direction.
     *
     * For the very first iteration, we typically cannot invoke the start
     * communication operation: Even if we do, most buffers won't get this
     * information as they are not created yet. It is here absolutely
     * irrelevant that the heap communication channels have been opened
     * before - if we open the channel, no data is received yet and we will
     * not move data or analyse read directions. Therefore, the start property
     * of this assertion flag is true - we can start to send data right away.
     *
     * Den Assert Parameter unbedingt auf ja, sonst geht erste Iteration schief, weil da
     * ja Eintrag in Map noch nicht existiert
     *
     * @param identifier Only required by the plot
     */
    BoundaryDataExchanger(const std::string& identifier, int tag, int rank);

    virtual ~BoundaryDataExchanger();

    void startToSendData(bool isTraversalInverted);
    void finishedToSendData(bool isTraversalInverted);

    void receiveDanglingMessages();

    void sendData(
      const std::vector<Data>&                      data,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    std::vector< Data > receiveData(
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    void plotStatistics() const;
    void clearStatistics();
};


#ifdef Parallel
#include "peano/heap/BoundaryDataExchanger.cpph"
#endif

#endif
