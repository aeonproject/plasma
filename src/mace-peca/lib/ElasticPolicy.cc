#include "ElasticPolicy.h"
#include "ContextMapping.h"
#include "ContextBaseClass.h"
#include "ContextOwnership.h"
#include "eMonitor.h"

const uint8_t mace::ElasticityRule::ACTOR_RULE;
const uint8_t mace::ElasticityRule::RESOURCE_RULE;
const uint8_t mace::ElasticityRule::SLA_ACTOR_MARKER_RULE;
const uint8_t mace::ElasticityRule::INIT_PLACEMENT;

/*****************************class EventAccessInfo**********************************************/
void mace::EventAccessInfo::serialize(std::string& str) const{
  mace::serialize( str, &eventType );
  mace::serialize( str, &isTargetContext );
  
  mace::serialize( str, &totalFromEventCount );
  mace::serialize( str, &totalToEventCount );
  
  mace::serialize( str, &totalEventExecuteTime );
  
  mace::serialize( str, &fromContextCounts );
  mace::serialize( str, &fromMessagesSize );

  mace::serialize( str, &toContextCounts );
}

int mace::EventAccessInfo::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &eventType );
  serializedByteSize += mace::deserialize( is, &isTargetContext   );

  serializedByteSize += mace::deserialize( is, &totalFromEventCount   );
  serializedByteSize += mace::deserialize( is, &totalToEventCount   );

  serializedByteSize += mace::deserialize( is, &totalEventExecuteTime   );

  serializedByteSize += mace::deserialize( is, &fromContextCounts   );
  serializedByteSize += mace::deserialize( is, &fromMessagesSize   );

  serializedByteSize += mace::deserialize( is, &toContextCounts   );
  return serializedByteSize;
}

void mace::EventAccessInfo::addFromEventAccess(const uint64_t execute_time, const mace::string& create_ctx_name, 
    const uint64_t& msg_size ) {
  totalFromEventCount ++;
  totalEventExecuteTime += execute_time;

  if( fromContextCounts.find(create_ctx_name) == fromContextCounts.end() ) {
    fromContextCounts[create_ctx_name] = 1;
    fromMessagesSize[create_ctx_name] = msg_size;
  } else {
    fromContextCounts[create_ctx_name] ++;
    fromMessagesSize[create_ctx_name] += msg_size;
  }
}

void mace::EventAccessInfo::addToEventAccess( const mace::string& to_ctx_name ) {
  totalToEventCount ++;
  
  if( toContextCounts.find(to_ctx_name) == toContextCounts.end() ) {
    toContextCounts[to_ctx_name] = 1;
  } else {
    toContextCounts[to_ctx_name] ++;
  }
}

mace::map< mace::string, uint64_t > mace::EventAccessInfo::getFromEventAccessCount( const mace::string& context_type ) const {
  mace::map< mace::string, uint64_t > ctx_eaccess_counts;

  for( mace::map< mace::string, uint64_t >::const_iterator iter = fromContextCounts.begin(); iter != fromContextCounts.end(); 
      iter ++ ) {

    if( Util::isContextType(iter->first, context_type) ) {
      ctx_eaccess_counts[ iter->first ] = iter->second;
    }
  }
  return ctx_eaccess_counts;
}

/*****************************class ContetxtInteractionInfo**********************************************/
void mace::ContetxtInteractionInfo::serialize(std::string& str) const{
  mace::serialize( str, &callerMethodCount );
  mace::serialize( str, &calleeMethodCount );
}

int mace::ContetxtInteractionInfo::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &callerMethodCount   );
  serializedByteSize += mace::deserialize( is, &calleeMethodCount   );
  return serializedByteSize;
}

void mace::ContetxtInteractionInfo::addCallerContext( mace::string const& methodType, uint64_t count ) {
  if( callerMethodCount.find(methodType) == callerMethodCount.end() ){
    callerMethodCount[methodType] = count;
  } else {
    callerMethodCount[methodType] += count;
  }
}
  
void mace::ContetxtInteractionInfo::addCalleeContext( mace::string const& methodType, uint64_t count ) {
  if( calleeMethodCount.find(methodType) == calleeMethodCount.end() ){
    calleeMethodCount[methodType] = count;
  } else {
    calleeMethodCount[methodType] += count;
  }
}


/*****************************class EventRuntimeInfo**********************************************/
void mace::EventRuntimeInfo::serialize(std::string& str) const{
  mace::serialize( str, &eventId );
  mace::serialize( str, &eventMethodType );
  mace::serialize( str, &curEventExecuteTime );
  mace::serialize( str, &curEventExecuteTimestamp );
  mace::serialize( str, &isTargetContext );
  mace::serialize( str, &createContextName );
  mace::serialize( str, &messageSize );
}

int mace::EventRuntimeInfo::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &eventId );
  serializedByteSize += mace::deserialize( is, &eventMethodType   );
  serializedByteSize += mace::deserialize( is, &curEventExecuteTime   );
  serializedByteSize += mace::deserialize( is, &curEventExecuteTimestamp   );
  serializedByteSize += mace::deserialize( is, &isTargetContext   );
  serializedByteSize += mace::deserialize( is, &createContextName   );
  serializedByteSize += mace::deserialize( is, &messageSize   );
  return serializedByteSize;
}

/*****************************class ContextRuntimeInfo**********************************************/
mace::ContextRuntimeInfo& mace::ContextRuntimeInfo::operator=( mace::ContextRuntimeInfo const& orig ){
  contextName = orig.contextName;

  curPeriodStartTimestamp = orig.curPeriodStartTimestamp;
  curPeriodEndTimestamp = orig.curPeriodEndTimestamp;
  timePeriod = orig.timePeriod;

  markerStartTimestamp = orig.markerStartTimestamp;
  markerTotalTimeperiod = orig.markerTotalTimeperiod;
  markerTotalCount = orig.markerTotalCount;
  
  eventAccessInfos = orig.eventAccessInfos;
  
  contextInterInfos = orig.contextInterInfos;

  eventRuntimeInfos = orig.eventRuntimeInfos;

  coaccessContextsCount = orig.coaccessContextsCount;

  return *this;
}

void mace::ContextRuntimeInfo::serialize(std::string& str) const{
  mace::serialize( str, &contextName );

  mace::serialize( str, &curPeriodStartTimestamp );
  mace::serialize( str, &curPeriodEndTimestamp );
  mace::serialize( str, &timePeriod );

  mace::serialize( str, &markerStartTimestamp );
  mace::serialize( str, &markerTotalTimeperiod );
  mace::serialize( str, &markerTotalCount );

  mace::serialize( str, &eventAccessInfos );
  mace::serialize( str, &contextInterInfos );
  mace::serialize( str, &eventRuntimeInfos );
  mace::serialize( str, &coaccessContextsCount );
}

int mace::ContextRuntimeInfo::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &contextName );

  serializedByteSize += mace::deserialize( is, &curPeriodStartTimestamp );
  serializedByteSize += mace::deserialize( is, &curPeriodEndTimestamp   );
  serializedByteSize += mace::deserialize( is, &timePeriod   );

  serializedByteSize += mace::deserialize( is, &markerStartTimestamp );
  serializedByteSize += mace::deserialize( is, &markerTotalTimeperiod   );
  serializedByteSize += mace::deserialize( is, &markerTotalCount   );

  serializedByteSize += mace::deserialize( is, &eventAccessInfos   );
  serializedByteSize += mace::deserialize( is, &contextInterInfos  );
  serializedByteSize += mace::deserialize( is, &eventRuntimeInfos );

  serializedByteSize += mace::deserialize( is, &coaccessContextsCount );
  return serializedByteSize;
}

