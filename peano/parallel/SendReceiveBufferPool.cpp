#include "peano/parallel/SendReceiveBufferPool.h"


#include "tarch/parallel/Node.h"
#include "tarch/parallel/NodePool.h"
#include "tarch/Assertions.h"
#include "tarch/timing/Watch.h"
#include "tarch/multicore/Lock.h"

#include "peano/datatraversal/TaskSet.h"


#include <atomic>


#include "tarch/services/ServiceFactory.h"
registerService(peano::parallel::SendReceiveBufferPool)


tarch::logging::Log                                                              peano::parallel::SendReceiveBufferPool::_log( "peano::parallel::SendReceiveBufferPool" );


#ifdef Parallel
peano::parallel::SendReceiveBufferPool::SendReceiveBufferPool():
  _iterationManagementTag(MPI_ANY_TAG),
  _iterationDataTag(MPI_ANY_TAG),
  _bufferSize(0),
  _mode(SendAndDeploy) {
  _iterationManagementTag = tarch::parallel::Node::getInstance().reserveFreeTag("SendReceiveBufferPool[it-mgmt]");
  _iterationDataTag       = tarch::parallel::Node::getInstance().reserveFreeTag("SendReceiveBufferPool[it-data]");

  #if defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  peano::datatraversal::TaskSet spawnTask(_backgroundThread);
  #endif
}
#else
peano::parallel::SendReceiveBufferPool::SendReceiveBufferPool():
  _iterationManagementTag(-1),
  _iterationDataTag(-1),
  _bufferSize(0),
  _mode(SendAndDeploy) {
}
#endif


peano::parallel::SendReceiveBufferPool::~SendReceiveBufferPool() {
  #if defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  assertion1( _backgroundThread._state == BackgroundThread::Terminate, _backgroundThread.toString() );
  #endif

  for (std::map<int,SendReceiveBuffer*>::iterator p = _map.begin(); p!=_map.end(); p++ ) {
    std::cerr << "encountered open buffer for destination " << p->first << " on rank " << tarch::parallel::Node::getInstance().getRank() <<  ". Would be nicer to call terminate() on SendReceiveBufferPool." << std::endl;
    delete p->second;
  }
}


peano::parallel::SendReceiveBufferPool& peano::parallel::SendReceiveBufferPool::getInstance() {
  static peano::parallel::SendReceiveBufferPool singleton;
  return singleton;
}



std::string peano::parallel::SendReceiveBufferPool::toString( SendReceiveMode  mode) {
  switch (mode) {
    case SendAndDeploy:
      return "send-and-deploy";
    case DeployButDoNotSend:
      return "deploy-but-do-not-send";
    case SendButDoNotDeploy:
      return "send-but-do-not-deploy";
    case NeitherDeployNorSend:
      return "neither-deploy-nor-send";
  }

  return "undef";
}


int peano::parallel::SendReceiveBufferPool::getIterationManagementTag() const {
  #ifdef Parallel
  assertion( _iterationManagementTag!=MPI_ANY_TAG );
  #endif
  return _iterationManagementTag;
}


int peano::parallel::SendReceiveBufferPool::getIterationDataTag() const {
  #ifdef Parallel
  assertion( _iterationDataTag!=MPI_ANY_TAG );
  #endif
  return _iterationDataTag;
}


void peano::parallel::SendReceiveBufferPool::receiveDanglingMessages() {
  SCOREP_USER_REGION("peano::parallel::SendReceiveBufferPool::receiveDanglingMessages()", SCOREP_USER_REGION_TYPE_FUNCTION)

  #if !defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  receiveDanglingMessagesFromAllBuffersInPool();
  #else
  if (BackgroundThread::_state==BackgroundThread::State::Suspend) {
    tarch::multicore::Lock lock(BackgroundThread::_semaphore);
    receiveDanglingMessagesFromAllBuffersInPool();
  }
  else {
    tarch::multicore::BooleanSemaphore::sendTaskToBack();
  }
  #endif
}


void peano::parallel::SendReceiveBufferPool::receiveDanglingMessagesFromAllBuffersInPool() {
  for (std::map<int,SendReceiveBuffer*>::iterator p = _map.begin(); p!=_map.end(); p++ ) {
    logDebug( "receiveDanglingMessagesFromAllBuffersInPool()", "receive data from rank " << p->first << " in mode " << toString(_mode) );
    p->second->receivePageIfAvailable();
  }
}


void peano::parallel::SendReceiveBufferPool::terminate() {
  #if defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  _backgroundThread.switchState(BackgroundThread::State::Terminate);
  #endif

  for (std::map<int,SendReceiveBuffer*>::iterator p = _map.begin(); p!=_map.end(); p++ ) {
    assertion1(  p->first >= 0, tarch::parallel::Node::getInstance().getRank() );
    assertion1( _map.count(p->first) == 1, tarch::parallel::Node::getInstance().getRank() );
    assertionEquals2( p->second->getNumberOfReceivedMessages(), 0, p->first, tarch::parallel::Node::getInstance().getRank() );
    delete p->second;
  }

  _map.clear();
}