void mace::ContextRuntimeInfo::clear() {
  ScopedLock sl(runtimeInfoMutex);
  eventAccessInfos.clear();
  contextInterInfos.clear();
  eventRuntimeInfos.clear();
  coaccessContextsCount.clear();

  markerStartTimestamp.clear();
  markerTotalTimeperiod.clear();
  markerTotalCount.clear();
}

void mace::ContextRuntimeInfo::runEvent( const mace::Event& event ){
  ADD_SELECTORS("ContextRuntimeInfo::runEvent");
  ScopedLock sl(runtimeInfoMutex);
  mace::map<mace::OrderID, EventRuntimeInfo>::iterator iter = eventRuntimeInfos.find(event.eventId);
  if( iter == eventRuntimeInfos.end() ){
    EventRuntimeInfo eventRuntimeInfo(event.eventId, event.eventMethodType);

    if( this->contextName == event.target_ctx_name ){
      eventRuntimeInfo.isTargetContext = true;
    } else {
      eventRuntimeInfo.isTargetContext = false;
    }
    eventRuntimeInfo.createContextName = event.create_ctx_name;
    eventRuntimeInfos[event.eventId] = eventRuntimeInfo;
    // macedbg(1) << "Create EventRuntimeInfo for event("<< event.eventId <<") in context("<< this->contextName <<")!" << Log::endl;
  } 
  // contexts interaction
  if( this->contextName != event.target_ctx_name ){
    const mace::string& methodType = event.eventOpInfo.methodName;
    const mace::string& callerContext = event.eventOpInfo.fromContextName;

    if( contextInterInfos.find(callerContext) == contextInterInfos.end() ){
      ContetxtInteractionInfo ctxInterInfo;
      contextInterInfos[callerContext] = ctxInterInfo;
    }
    contextInterInfos[callerContext].addCallerContext( methodType, 1 );
    macedbg(1) << "Add routine("<< methodType <<") context("<< callerContext <<") -> context("<< contextName <<")!" << Log::endl;
  }
  
  eventRuntimeInfos[event.eventId].curEventExecuteTimestamp = TimeUtil::timeu();
}

void mace::ContextRuntimeInfo::addEventMessageSize( const mace::Event& event, const uint64_t msg_size ) {
  ADD_SELECTORS("ContextRuntimeInfo::addEventMessageSize");
  macedbg(1) << "event("<< event.eventId <<"): eventMethodType=" << event.eventMethodType << ", msgSize=" << msg_size << Log::endl; 
  ScopedLock sl(runtimeInfoMutex);
  mace::map<mace::OrderID, EventRuntimeInfo>::iterator iter = eventRuntimeInfos.find(event.eventId);
  if( iter == eventRuntimeInfos.end() ){
    EventRuntimeInfo eventRuntimeInfo(event.eventId, event.eventMethodType);

    if( this->contextName == event.target_ctx_name ){
      eventRuntimeInfo.isTargetContext = true;
    } else {
      eventRuntimeInfo.isTargetContext = false;
    }
    eventRuntimeInfo.createContextName = event.create_ctx_name;
    eventRuntimeInfos[event.eventId] = eventRuntimeInfo;
    // macedbg(1) << "Create EventRuntimeInfo for event("<< event.eventId <<") in context("<< this->contextName <<")!" << Log::endl;
  }

  eventRuntimeInfos[event.eventId].messageSize = msg_size;
}

void mace::ContextRuntimeInfo::stopEvent( const mace::OrderID& eventId ) {
  ADD_SELECTORS("ContextRuntimeInfo::stopEvent");
  // macedbg(1) << "To stop event " << eventId << " in context("<< this->contextName <<")" << Log::endl;
  ScopedLock sl(runtimeInfoMutex);
  if( eventRuntimeInfos.find(eventId) == eventRuntimeInfos.end()) {
    return;
  }
  eventRuntimeInfos[eventId].curEventExecuteTime += TimeUtil::timeu() - eventRuntimeInfos[eventId].curEventExecuteTimestamp;
}

void mace::ContextRuntimeInfo::addCalleeContext( mace::string const& calleeContext, mace::string const& methodType ){
  ScopedLock sl(runtimeInfoMutex);
  if( contextInterInfos.find(calleeContext) == contextInterInfos.end() ){
    ContetxtInteractionInfo ctxInterInfo;
    contextInterInfos[calleeContext] = ctxInterInfo;
  }
  contextInterInfos[calleeContext].addCalleeContext( methodType, 1 );
}
  
void mace::ContextRuntimeInfo::commitEvent( const mace::OrderID& eventId ) {
  ADD_SELECTORS("ContextRuntimeInfo::commitEvent");
  ScopedLock sl(runtimeInfoMutex);
  mace::map<mace::OrderID, EventRuntimeInfo>::iterator iter = eventRuntimeInfos.find(eventId);

  if( iter != eventRuntimeInfos.end() ){
    //macedbg(1) << "To delete EventRuntimeInfo for " << eventId << " in context("<< this->contextName <<")" << Log::endl;
    EventRuntimeInfo& eventRuntimeInfo = iter->second;
    if( !eventRuntimeInfo.isTargetContext ){
      return;
    }

    macedbg(1) << "Event("<< eventRuntimeInfo.eventMethodType <<"): context("<< eventRuntimeInfo.createContextName <<") -> context("<< contextName <<")!" << Log::endl;

    if( eventAccessInfos.find(eventRuntimeInfo.eventMethodType) == eventAccessInfos.end() ){
      EventAccessInfo eventAccessInfo( eventRuntimeInfo.eventMethodType, eventRuntimeInfo.isTargetContext );
      eventAccessInfo.addFromEventAccess(eventRuntimeInfo.curEventExecuteTime, eventRuntimeInfo.createContextName, 
        eventRuntimeInfo.messageSize);
      eventAccessInfos[eventRuntimeInfo.eventMethodType] = eventAccessInfo;
    } else {
      eventAccessInfos[eventRuntimeInfo.eventMethodType].addFromEventAccess(eventRuntimeInfo.curEventExecuteTime, 
        eventRuntimeInfo.createContextName, eventRuntimeInfo.messageSize );
    }

    eventRuntimeInfos.erase(iter);
  }
}

void mace::ContextRuntimeInfo::addCoaccessContext( mace::string const& context ) {
  ScopedLock sl(runtimeInfoMutex);
  if( coaccessContextsCount.find(context) == coaccessContextsCount.end() ) {
    coaccessContextsCount[ context ] = 1;
  } else {
    coaccessContextsCount[ context ] ++;
  }
}

void mace::ContextRuntimeInfo::addToEventAccess( const mace::string& method_type, const mace::string& to_ctx_name ) {
  ScopedLock sl(runtimeInfoMutex);
  if( eventAccessInfos.find(method_type) == eventAccessInfos.end() ) {
    EventAccessInfo info(method_type, false);
    eventAccessInfos[ method_type ] = info;
  } 
  eventAccessInfos[ method_type ].addToEventAccess(to_ctx_name);
}

uint64_t mace::ContextRuntimeInfo::getCPUTime() {
  ScopedLock sl(runtimeInfoMutex);
  uint64_t cpu_time = 0;
  for( mace::map<mace::string, EventAccessInfo>::const_iterator iter = eventAccessInfos.begin(); iter != eventAccessInfos.end(); 
      iter ++ ) {
    cpu_time += (iter->second).getTotalExecuteTime();
  }
  return cpu_time;
}

mace::string mace::ContextRuntimeInfo::getCoaccessContext( mace::string const& context_type ) {
  ScopedLock sl(runtimeInfoMutex);
  for( mace::map< mace::string, uint64_t >::const_iterator iter = coaccessContextsCount.begin(); iter != coaccessContextsCount.end(); 
      iter ++ ) {
    if( Util::isContextType(iter->first, context_type) && iter->second > mace::eMonitor::EVENT_COACCESS_THRESHOLD ) {
      return iter->first;
    }
  }
  return "";
}

uint32_t mace::ContextRuntimeInfo::getConnectionStrength( const mace::ContextMapping& snapshot, const mace::MaceAddr& addr ) {
  ScopedLock sl(runtimeInfoMutex);
  uint32_t strength = 0;
  for( mace::map<mace::string, ContetxtInteractionInfo>::const_iterator iter = contextInterInfos.begin(); iter != contextInterInfos.end();
      iter ++ ) {
    if( mace::ContextMapping::getNodeByContext(snapshot, iter->first) == addr ) {
      strength += (iter->second).getInterCount();
    }
  }
  return strength;
}

mace::vector<mace::string> mace::ContextRuntimeInfo::getInterctContextNames( mace::string const& context_type ) {
  ADD_SELECTORS("ContextRuntimeInfo::getInterctContextNames");
  ScopedLock sl(runtimeInfoMutex);
  mace::vector<mace::string> inter_ctx_names;
  for( mace::map<mace::string, ContetxtInteractionInfo>::const_iterator iter = contextInterInfos.begin(); iter != contextInterInfos.end();
      iter ++ ) {
    if( Util::isContextType(iter->first, context_type) ) {
      uint64_t inter_count = (iter->second).getInterCount();
      macedbg(1) << "context("<< iter->first <<") inter_count=" << inter_count << Log::endl;
      if( inter_count >= mace::eMonitor::INTER_CONTEXTS_STRONG_CONNECTED_THREAHOLD ) {
        inter_ctx_names.push_back( iter->first );
      }
    }
  }
  return inter_ctx_names;
}

mace::vector<mace::string> mace::ContextRuntimeInfo::getEventInterctContextNames( mace::string const& context_type ) {
  ADD_SELECTORS("ContextRuntimeInfo::getEventInterctContextNames");
  ScopedLock sl(runtimeInfoMutex);
  mace::vector<mace::string> inter_ctx_names;

  mace::map< mace::string, uint64_t> context_eaccess_counts;
  for( mace::map<mace::string, EventAccessInfo>::const_iterator iter = eventAccessInfos.begin(); iter != eventAccessInfos.end();
      iter ++ ) {

    mace::map< mace::string, uint64_t> ctx_eaccess_counts = (iter->second).getFromEventAccessCount(context_type);
    for( mace::map< mace::string, uint64_t>::iterator iter1 = ctx_eaccess_counts.begin(); iter1 != ctx_eaccess_counts.end(); iter1 ++ ) {
      if( context_eaccess_counts.find(iter1->first) == context_eaccess_counts.end() ){
        context_eaccess_counts[ iter1->first ] = iter1->second;
      } else {
        context_eaccess_counts[ iter1->first ] += iter1->second;
      }
    }
  }

  for( mace::map< mace::string, uint64_t>::iterator iter = context_eaccess_counts.begin(); iter != context_eaccess_counts.end(); 
      iter ++ ) {
    
    if( iter->second >= mace::eMonitor::INTER_CONTEXTS_STRONG_CONNECTED_THREAHOLD ) {
      inter_ctx_names.push_back( iter->first );
    }
  }

  return inter_ctx_names;
}

void mace::ContextRuntimeInfo::printRuntimeInformation(const uint64_t& total_ctx_execution_time, const double& server_cpu_usage, 
    const mace::ContextMapping& snapshot, const uint64_t& r ) {
  ADD_SELECTORS("ContextRuntimeInfo::printRuntimeInformation");

  double ctx_execution_time = (double)(this->getCPUTime());
  macedbg(1) << "Context("<< this->contextName <<") Round("<< r <<") execution time=" << ctx_execution_time << "; total execution time=" << total_ctx_execution_time << Log::endl;
  double ctx_cpu_usage = ( ctx_execution_time/total_ctx_execution_time ) * server_cpu_usage;

  mace::map< mace::MaceAddr, uint64_t > servers_comm_count;
  mace::map< mace::string, uint64_t > ctxs_event_size; 

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();
    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_access_count.begin(); iter2 != ctxes_event_access_count.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }
      mace::MaceAddr addr = mace::ContextMapping::getNodeByContext( snapshot, iter2->first);
      if( servers_comm_count.find(addr) == servers_comm_count.end() ) {
        servers_comm_count[addr] = iter2->second;
      } else {
        servers_comm_count[addr] += iter2->second;
      }
    }

    mace::map< mace::string, uint64_t> ctxes_event_message_size = (iter1->second).getFromEventMessageSize();
    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_message_size.begin(); iter2 != ctxes_event_message_size.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }
      if( ctxs_event_size.find(iter2->first) == ctxs_event_size.end() ) {
        ctxs_event_size[iter2->first] = iter2->second;
      } else {
        ctxs_event_size[iter2->first] += iter2->second;
      }
    }
  }

  macedbg(1) << "Context("<< this->contextName <<") runtime information: " << Log::endl;
  macedbg(1) << "Context CPU usage: " << ctx_cpu_usage << Log::endl;
  macedbg(1) << "Context interaction: " << servers_comm_count << Log::endl;
  macedbg(1) << "Context message size: " << ctxs_event_size << Log::endl;
}

void mace::ContextRuntimeInfo::markStartTimestamp( mace::string const& marker ) {
  ScopedLock sl(runtimeInfoMutex);
  markerStartTimestamp[ marker ] = TimeUtil::timeu();
}

void mace::ContextRuntimeInfo::markEndTimestamp( mace::string const& marker ) {
  ScopedLock sl(runtimeInfoMutex);
  if( markerStartTimestamp.find(marker) == markerStartTimestamp.end() ){
    return;
  }

  uint64_t time_period = TimeUtil::timeu() - markerStartTimestamp[marker];
  if( markerTotalTimeperiod.find(marker) == markerTotalTimeperiod.end() ){
    markerTotalTimeperiod[ marker ] = time_period;
    markerTotalCount[ marker ] = 1;
  } else {
    markerTotalTimeperiod[ marker ] += time_period;
    markerTotalCount[ marker ] += 1;
  }
} 