void peano::parallel::SendReceiveBufferPool::restart() {
  assertion1( _map.empty(), tarch::parallel::Node::getInstance().getRank() );
}


void peano::parallel::SendReceiveBufferPool::releaseMessages() {
  SCOREP_USER_REGION("peano::parallel::SendReceiveBufferPool::releaseMessages()", SCOREP_USER_REGION_TYPE_FUNCTION)

  logTraceInWith1Argument( "releaseMessages()", toString(_mode) );

  #if defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  _backgroundThread.switchState(BackgroundThread::State::Suspend);
  #endif


  std::map<int,SendReceiveBuffer*>::iterator p = _map.begin();
  while (  p != _map.end() ) {
    p->second->releaseSentMessages();
    if ( p->second->getNumberOfSentMessages()==0 ) {
      logInfo( "releaseMessages()", "no message have been sent out, so remove buffer" );
      p = _map.erase(p);
    }
    else {
      p++;
    }
  }

  for ( std::map<int,SendReceiveBuffer*>::const_reverse_iterator p = _map.rbegin(); p != _map.rend(); p++ ) {
    p->second->releaseReceivedMessages(true);
  }

  #if defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  _backgroundThread.switchState(BackgroundThread::State::ReceiveDataInBackground);
  #endif

  switch (_mode) {
    case SendAndDeploy:
      break;
    case DeployButDoNotSend:
      _mode = NeitherDeployNorSend;
      break;
    case SendButDoNotDeploy:
      _mode = SendAndDeploy;
      break;
    case NeitherDeployNorSend:
      break;
  }

  logTraceOutWith1Argument( "releaseMessages()", toString(_mode) );
}


void peano::parallel::SendReceiveBufferPool::setBufferSize( int bufferSize ) {
  #ifdef Parallel
  assertion1( _map.empty(), tarch::parallel::Node::getInstance().getRank() );
  assertion2( bufferSize>0, bufferSize, tarch::parallel::Node::getInstance().getRank() );

  _bufferSize = bufferSize;
  #endif
}


tarch::multicore::BooleanSemaphore                               peano::parallel::SendReceiveBufferPool::BackgroundThread::_semaphore;
peano::parallel::SendReceiveBufferPool::BackgroundThread::State  peano::parallel::SendReceiveBufferPool::BackgroundThread::_state(peano::parallel::SendReceiveBufferPool::BackgroundThread::State::Suspend);


void peano::parallel::SendReceiveBufferPool::BackgroundThread::operator()() {
  #if !defined(SEND_RECEIVE_BUFFER_POOL_USES_BACKGROUND_THREAD_TO_RECEIVE_DATA)
  assertionMsg( false, "not never enter this operator" );
  #endif

  bool terminate = false;

  while (!terminate) {

    switch (_state) {
      case State::ReceiveDataInBackground:
       {
          tarch::multicore::Lock lock(_semaphore);

          SendReceiveBufferPool::getInstance().receiveDanglingMessagesFromAllBuffersInPool();

          // A release fence prevents the memory reordering of any read or write which precedes it in program order with any write which follows it in program order.
          //std::atomic_thread_fence(std::memory_order_release);
	     }
        break;
      case State::Suspend:
        break;
      case State::Terminate:
        terminate = true;
        break;
    }
    tarch::multicore::BooleanSemaphore::sendTaskToBack();
  }
}


std::string peano::parallel::SendReceiveBufferPool::BackgroundThread::toString() const {
  switch (_state) {
    case State::ReceiveDataInBackground:
      return "receive-data-in-background";
    case State::Suspend:
      return "suspend";
    case State::Terminate:
      return "terminate";
  }

  return "<undef>";
}


void peano::parallel::SendReceiveBufferPool::BackgroundThread::switchState(State newState ) {
  logTraceInWith1Argument( "switchState(State)", toString() );
  tarch::multicore::Lock lock(_semaphore);

  assertion1( _state != BackgroundThread::State::Terminate, toString() );

  _state = newState;
  logTraceOutWith1Argument( "switchState(State)", toString() );
}


void peano::parallel::SendReceiveBufferPool::exchangeBoundaryVertices(bool value) {
  logTraceInWith2Arguments( "exchangeBoundaryVertices(bool)", toString(_mode), value );
  switch (_mode) {
    case SendAndDeploy:
      if (value) {
        _mode = SendAndDeploy;
      }
      else {
        _mode = DeployButDoNotSend;
      }
      break;
    case DeployButDoNotSend:
      assertionMsg( false, "mode should not be set in-between two iterations" );
      break;
    case SendButDoNotDeploy:
      assertionMsg( false, "mode should not be set in-between two iterations" );
      break;
    case NeitherDeployNorSend:
      if (value) {
        _mode = SendButDoNotDeploy;
      }
      else {
        _mode = NeitherDeployNorSend;
      }
      break;
  }
  logTraceOutWith1Argument( "exchangeBoundaryVertices(bool)", toString(_mode) );
}


bool peano::parallel::SendReceiveBufferPool::deploysValidData() const {
  return _mode==SendAndDeploy || _mode==DeployButDoNotSend;
}