mace::map< mace::MaceAddr, uint64_t > mace::ContextRuntimeInfo::getServerInteractionCount( mace::ContextMapping const& snapshot ) {
  ScopedLock sl(runtimeInfoMutex);
  mace::map< mace::MaceAddr, uint64_t > servers_comm_count;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();
    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_access_count.begin(); iter2 != ctxes_event_access_count.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }
      mace::MaceAddr addr = mace::ContextMapping::getNodeByContext( snapshot, iter2->first);
      if( servers_comm_count.find(addr) == servers_comm_count.end() ) {
        servers_comm_count[addr] = iter2->second;
      } else {
        servers_comm_count[addr] += iter2->second;
      }
    }
  }

  for( mace::map<mace::string, ContetxtInteractionInfo>::iterator iter1 = contextInterInfos.begin(); iter1 != contextInterInfos.end();
      iter1 ++ ) {
    mace::MaceAddr addr = mace::ContextMapping::getNodeByContext( snapshot, iter1->first);
    mace::map<mace::string, uint64_t>& callee_count = (iter1->second).calleeMethodCount;
    for( mace::map<mace::string, uint64_t>::iterator iter2 = callee_count.begin(); iter2 != callee_count.end();
        iter2 ++ ) {
      if( servers_comm_count.find(addr) == servers_comm_count.end() ) {
        servers_comm_count[addr] = iter2->second;
      } else {
        servers_comm_count[addr] += iter2->second;
      } 
    }
  }
  return servers_comm_count;
}

mace::map< mace::string, uint64_t > mace::ContextRuntimeInfo::getEstimateContextsInteractionSize() {
  ADD_SELECTORS("ContextRuntimeInfo::getEstimateContextsInteractionSize");
  ScopedLock sl(runtimeInfoMutex);
  mace::map< mace::string, uint64_t > ctxs_inter_size;

  uint64_t total_remote_msg_size;
  uint64_t total_remote_event_count;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();
    mace::map< mace::string, uint64_t> ctxes_event_msg_size = (iter1->second).getFromEventMessageSize();

    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_msg_size.begin(); iter2 != ctxes_event_msg_size.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }
      if( iter2->second != 0 ) {
        total_remote_msg_size += iter2->second;
        total_remote_event_count += ctxes_event_access_count[ iter2->first ];
      }
    }
  }

  if( total_remote_event_count == 0 ){
    return ctxs_inter_size;
  }

  uint64_t avg_msg_size = (uint64_t)( total_remote_msg_size / total_remote_event_count );
  
  uint64_t estimated_remote_msg_count = 0;
  uint64_t estimated_remote_msg_size = 0;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();
    mace::map< mace::string, uint64_t> ctxes_event_msg_size = (iter1->second).getFromEventMessageSize();

    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_msg_size.begin(); iter2 != ctxes_event_msg_size.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }

      uint64_t msg_size = 0;
      if( iter2->second == 0 ) {
        msg_size = ctxes_event_access_count[ iter2->first ] * avg_msg_size;
        estimated_remote_msg_count += ctxes_event_access_count[ iter2->first ];
        estimated_remote_msg_size += msg_size;
      } else {
        msg_size = iter2->second;
      }

      if( ctxs_inter_size.find(iter2->first) == ctxs_inter_size.end() ) {
        ctxs_inter_size[iter2->first] = msg_size;
      } else {
        ctxs_inter_size[iter2->first] += msg_size;
      }
    }
  }

  macedbg(1) << "avg_msg_size=" << avg_msg_size << ", estimated_remote_msg_count=" << estimated_remote_msg_count 
            << ", estimated_remote_msg_size=" << estimated_remote_msg_size << Log::endl;
  return ctxs_inter_size;
}

mace::map< mace::string, uint64_t > mace::ContextRuntimeInfo::getContextInteractionCount() {
  ScopedLock sl(runtimeInfoMutex);
  mace::map< mace::string, uint64_t > ctxs_comm_count;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();
    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_access_count.begin(); iter2 != ctxes_event_access_count.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }
      if( ctxs_comm_count.find(iter2->first) == ctxs_comm_count.end() ) {
        ctxs_comm_count[iter2->first] = iter2->second;
      } else {
        ctxs_comm_count[iter2->first] += iter2->second;
      }
    }
  }

  for( mace::map<mace::string, ContetxtInteractionInfo>::iterator iter1 = contextInterInfos.begin(); iter1 != contextInterInfos.end();
      iter1 ++ ) {
    mace::map<mace::string, uint64_t>& caller_count = (iter1->second).callerMethodCount;
    for( mace::map<mace::string, uint64_t>::iterator iter2 = caller_count.begin(); iter2 != caller_count.end();
        iter2 ++ ) {
      if( ctxs_comm_count.find(iter1->first) == ctxs_comm_count.end() ) {
        ctxs_comm_count[iter1->first] = iter2->second;
      } else {
        ctxs_comm_count[iter1->first] += iter2->second;
      } 
    }
  }
  return ctxs_comm_count;
}

mace::map< mace::string, uint64_t > mace::ContextRuntimeInfo::getFromAccessCountByTypes( const mace::set<mace::string>& context_types ) {
  ADD_SELECTORS("ContextRuntimeInfo::getFromAccessCountByTypes");
  ScopedLock sl(runtimeInfoMutex);
  mace::map< mace::string, uint64_t > ctxs_comm_count;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();

    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_access_count.begin(); iter2 != ctxes_event_access_count.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }

      mace::string context_type = Util::extractContextType(iter2->first);
      if( context_types.size() > 0 && context_types.count(context_type) == 0 ){
        continue;
      }
      if( ctxs_comm_count.find(iter2->first) == ctxs_comm_count.end() ) {
        ctxs_comm_count[iter2->first] = iter2->second;
      } else {
        ctxs_comm_count[iter2->first] += iter2->second;
      }
    }
  }

  for( mace::map<mace::string, ContetxtInteractionInfo>::iterator iter1 = contextInterInfos.begin(); iter1 != contextInterInfos.end();
      iter1 ++ ) {
    mace::string context_type = Util::extractContextType(iter1->first);
    if( context_types.size() > 0 && context_types.count(context_type) == 0 ){
      continue;
    }

    mace::map<mace::string, uint64_t>& caller_count = (iter1->second).callerMethodCount;
    for( mace::map<mace::string, uint64_t>::iterator iter2 = caller_count.begin(); iter2 != caller_count.end();
        iter2 ++ ) {
      if( ctxs_comm_count.find(iter1->first) == ctxs_comm_count.end() ) {
        ctxs_comm_count[iter1->first] = iter2->second;
      } else {
        ctxs_comm_count[iter1->first] += iter2->second;
      } 
    }
  }
  return ctxs_comm_count;
}

mace::map< mace::string, uint64_t > mace::ContextRuntimeInfo::getToAccessCountByTypes( const mace::set<mace::string>& context_types ) {
  ADD_SELECTORS("ContextRuntimeInfo::getToAccessCountByTypes");
  ScopedLock sl(runtimeInfoMutex);
  mace::map< mace::string, uint64_t > ctxs_comm_count;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getToEventAccessCount();

    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_access_count.begin(); iter2 != ctxes_event_access_count.end();
        iter2 ++ ) {
      if( iter2->first == this->contextName ) {
        continue;
      }

      mace::string context_type = Util::extractContextType(iter2->first);
      if( context_types.size() > 0 && context_types.count(context_type) == 0 ){
        continue;
      }
      if( ctxs_comm_count.find(iter2->first) == ctxs_comm_count.end() ) {
        ctxs_comm_count[iter2->first] = iter2->second;
      } else {
        ctxs_comm_count[iter2->first] += iter2->second;
      }
    }
  }

  for( mace::map<mace::string, ContetxtInteractionInfo>::iterator iter1 = contextInterInfos.begin(); iter1 != contextInterInfos.end();
      iter1 ++ ) {
    mace::string context_type = Util::extractContextType(iter1->first);
    if( context_types.size() > 0 && context_types.count(context_type) == 0 ){
      continue;
    }

    mace::map<mace::string, uint64_t>& caller_count = (iter1->second).calleeMethodCount;
    for( mace::map<mace::string, uint64_t>::iterator iter2 = caller_count.begin(); iter2 != caller_count.end();
        iter2 ++ ) {
      if( ctxs_comm_count.find(iter1->first) == ctxs_comm_count.end() ) {
        ctxs_comm_count[iter1->first] = iter2->second;
      } else {
        ctxs_comm_count[iter1->first] += iter2->second;
      } 
    }
  }
  return ctxs_comm_count;
}

uint64_t mace::ContextRuntimeInfo::getExternalMessageCount( ) {
  ADD_SELECTORS("ContextRuntimeInfo::getExternalMessageCount");
  ScopedLock sl(runtimeInfoMutex);
  uint64_t total_count = 0;

  for( mace::map<mace::string, EventAccessInfo>::iterator iter1 = eventAccessInfos.begin(); iter1 != eventAccessInfos.end(); iter1 ++ ){
    mace::map< mace::string, uint64_t> ctxes_event_access_count = (iter1->second).getFromEventAccessCount();

    for( mace::map<mace::string, uint64_t>::iterator iter2 = ctxes_event_access_count.begin(); iter2 != ctxes_event_access_count.end();
        iter2 ++ ) {
      mace::string context_type = Util::extractContextType(iter2->first);
      if( context_type != "externalCommContext" ){
        continue;
      }

      total_count += iter2->second;
    }
  }

  return total_count;
}

uint64_t mace::ContextRuntimeInfo::getMarkerTotalLatency( const mace::string& marker ) {
  ScopedLock sl(runtimeInfoMutex);
  if( markerTotalTimeperiod.find(marker) == markerTotalTimeperiod.end() ) {
    return 0;
  }
  return markerTotalTimeperiod[marker];
}

double mace::ContextRuntimeInfo::getMarkerAvgLatency( const mace::string& marker ) {
  ScopedLock sl(runtimeInfoMutex);
  if( markerTotalTimeperiod.find(marker) == markerTotalTimeperiod.end() ) {
    return 0;
  }
  double avg_latency = markerTotalTimeperiod[marker] / markerTotalCount[marker];
  return avg_latency;
}

uint64_t mace::ContextRuntimeInfo::getMarkerCount( const mace::string& marker ) {
  ScopedLock sl(runtimeInfoMutex);
  if( markerTotalCount.find(marker) == markerTotalCount.end() ) {
    return 0;
  }
  return markerTotalCount[marker];
}

/***************************** class SatisfiedConditionInfo **********************************************/
void mace::SatisfiedConditionInfo::serialize(std::string& str) const{
  mace::serialize( str, &type );
  mace::serialize( str, &contexts );
  mace::serialize( str, &satisfied );
}

int mace::SatisfiedConditionInfo::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &type );
  serializedByteSize += mace::deserialize( is, &contexts );
  serializedByteSize += mace::deserialize( is, &satisfied );
  return serializedByteSize;
}


mace::string mace::SatisfiedConditionInfo::getColocateContextName( const mace::ContextMapping& snapshot, const mace::set<mace::string>& pin_ctx_types ) const {
  mace::string col_ctx_name = "";

  if( type == METHOD_COUNT ) {
    uint64_t max_count = 0;
    for( mace::map<mace::string, uint64_t>::const_iterator iter = contexts.begin(); iter != contexts.end(); iter ++ ) {
      mace::string ctx_type = Util::extractContextType(iter->first);
      if( pin_ctx_types.count(ctx_type) > 0 ) {
        continue;
      }

      if( mace::ContextMapping::getNodeByContext(snapshot, iter->first) != Util::getMaceAddr() && max_count < iter->second ){
        col_ctx_name = iter->first;
        max_count = iter->second;
      }
    }
  } else if( type == REFERENCE ) {
    uint64_t max_count = 0;
    for( mace::map<mace::string, uint64_t>::const_iterator iter = contexts.begin(); iter != contexts.end(); iter ++ ) {
      mace::string ctx_type = Util::extractContextType(iter->first);
      if( pin_ctx_types.count(ctx_type) > 0 ) {
        continue;
      }

      if( mace::ContextMapping::getNodeByContext(snapshot, iter->first) != Util::getMaceAddr() && max_count < iter->second ){
        col_ctx_name = iter->first;
        max_count = iter->second;
      }
    }
  }

  return col_ctx_name;
}

void mace::SatisfiedConditionInfo::print(std::ostream& out) const {
  out << "SatisfiedConditionInfo(";
  out << "type="; mace::printItem(out, &type ); out<<", ";
  out << "contexts="; mace::printItem(out, &contexts ); out<<", ";
  out << "satisfied="; mace::printItem(out, &satisfied ); 
  out << ")";
}
  
/***************************** class ElasticityCondition **********************************************/
mace::SatisfiedConditionInfo mace::ElasticityCondition::satisfyCondition(const mace::ContextRuntimeInfoForElasticity& ctx_rt_info, 
    const ContextStructure& ctxStructure ) const {
  ADD_SELECTORS("ElasticityCondition::satisfyCondition#1");
  SatisfiedConditionInfo s_cond_info;
  if( conditionType == METHOD_CALL_COUNT && compareType == ElasticityCondition::COMPARE_ME ) {
    s_cond_info.contexts = ctx_rt_info.getContextMethodCounts(this->threshold, this->contextTypes);
    if( s_cond_info.contexts.size() > 0 ) {
      s_cond_info.satisfied = true;
      s_cond_info.type = mace::SatisfiedConditionInfo::METHOD_COUNT;
    }
  } else if( conditionType == REFERENCE ) {
    const mace::string& ctx_name = ctx_rt_info.contextName;
    mace::string ctx_type = Util::extractContextType( ctx_name );
    if( contextTypes.count(ctx_type) > 0 && contextTypes.size() == 2 ) {
      mace::string col_ctx_type = "";

      for( mace::set<mace::string>::const_iterator iter = contextTypes.begin(); iter != contextTypes.end(); iter++ ) {
        if( ctx_type != *iter ) {
          col_ctx_type = *iter;
          break;
        }
      }

      mace::vector<mace::string> child_ctxes = ctxStructure.getAllChildContexts( ctx_name );
      for( uint32_t i=0; i<child_ctxes.size(); i++ ){
        if( child_ctxes[i] != ctx_name &&  Util::isContextType( child_ctxes[i], col_ctx_type ) ) {
          s_cond_info.contexts[ child_ctxes[i] ] = ctx_rt_info.getMethodCount(child_ctxes[i]) ;
        }
      }

      mace::vector<mace::string> parent_ctxes = ctxStructure.getAllParentContexts( ctx_name );
      for( uint32_t i=0; i<parent_ctxes.size(); i++ ){
        if( parent_ctxes[i] != ctx_name &&  Util::isContextType( parent_ctxes[i], col_ctx_type ) ) {
          s_cond_info.contexts[ parent_ctxes[i] ] = ctx_rt_info.getMethodCount(parent_ctxes[i]) ;
        }
      }

      if( s_cond_info.contexts.size() > 0 ) {
        s_cond_info.satisfied = true;
        s_cond_info.type = mace::SatisfiedConditionInfo::REFERENCE;
      }
    }
  }
  return s_cond_info;
} 

mace::SatisfiedConditionInfo mace::ElasticityCondition::satisfyCondition(const ServerRuntimeInfo& server_info ) const {
  ADD_SELECTORS("ElasticityCondition::satisfyCondition#2");
  SatisfiedConditionInfo s_cond_info;
  s_cond_info.satisfied = false;
  if( conditionType == SERVER_CPU_USAGE ) {
    macedbg(1) << "threshold=" << this->threshold << " cpu_usage=" << server_info.CPUUsage << Log::endl;
    if( compareType == ElasticityCondition::COMPARE_MT && server_info.CPUUsage > this->threshold ) {
      s_cond_info.satisfied = true;
      s_cond_info.type = mace::SatisfiedConditionInfo::UPPER_SERVER_CPU_USAGE;
    } else if( compareType == ElasticityCondition::COMPARE_LT && server_info.CPUUsage < this->threshold ) {
      s_cond_info.satisfied = true;
      s_cond_info.type = mace::SatisfiedConditionInfo::LOWER_SERVER_CPU_USAGE;
    }
  } 
  return s_cond_info;
}

mace::SatisfiedConditionInfo mace::ElasticityCondition::satisfyCondition( const mace::map<mace::string, mace::ContextRuntimeInfoForElasticity >& contextsRuntimeInfo ) const {
  ADD_SELECTORS("ElasticityCondition::satisfyCondition#3");
  SatisfiedConditionInfo s_cond_info;
  if( conditionType == EXTERNAL_MSG_COUNT_PERCENT ) {
    s_cond_info.type = mace::SatisfiedConditionInfo::EXTERNAL_MSG_COUNT_PERCENT;

    mace::map< mace::string, uint64_t > msg_counts;
    uint64_t total_count = 0;

    for( mace::map<mace::string, mace::ContextRuntimeInfoForElasticity >::const_iterator iter = contextsRuntimeInfo.begin(); iter != contextsRuntimeInfo.end(); iter ++ ) {
      msg_counts[ iter->first ] = (iter->second).externalMessageCount;
      total_count += msg_counts[iter->first];
    }

    for( mace::map< mace::string, uint64_t>::iterator iter = msg_counts.begin(); iter != msg_counts.end(); iter ++ ) {
      double p = (double)iter->second / total_count;
      macedbg(1) << "context("<< iter->first <<")'s percent=" << p << ", threshold=" << this->threshold << Log::endl;
      if( this->compareType == mace::ElasticityCondition::COMPARE_MT && p > this->threshold  ) {
        s_cond_info.contexts[iter->first] = iter->second;
        s_cond_info.satisfied = true;
      }
    }
  } 

  return s_cond_info;
}

bool mace::ElasticityCondition::checkContextCreateRule( const mace::string& ctx_name) const {
  if( conditionType == REFERENCE ) {
    return true;
  }

  return false;
}

void mace::ElasticityCondition::serialize(std::string& str) const{
  mace::serialize( str, &conditionType );
  mace::serialize( str, &compareType );
  mace::serialize( str, &contextTypes );
  mace::serialize( str, &marker );
  mace::serialize( str, &threshold );

}

int mace::ElasticityCondition::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &conditionType );
  serializedByteSize += mace::deserialize( is, &compareType );
  serializedByteSize += mace::deserialize( is, &contextTypes );
  serializedByteSize += mace::deserialize( is, &marker );
  serializedByteSize += mace::deserialize( is, &threshold );
  return serializedByteSize;
}

/***************************** class ElasticityAndConditions **********************************************/
void mace::ElasticityAndConditions::serialize(std::string& str) const{
  mace::serialize( str, &conditions );
}

int mace::ElasticityAndConditions::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &conditions );
  return serializedByteSize;
}

bool mace::ElasticityAndConditions::satisfyAndConditions(const mace::ContextRuntimeInfoForElasticity& ctx_rt_info, 
    const ContextStructure& ctxStructure, mace::vector<mace::SatisfiedConditionInfo>& s_cond_infos ) const {
  ADD_SELECTORS("ElasticityAndConditions::satisfyAndConditions#1");
  ASSERT( conditions.size() > 0 );
  
  for( uint32_t i=0; i<conditions.size(); i++ ){
    SatisfiedConditionInfo s_cond_info = conditions[i].satisfyCondition(ctx_rt_info, ctxStructure);
    if( !s_cond_info.satisfied ){
      return false;
    } else {
      s_cond_infos.push_back(s_cond_info);
    }
  }

  return true;
}

bool mace::ElasticityAndConditions::satisfyAndConditions(const ServerRuntimeInfo& server_info, 
    mace::vector<mace::SatisfiedConditionInfo>& s_cond_infos ) const {
  ADD_SELECTORS("ElasticityAndConditions::satisfyAndConditions#2");
  ASSERT( conditions.size() > 0 );
  
  for( uint32_t i=0; i<conditions.size(); i++ ){
    SatisfiedConditionInfo s_cond_info = conditions[i].satisfyCondition(server_info);
    if( !s_cond_info.satisfied ){
      return false;
    } else {
      s_cond_infos.push_back(s_cond_info);
    }
  }

  return true;
}

bool mace::ElasticityAndConditions::satisfyAndConditions( const ServerRuntimeInfo& server_info,
    const mace::map<mace::string, mace::ContextRuntimeInfoForElasticity >& contextsRuntimeInfo, 
    mace::vector<mace::SatisfiedConditionInfo>& s_cond_infos ) const {

  ADD_SELECTORS("ElasticityAndConditions::satisfyAndConditions#3");
  ASSERT( conditions.size() > 0 );
  
  for( uint32_t i=0; i<conditions.size(); i++ ){
    if( conditions[i].conditionType == mace::ElasticityCondition::SERVER_CPU_USAGE ){
      SatisfiedConditionInfo s_cond_info = conditions[i].satisfyCondition(server_info);
      if( !s_cond_info.satisfied ){
        return false;
      } else {
        s_cond_infos.push_back(s_cond_info);
      }
    } else if( conditions[i].conditionType == mace::ElasticityCondition::EXTERNAL_MSG_COUNT_PERCENT ){
      SatisfiedConditionInfo s_cond_info = conditions[i].satisfyCondition(contextsRuntimeInfo);
      if( !s_cond_info.satisfied ){
        return false;
      } else {
        s_cond_infos.push_back(s_cond_info);
      }
    } else {
      return false;
    }
  }

  return true;
}

double mace::ElasticityAndConditions::getServerCPUUpperBound() const {
  for(uint32_t i=0; i<conditions.size(); i++ ) {
    if( conditions[i].conditionType == mace::ElasticityCondition::SERVER_CPU_USAGE && 
        conditions[i].compareType == mace::ElasticityCondition::COMPARE_MT ) {
      return conditions[i].threshold;
    }
  }
  return -1.0;
}

double mace::ElasticityAndConditions::getServerCPULowerBound() const {
  for(uint32_t i=0; i<conditions.size(); i++ ) {
    if( conditions[i].conditionType == mace::ElasticityCondition::SERVER_CPU_USAGE && 
        conditions[i].compareType == mace::ElasticityCondition::COMPARE_LT ) {
      return conditions[i].threshold;
    }
  }
  return -1.0;
}

bool mace::ElasticityAndConditions::checkContextCreateRule( const mace::string& ctx_name) const {
  for( uint32_t i=0; i<conditions.size(); i++ ) {
    if( conditions[i].checkContextCreateRule(ctx_name) ) {
      return true;
    }
  }

  return false;
}

bool mace::ElasticityAndConditions::includeReferenceCondtion( const mace::string& ctx_type ) const {
  for( uint32_t i=0; i<conditions.size(); i++ ){
    if( conditions[i].conditionType == mace::ElasticityCondition::REFERENCE && conditions[i].contextTypes.count(ctx_type) > 0 ) {
      return true;
    }
  }

  return false;
}
  
bool mace::ElasticityAndConditions::includeServerCPUUsageCondition() const {
  for( uint32_t i=0; i<conditions.size(); i++ ){
    if( conditions[i].conditionType == mace::ElasticityCondition::SERVER_CPU_USAGE ) {
      return true;
    }
  }

  return false;
}

/***************************** class ElasticityBehavior **********************************************/
void mace::ElasticityBehavior::serialize(std::string& str) const{
  mace::serialize( str, &behaviorType );
  mace::serialize( str, &resourceType );
  mace::serialize( str, &specialRequirement );
  mace::serialize( str, &contextNames );
  mace::serialize( str, &contextTypes );
}

int mace::ElasticityBehavior::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &behaviorType );
  serializedByteSize += mace::deserialize( is, &resourceType );
  serializedByteSize += mace::deserialize( is, &specialRequirement );
  serializedByteSize += mace::deserialize( is, &contextNames );
  serializedByteSize += mace::deserialize( is, &contextTypes );
  return serializedByteSize;
}



/***************************** class ElasticityRule **********************************************/
mace::ElasticityBehaviorAction* mace::ElasticityRule::proposeMigrationAction( const mace::ContextRuntimeInfoForElasticity& ctx_rt_info, 
    const ContextMapping& snapshot, const ContextStructure& ctxStructure  ) const {
  
  ADD_SELECTORS("ElasticityRule::proposeMigrationAction");
  ASSERT( conditions.size() > 0 );

  bool satisfied = false;
  
  mace::vector<SatisfiedConditionInfo> s_cond_infos;
  for( uint32_t i=0; i<conditions.size(); i++ ) {
    s_cond_infos.clear();
    if( conditions[i].satisfyAndConditions( ctx_rt_info, ctxStructure, s_cond_infos ) ) {
      satisfied = true;
      break;
    }
  }

  if( satisfied ) {
    macedbg(1) << "SatisfiedConditionInfos: " << s_cond_infos << Log::endl;
    if( behavior.behaviorType == mace::ElasticityBehavior::COLOCATE ) {
      uint8_t rule_type = mace::ElasticityBehaviorAction::UNDEF;
      uint8_t spec_req = mace::ElasticityBehaviorAction::UNDEF;

      mace::string col_ctx_name = "";
      mace::set<mace::string> pin_ctx_types;
      if( behavior.specialRequirement == mace::ElasticityBehavior::REQ_PIN_ONE_ACTOR ) {
        ASSERT( behavior.contextTypes.size() > 1 );
        pin_ctx_types.insert( behavior.contextTypes[1] );
        spec_req = mace::ElasticityBehaviorAction::REQ_PIN_ONE_ACTOR;
      }

      mace::map<mace::string, uint64_t> ctx_inter_count;
      for( uint32_t i=0; i<s_cond_infos.size(); i++ ) {
        if( s_cond_infos[i].type == mace::SatisfiedConditionInfo::METHOD_COUNT ) {
          rule_type = mace::ElasticityBehaviorAction::RULE_METHOD_COUNT;
          col_ctx_name = s_cond_infos[i].getColocateContextName( snapshot, pin_ctx_types );
          ctx_inter_count = s_cond_infos[i].contexts;
        } else if( s_cond_infos[i].type == mace::SatisfiedConditionInfo::REFERENCE ) {
          rule_type = mace::ElasticityBehaviorAction::RULE_REFERENCE;
          col_ctx_name = s_cond_infos[i].getColocateContextName( snapshot, pin_ctx_types );
          ctx_inter_count = s_cond_infos[i].contexts;
        }
      }

      if( col_ctx_name != "" ){ 
        const mace::MaceAddr& addr = mace::ContextMapping::getNodeByContext(snapshot, col_ctx_name);
        if( Util::getMaceAddr() != addr ) {
          mace::ElasticityBehaviorAction* maction = new mace::ElasticityBehaviorAction( ElasticityBehaviorAction::EBACTION_COLOCATE, 
            rule_type, this->priority, spec_req, Util::getMaceAddr(), addr, ctx_rt_info.contextName, ctx_rt_info.contextExecTime );
          maction->contextsInterCount = ctx_inter_count;

          macedbg(1) << "Generating: " << *maction << Log::endl;

          return maction;
        }
      }
    } 
  }

  macedbg(1) << "Do not generate any maction for " << ctx_rt_info.contextName << Log::endl;
  return NULL;
}

mace::vector< mace::ContextRuntimeInfoForElasticity > mace::ElasticityRule::aggregateContextRuntimeInfos( const ServerRuntimeInfo& server_info, 
    const mace::map<mace::string, mace::ContextRuntimeInfoForElasticity >& contextsRuntimeInfo ) const {
  
  ADD_SELECTORS("ElasticityRule::aggregateContextRuntimeInfos");

  macedbg(1) << "relatedContextTypes: " << this->relatedContextTypes << Log::endl;
    
  ASSERT( conditions.size() > 0 );
  
  bool satisfied = false;

  mace::vector<SatisfiedConditionInfo> s_cond_infos;

  if( this->ruleType == mace::ElasticityRule::RESOURCE_RULE ){
    for( uint32_t i=0; i<conditions.size(); i++ ) {
      s_cond_infos.clear();
      if( conditions[i].satisfyAndConditions( server_info, contextsRuntimeInfo, s_cond_infos ) ) {
        satisfied = true;
        break;
      }
    }
  } 
  

  mace::vector<mace::ContextRuntimeInfoForElasticity> report_ctx_rt_infos;
  if( satisfied ) {
    macedbg(1) << "Satisfied!!!" << Log::endl;

    if( behavior.behaviorType == mace::ElasticityBehavior::WORKLOAD_BALANCE ) {
      //uint8_t rule_type = mace::ElasticityBehaviorAction::RULE_UNDEF;

      if( behavior.resourceType == mace::ElasticityBehavior::RES_CPU ) {
        //rule_type = mace::ElasticityBehaviorAction::RULE_CPU_BALANCE;
      }

      for( mace::map<mace::string, mace::ContextRuntimeInfoForElasticity>::const_iterator iter = contextsRuntimeInfo.begin(); 
          iter != contextsRuntimeInfo.end(); iter ++ ) {
        mace::string context_type = Util::extractContextType( (iter->second).contextName );
        if( relatedContextTypes.count(context_type) > 0 ) {
          macedbg(1) << "Report context("<< (iter->second).contextName <<")'s runtime information("<< (iter->second).contextExecTime <<") to GEM!" << Log::endl;
          report_ctx_rt_infos.push_back( iter->second );
        }
      }
    } else if( behavior.behaviorType == mace::ElasticityBehavior::ISOLATE ) {
      SatisfiedConditionInfo& s_cond_info = s_cond_infos[0];

      for( uint32_t i=0; i<s_cond_infos.size(); i++ ) {
        if(  s_cond_infos[i].type == mace::SatisfiedConditionInfo::EXTERNAL_MSG_COUNT_PERCENT ){
          s_cond_info = s_cond_infos[i];
          break;
        }
      }

      macedbg(1) << "Candidates: " << s_cond_info.contexts << Log::endl;

      const mace::map<mace::string, uint64_t>& contexts = s_cond_info.contexts;
      for( mace::map<mace::string, uint64_t>::const_iterator iter = contexts.begin(); iter != contexts.end(); iter ++ ) {
        mace::string context_type = Util::extractContextType( iter->first );
        if( relatedContextTypes.count(context_type) > 0 ) {
          macedbg(1) << "Report context("<< iter->first <<")'s runtime information("<< iter->second <<") to GEM!" << Log::endl;

          mace::map<mace::string, ContextRuntimeInfoForElasticity>::const_iterator cIter = contextsRuntimeInfo.find( iter->first);
          report_ctx_rt_infos.push_back( cIter->second );
        }
      }
    }
  }

  return report_ctx_rt_infos;
}

bool mace::ElasticityRule::relatedContextType( const mace::string& context_type ) const {
  if(relatedContextTypes.count(context_type) > 0 ){
    return true;
  } else {
    return false;
  }
}

bool mace::ElasticityRule::relatedContext( const mace::string& ctx_name ) const {
  mace::string context_type = Util::extractContextType( ctx_name );
  if(relatedContextTypes.count(context_type) > 0 ){
    return true;
  } else {
    return false;
  }
}

double mace::ElasticityRule::getServerCPUUpperBound() const {
  double upper_bound = -1.0;
  for( uint32_t i=0; i<conditions.size(); i++ ) {
    upper_bound = conditions[i].getServerCPUUpperBound();
    if( upper_bound >= 0 ) {
      break;
    }
  }

  if( upper_bound < 0 ){
    upper_bound = 100.0;
  }
  return upper_bound;
}

double mace::ElasticityRule::getServerCPULowerBound() const {
  double lower_bound = -1.0;
  for( uint32_t i=0; i<conditions.size(); i++ ) {
    lower_bound = conditions[i].getServerCPULowerBound();
    if( lower_bound >= 0 ) {
      break;
    }
  }

  if( lower_bound < 0 ){
    lower_bound = 0;
  }
  return lower_bound;
}

bool mace::ElasticityRule::checkContextCreateRule( const mace::string& ctx_name) const {
  for( uint32_t i=0; i<conditions.size(); i++ ) {
    if( conditions[i].checkContextCreateRule(ctx_name) ) {
      return true;
    }
  }

  return false;
}

mace::ElasticityRuleInfo mace::ElasticityRule::getContextCreateRule( const mace::string& ctx_type ) const {
  mace::ElasticityRuleInfo rule_info;

  for( uint32_t i=0; i<conditions.size(); i++ ) {
    if( conditions[i].includeReferenceCondtion(ctx_type) && behavior.behaviorType == mace::ElasticityBehavior::COLOCATE ) {
      rule_info.priority = this->priority;
      rule_info.ruleType = mace::ElasticityRuleInfo::REFERENCE_COLOCATE;

      const mace::vector<mace::string>& relatedContextTypes = behavior.contextTypes;
      for( uint32_t i=0; i<relatedContextTypes.size(); i++ ) {
        rule_info.contextTypes.insert(relatedContextTypes[i]);
      }
      break;
    } else if( conditions[i].includeServerCPUUsageCondition() && 
        behavior.behaviorType == mace::ElasticityBehavior::WORKLOAD_BALANCE && 
        behavior.resourceType == mace::ElasticityBehavior::RES_CPU ) {
      rule_info.priority = this->priority;
      rule_info.ruleType = mace::ElasticityRuleInfo::BALANCE_SERVER_CPU_USAGE;
      break;
    }
  }

  return rule_info;
}

void mace::ElasticityRule::serialize(std::string& str) const{
  mace::serialize( str, &ruleType );
  mace::serialize( str, &priority );
  mace::serialize( str, &conditions );
  mace::serialize( str, &relatedContextTypes );
  mace::serialize( str, &behavior );
}

int mace::ElasticityRule::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &ruleType );
  serializedByteSize += mace::deserialize( is, &priority );
  serializedByteSize += mace::deserialize( is, &conditions );
  serializedByteSize += mace::deserialize( is, &relatedContextTypes );
  serializedByteSize += mace::deserialize( is, &behavior );
  return serializedByteSize;
}

/***************************** class ElasticityConfiguration **********************************************/

void mace::ElasticityConfiguration::readElasticityPolicies() {

}

mace::vector< mace::ElasticityRule > mace::ElasticityConfiguration::getRulesByContextType( const mace::string& context_type ) const {
  mace::vector< mace::ElasticityRule > rules;
  for( uint32_t i=0; i<elasticityRules.size(); i++ ){
    if( elasticityRules[i].relatedContextTypes.count(context_type) > 0 || elasticityRules[i].relatedContextTypes.count("ANY") > 0 ) {
      rules.push_back( elasticityRules[i] );
    }
  }
  return rules;
}

bool mace::ElasticityConfiguration::checkContextCreateRule( const mace::string& ctx_name) const {
  mace::string ctx_type = Util::extractContextType(ctx_name);

  for( uint32_t i=0; i<elasticityRules.size(); i++ ) {
    const mace::ElasticityRule& rule = elasticityRules[i];

    if( rule.relatedContextType( ctx_type ) && rule.checkContextCreateRule(ctx_name) ) {
      return true;
    }
  }

  return false;
}

mace::ElasticityRuleInfo mace::ElasticityConfiguration::getContextCreateRule( const mace::string& ctx_type ) const {
  mace::ElasticityRuleInfo rule_info;

  for( uint32_t i=0; i<elasticityRules.size(); i++ ) {
    if( elasticityRules[i].relatedContextType(ctx_type) ) {
      mace::ElasticityRuleInfo r_info = elasticityRules[i].getContextCreateRule( ctx_type );
      if( r_info.priority > rule_info.priority ) {
        rule_info = r_info;
      }
    }
  }

  return rule_info;
}



