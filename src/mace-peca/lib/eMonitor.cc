#include "eMonitor.h"
#include "SysUtil.h"
#include "ContextService.h"

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "sys/types.h"
#include "sys/sysinfo.h"

const uint8_t mace::ElasticityBehaviorAction::EBACTION_COLOCATE;
const uint8_t mace::ElasticityBehaviorAction::EBACTION_MIGRATION;
const uint8_t mace::ElasticityBehaviorAction::RULE_CPU_BALANCE; 
const uint8_t mace::ElasticityBehaviorAction::REQ_PIN_ONE_ACTOR;
const uint8_t mace::ElasticityBehaviorAction::UNDEF; 

/******************************* class ElasticityBehaviorAction *************************************************/
uint64_t mace::ElasticityBehaviorAction::getNodeMethodCount( const mace::MaceAddr& addr, 
		const mace::set<mace::string>& predict_local_context, const mace::ContextMapping& snapshot) const {
	uint64_t count = 0;
	for( mace::map<mace::string, uint64_t>::const_iterator iter = contextsInterCount.begin(); iter != contextsInterCount.end(); 
			iter ++) {
		if( predict_local_context.count(iter->first) > 0 || mace::ContextMapping::getNodeByContext( snapshot, iter->first ) == addr ) {
			count += iter->second;
		}
	}

	return count;
}

void mace::ElasticityBehaviorAction::serialize(std::string& str) const{
  mace::serialize( str, &actionType );
  mace::serialize( str, &ruleType );
  mace::serialize( str, &priority );
  mace::serialize( str, &specialRequirement );
  mace::serialize( str, &fromNode );
  mace::serialize( str, &toNode );
  mace::serialize( str, &contextName );
  mace::serialize( str, &contextExecTime );
  mace::serialize( str, &contextsInterCount );
}

int mace::ElasticityBehaviorAction::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &actionType );
  serializedByteSize += mace::deserialize( is, &ruleType );
  serializedByteSize += mace::deserialize( is, &priority );
  serializedByteSize += mace::deserialize( is, &specialRequirement );
  serializedByteSize += mace::deserialize( is, &fromNode );
  serializedByteSize += mace::deserialize( is, &toNode );
  serializedByteSize += mace::deserialize( is, &contextName );
  serializedByteSize += mace::deserialize( is, &contextExecTime );
  serializedByteSize += mace::deserialize( is, &contextsInterCount );
  return serializedByteSize;
}

void mace::ElasticityBehaviorAction::print(std::ostream& out) const {
  out<< "ElasticityBehaviorAction(";
  out<< "actionType="; mace::printItem(out, &actionType ); out<<", ";
  out<< "ruleType="; mace::printItem(out, &ruleType ); out<<", ";
  out<< "priority="; mace::printItem(out, &priority ); out<<", ";
  out<< "specialRequirement="; mace::printItem(out, &specialRequirement ); out<<", ";
  out<< "fromNode="; mace::printItem(out, &fromNode); out<<", ";
  out<< "toNode="; mace::printItem(out, &toNode); out<< ", ";
  out<< "contextName="; mace::printItem(out, &contextName);
  out<< ")";
}

/******************************* class CPUInformation ***********************************************************/
void mace::CPUInformation::serialize(std::string& str) const{
  mace::serialize( str, &totalUserCPUTime );
  mace::serialize( str, &totalUserLowCPUTime );
  mace::serialize( str, &totalSysCPUTime );
  mace::serialize( str, &totalIdleCPUTime );
}

int mace::CPUInformation::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &totalUserCPUTime );
  serializedByteSize += mace::deserialize( is, &totalUserLowCPUTime );
  serializedByteSize += mace::deserialize( is, &totalSysCPUTime   );
  serializedByteSize += mace::deserialize( is, &totalIdleCPUTime   );
  return serializedByteSize;
}

/******************************* class ServerRuntimeInfo **************************************************/
void mace::ServerRuntimeInfo::serialize(std::string& str) const{
  mace::serialize( str, &CPUUsage );
  mace::serialize( str, &totalCPUTime );
  mace::serialize( str, &contextsNumber );
}

int mace::ServerRuntimeInfo::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &CPUUsage );
  serializedByteSize += mace::deserialize( is, &totalCPUTime );
  serializedByteSize += mace::deserialize( is, &contextsNumber );
  return serializedByteSize;
}

void mace::ServerRuntimeInfo::print(std::ostream& out) const {
  out<< "ServerRuntimeInfo(";
  out<< "CPUUsage="; mace::printItem(out, &(CPUUsage) ); out<<", ";
  out<< "totalCPUTime="; mace::printItem(out, &(totalCPUTime) ); out<<", ";
  out<< "contextsNumber="; mace::printItem(out, &(contextsNumber) ); 
  out<< ")";
}

/******************************* class ContextRuntimeInfoForElasticity ***********************************************************/
void mace::ContextRuntimeInfoForElasticity::serialize(std::string& str) const{
  mace::serialize( str, &contextName );
  mace::serialize( str, &currAddr );
  mace::serialize( str, &fromAccessCount );
  mace::serialize( str, &fromMessageSize );
  mace::serialize( str, &toAccessCount );
  mace::serialize( str, &currLatency );
  mace::serialize( str, &contextExecTime );
  mace::serialize( str, &avgLatency );
  mace::serialize( str, &count );
  mace::serialize( str, &externalMessageCount );
}

int mace::ContextRuntimeInfoForElasticity::deserialize(std::istream & is) throw (mace::SerializationException){
  int serializedByteSize = 0;
  serializedByteSize += mace::deserialize( is, &contextName );
  serializedByteSize += mace::deserialize( is, &currAddr );
  serializedByteSize += mace::deserialize( is, &fromAccessCount );
  serializedByteSize += mace::deserialize( is, &fromMessageSize );
  serializedByteSize += mace::deserialize( is, &toAccessCount );
  serializedByteSize += mace::deserialize( is, &currLatency );
  serializedByteSize += mace::deserialize( is, &contextExecTime );
  serializedByteSize += mace::deserialize( is, &avgLatency );
  serializedByteSize += mace::deserialize( is, &count );
  serializedByteSize += mace::deserialize( is, &externalMessageCount );
  return serializedByteSize;
}

void mace::ContextRuntimeInfoForElasticity::print(std::ostream& out) const {
  out<< "ContextRuntimeInfoForElasticity(";
  out<< "contextName="; mace::printItem(out, &contextName ); out<<", ";
  out<< "currAddr="; mace::printItem(out, &(currAddr) ); 
  out<< ")";
}


mace::map< mace::MaceAddr, uint64_t > mace::ContextRuntimeInfoForElasticity::computeExchangeBenefit( const mace::ContextMapping& snapshot ) {
  ADD_SELECTORS("ContextRuntimeInfoForElasticity::computeExchangeBenefit");
  mace::map< mace::MaceAddr, uint64_t > servers_comm_count;
  for( mace::map< mace::string, uint64_t >::iterator iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
  	mace::MaceAddr addr = mace::ContextMapping::getNodeByContext( snapshot, iter->first );
  	if( servers_comm_count.find(addr) == servers_comm_count.end() ) {
  		servers_comm_count[addr] = iter->second;
  	} else {
  		servers_comm_count[addr] += iter->second;
  	}
  }

  mace::map< mace::MaceAddr, uint64_t > exchange_benefits;
  uint64_t local_comm_count = 0;
  if( servers_comm_count.find(Util::getMaceAddr()) != servers_comm_count.end() ) {
    local_comm_count = servers_comm_count[ Util::getMaceAddr()];
  }
  macedbg(1) << "Local msg count: " << local_comm_count << Log::endl;

  for( mace::map< mace::MaceAddr, uint64_t >::iterator iter = servers_comm_count.begin(); iter != servers_comm_count.end(); iter ++ ){
    if( iter->first == Util::getMaceAddr() ) {
      continue;
    }

    macedbg(1) << "Server("<< iter->first <<") msg count: " << iter->second << Log::endl;
    if( iter->second > local_comm_count ) {
      exchange_benefits[ iter->first ] = iter->second - local_comm_count;
    }
  }
  
  return exchange_benefits;
}

uint64_t mace::ContextRuntimeInfoForElasticity::getTotalFromAccessCount() const {
	uint64_t total_count = 0;
	for( mace::map< mace::string, uint64_t>::const_iterator iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
		total_count += iter->second;
	}
	return total_count;
} 

bool mace::ContextRuntimeInfoForElasticity::isStrongConnected( const mace::string& ctx_name ) const {
	mace::map<mace::string, uint64_t> total_counts;
	mace::map< mace::string, uint64_t>::const_iterator iter;

	for(iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
		mace::string context_type = Util::extractContextType(iter->first);
		if( context_type == "globalContext" || context_type == "externalCommContext") {
			continue;
		}
		if( total_counts.find(iter->first) == total_counts.end() ) {
			total_counts[iter->first] = iter->second;
		} else {
			total_counts[iter->first] += iter->second;
		}
	}
	 
	for(iter = toAccessCount.begin(); iter != toAccessCount.end(); iter ++ ){
		mace::string context_type = Util::extractContextType(iter->first);
		if( context_type == "globalContext" || context_type == "externalCommContext") {
			continue;
		}
		if( total_counts.find(iter->first) == total_counts.end() ) {
			total_counts[iter->first] = iter->second;
		} else {
			total_counts[iter->first] += iter->second;
		}
	}

	if( total_counts.find(ctx_name) == total_counts.end() ) {
		return false;
	}

	uint64_t total_count = total_counts[ctx_name];
	double count = 0;
	for( iter = total_counts.begin(); iter != total_counts.end(); iter ++ ){

		if( total_count >= iter->second ){
			count ++;
		}
	}

	double frequency = count / total_counts.size();
	if( frequency > eMonitor::CONTEXT_STRONG_CONNECTED_PER_THREAHOLD ) {
		return true;
	} else if( contextExecTime/count > eMonitor::CONTEXT_STRONG_CONNECTED_NUM_THREAHOLD ) {
		return true;
	} else {
		return false;
	}
}

mace::set<mace::string> mace::ContextRuntimeInfoForElasticity::getStrongConnectContexts() const {
	ADD_SELECTORS("ContextRuntimeInfoForElasticity::getStrongConnectContexts");
	mace::map< mace::string, uint64_t > total_counts;
	mace::map< mace::string, uint64_t >::const_iterator iter;

	uint64_t total_count = 0;

	for(iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
		mace::string context_type = Util::extractContextType(iter->first);
		if( context_type == "globalContext" || context_type == "externalCommContext") {
			continue;
		}
		if( total_counts.find(iter->first) == total_counts.end() ) {
			total_counts[iter->first] = iter->second;
		} else {
			total_counts[iter->first] += iter->second;
		}
		total_count += iter->second;
	}
	 
	for(iter = toAccessCount.begin(); iter != toAccessCount.end(); iter ++ ){
		mace::string context_type = Util::extractContextType(iter->first);
		if( context_type == "globalContext" || context_type == "externalCommContext") {
			continue;
		}
		if( total_counts.find(iter->first) == total_counts.end() ) {
			total_counts[iter->first] = iter->second;
		} else {
			total_counts[iter->first] += iter->second;
		}
		total_count += iter->second;
	}
	double avg_count = 0.0;
	if( total_counts.size() > 0 ){
		avg_count = total_count / total_counts.size();
	}

	mace::set<mace::string> strong_conn_contexts;

	for( mace::map<mace::string, uint64_t>::iterator iter1 = total_counts.begin(); iter1 != total_counts.end(); iter1 ++ ){
		macedbg(1) << "context("<< iter1->first <<") count: " << iter1->second << Log::endl;
		// double count = 0.0;
		// for( mace::map<mace::string, uint64_t>::iterator iter2 = total_counts.begin(); iter2 != total_counts.end(); iter2 ++ ) {
		// 	if( iter1->second >= iter2->second ){
		// 		count ++;
		// 	}
		// }
		// double p = count / total_counts.size();
		if( // p > eMonitor::CONTEXT_STRONG_CONNECTED_PER_THREAHOLD || 
				iter1->second > eMonitor::CONTEXT_STRONG_CONNECTED_NUM_THREAHOLD
				|| iter1->second > avg_count ) {
			strong_conn_contexts.insert(iter1->first);
		}
	}
	return strong_conn_contexts;	
}

mace::map<mace::string, uint64_t> mace::ContextRuntimeInfoForElasticity::getContextMethodCounts( const double& threshold, 
		const mace::set<mace::string>& contextTypes ) const {
	ADD_SELECTORS("ContextRuntimeInfoForElasticity::getContextMethodCounts");
	mace::map< mace::string, uint64_t > total_counts;
	mace::map< mace::string, uint64_t >::const_iterator iter;

	mace::map<mace::string, uint64_t> ctx_method_counts;

	for(iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
		mace::string context_type = Util::extractContextType(iter->first);
		if( context_type == "globalContext" || context_type == "externalCommContext") {
			continue;
		}
		if( contextTypes.size() > 0 && contextTypes.count(context_type) == 0 ) {
			continue;
		}

		if( total_counts.find(iter->first) == total_counts.end() ) {
			total_counts[iter->first] = iter->second;
		} else {
			total_counts[iter->first] += iter->second;
		}
	}
	 
	for(iter = toAccessCount.begin(); iter != toAccessCount.end(); iter ++ ){
		mace::string context_type = Util::extractContextType(iter->first);
		if( context_type == "globalContext" || context_type == "externalCommContext") {
			continue;
		}
		if( contextTypes.size() > 0 && contextTypes.count(context_type) == 0 ) {
			continue;
		}

		if( total_counts.find(iter->first) == total_counts.end() ) {
			total_counts[iter->first] = iter->second;
		} else {
			total_counts[iter->first] += iter->second;
		}
	}



	for( mace::map<mace::string, uint64_t>::iterator iter = total_counts.begin(); iter != total_counts.end(); iter ++ ){
		if( iter->second >= threshold ) {
			ctx_method_counts[iter->first] = iter->second;
		}
	}
	return ctx_method_counts;	
}

uint64_t mace::ContextRuntimeInfoForElasticity::getMethodCount( const mace::string& ctx_name ) const {
	ADD_SELECTORS("ContextRuntimeInfoForElasticity::getMethodCount");
	uint64_t total_method_count = 0;

	mace::map< mace::string, uint64_t >::const_iterator iter = fromAccessCount.find(ctx_name);
	if( iter != fromAccessCount.end() ) {
		total_method_count += iter->second;
	} 

	iter = toAccessCount.find(ctx_name);
	if( iter != toAccessCount.end() ) {
		total_method_count += iter->second;
	}

	return total_method_count;
}

uint64_t mace::ContextRuntimeInfoForElasticity::getNodeInterCount( const mace::ContextMapping& snapshot, const MaceAddr& nodeAddr, 
    const mace::map<mace::string, mace::MaceAddr>& adjust_contexts ) const {
	ADD_SELECTORS("ContextRuntimeInfoForElasticity::getNodeInterCount");
	uint64_t inter_count = 0;
	mace::map< mace::string, uint64_t >::const_iterator iter;
	mace::map< mace::string, mace::MaceAddr >::const_iterator iter2;

	for(iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
		mace::MaceAddr addr;
		iter2 = adjust_contexts.find(iter->first);
		if( iter2 != adjust_contexts.end() ) {
			addr = iter2->second;
		} else {
			addr = mace::ContextMapping::getNodeByContext( snapshot, iter->first );
		}

		if( addr == nodeAddr ) {
			inter_count += iter->second;		}

	}
	 
	for(iter = toAccessCount.begin(); iter != toAccessCount.end(); iter ++ ){
		mace::MaceAddr addr;
		iter2 = adjust_contexts.find(iter->first);
		if( iter2 != adjust_contexts.end() ) {
			addr = iter2->second;
		} else {
			addr = mace::ContextMapping::getNodeByContext( snapshot, iter->first );
		}

		if( addr == nodeAddr ) {
			inter_count += iter->second;
		}
	}

	return inter_count;		
}

uint64_t mace::ContextRuntimeInfoForElasticity::getTotalClientRequestNumber() const {
	uint64_t count = 0;
	for( mace::map<mace::string, uint64_t>::const_iterator iter = fromAccessCount.begin(); iter != fromAccessCount.end(); iter ++ ){
		mace::string ctype = Util::extractContextType(iter->first);
		if( ctype == "externalCommContext" ){
			count += iter->second;
		}
	}
	return count;
}

/******************************* class LEMReportInfo ***********************************************************/
mace::vector<mace::ContextRuntimeInfoForElasticity> mace::LEMReportInfo::findMigrationContextForCPU( const double& upper_bound, const mace::set<mace::string>& selected_ctx_types ) {
	ADD_SELECTORS("LEMReportInfo::findMigrationContextForCPU");
	
	double cur_cpu_usage = curCPUUsage;
	double cur_cpu_time = curCPUTime;

	macedbg(1) << "server_cpu: " << cur_cpu_usage << ", upper_bound: " << upper_bound << Log::endl;
	mace::vector<mace::ContextRuntimeInfoForElasticity> sorted_cpu_contexts;

	if( cur_cpu_usage <= upper_bound ) {
		return sorted_cpu_contexts;
	}

	

	for( uint32_t i=0; i<contextRuntimeInfos.size(); i++ ) {
		mace::string ctx_type = Util::extractContextType( contextRuntimeInfos[i].contextName );
		if( selected_ctx_types.size() > 0 && selected_ctx_types.count(ctx_type) == 0 ) {
			continue;
		}

		bool insert_flag = false;
		for( mace::vector<mace::ContextRuntimeInfoForElasticity>::iterator iter = sorted_cpu_contexts.begin(); 
				iter != sorted_cpu_contexts.end(); iter ++ ) {
			if( contextRuntimeInfos[i].contextExecTime >= (*iter).contextExecTime ) {
				sorted_cpu_contexts.insert( iter, contextRuntimeInfos[i] );
				insert_flag = true;
				break;
			}
		}
		if( !insert_flag ){
			sorted_cpu_contexts.push_back( contextRuntimeInfos[i] );
		}

	}

	mace::vector<mace::ContextRuntimeInfoForElasticity> selected_contexts;
	for( uint32_t i=0; i<sorted_cpu_contexts.size(); i++ ){
		selected_contexts.push_back( sorted_cpu_contexts[i] );
		
		cur_cpu_usage = ( cur_cpu_usage*cur_cpu_time - sorted_cpu_contexts[i].contextExecTime ) / cur_cpu_time;

		macedbg(1) << "Select context("<< sorted_cpu_contexts[i].contextName <<") for migration! Now cpu_usage=" << cur_cpu_usage << Log::endl;
		if( cur_cpu_usage <= upper_bound ) {
			break;
		}
	}

	curCPUUsage = cur_cpu_usage;
	curCPUTime = cur_cpu_time;
	
	return selected_contexts;
}

mace::vector<mace::ElasticityBehaviorAction> mace::LEMReportInfo::balanceServerCPUUsage( const double& upper_bound, 
		const double& lower_bound, mace::vector<mace::ContextRuntimeInfoForElasticity>& mig_ctx_cands, const uint8_t& rule_priority ) {
	ADD_SELECTORS("LEMReportInfo::balanceServerCPUUsage");
	mace::vector<mace::ElasticityBehaviorAction> mactions;

	double cur_cpu_usage = this->curCPUUsage;
	double cur_cpu_time = this->curCPUTime;

	if( mig_ctx_cands.size() == 0 || cur_cpu_usage >= upper_bound ) {
		return mactions;
	}

	macedbg(1) << "To balance cpu usage on " << this->fromAddress << " with " << serverRuntimeInfo << Log::endl;

	mace::set<mace::string> selected_ctx_names;

	for( uint32_t i=0; i<mig_ctx_cands.size(); i++ ) {
		const mace::ContextRuntimeInfoForElasticity& ctx_rt_info = mig_ctx_cands[i];
		double miged_cpu_usage = ( cur_cpu_time*cur_cpu_usage + ctx_rt_info.contextExecTime ) / cur_cpu_time;
		macedbg(1) << "Accept context("<< ctx_rt_info.contextName <<") with execution time("<< ctx_rt_info.contextExecTime <<"), cpu usage: " << miged_cpu_usage << Log::endl;

		if( miged_cpu_usage <= upper_bound ) {
			selected_ctx_names.insert( ctx_rt_info.contextName );
			cur_cpu_usage = miged_cpu_usage;

			mace::ElasticityBehaviorAction maction( mace::ElasticityBehaviorAction::EBACTION_MIGRATION, 
				mace::ElasticityBehaviorAction::RULE_CPU_BALANCE, rule_priority, mace::ElasticityBehaviorAction::UNDEF, ctx_rt_info.currAddr, this->fromAddress, 
				ctx_rt_info.contextName, ctx_rt_info.contextExecTime );
			mactions.push_back(maction);

			macedbg(1) << "Generating: " << maction << Log::end;

			toAcceptContexts.push_back(ctx_rt_info);
		} 
	}

	this->curCPUUsage = cur_cpu_usage;
	this->curCPUTime = cur_cpu_time;

	bool to_continue = true;
	while(to_continue){
		to_continue = false;
		for( mace::vector<mace::ContextRuntimeInfoForElasticity>::iterator iter = mig_ctx_cands.begin(); iter != mig_ctx_cands.end(); iter ++ ) {
			if( selected_ctx_names.count( (*iter).contextName )  > 0 ){
				mig_ctx_cands.erase(iter);
				to_continue = true;
				break;
			}
		}
	}

	return mactions;
}

mace::vector<mace::ContextRuntimeInfoForElasticity> mace::LEMReportInfo::getContextRuntimeInfos(const mace::set<mace::string>& ctx_types) const {
	mace::vector<mace::ContextRuntimeInfoForElasticity> ctx_runtime_infos;

	for( uint32_t i=0; i<contextRuntimeInfos.size(); i++ ){
		if( ctx_types.size() == 0 || ctx_types.count(Util::extractContextType(contextRuntimeInfos[i].contextName))>0 ){
			ctx_runtime_infos.push_back( contextRuntimeInfos[i] );
		}
	}

	// for( uint32_t i=0; i<toAcceptContexts.size(); i++ ){
	// 	if( ctx_types.size() == 0 || ctx_types.count(Util::extractContextType(toA[i].contextName))>0 ){
	// 		ctx_runtime_infos.push_back( contextRuntimeInfos[i] );
	// 	}
	// }

	return ctx_runtime_infos;
}

/******************************* class ElasticityRuleInfo ***********************************************************/
void mace::ElasticityRuleInfo::print(std::ostream& out) const {
  out<< "ElasticityRuleInfo(";
  out<< "ruleType="; mace::printItem(out, &ruleType ); out<<", ";
  out<< "priority="; mace::printItem(out, &priority ); out<<", ";
  out<< "contextTypes="; mace::printItem(out, &contextTypes ); 
  out<< ")";
}


/******************************* class eMonitor ***********************************************************/
// pthread_mutex_t elasticityBehaviorActionQueueMutex = PTHREAD_MUTEX_INITIALIZER;
// std::queue< mace::ElasticityBehaviorAction* > elasticityBehaviorActionQueue;
// pthread_cond_t elasticityBehaviorSignal = PTHREAD_COND_INITIALIZER;

const uint32_t mace::eMonitor::MAJOR_HANDLER_THREAD_ID;
const double mace::eMonitor::MIGRATION_THRESHOLD_STEP;

mace::eMonitor::eMonitor( AsyncEventReceiver* sv, const uint32_t& period_time, const uint32_t& gem_waiting_time, const mace::vector<mace::MaceAddr>& gems, 
		const mace::set<mace::string>& manange_contexts, const uint32_t& server_monitor_interval_time ): 
		sv(sv), isGEM(false), GEMs(gems), periodTime(period_time), gemWaitingTime(gem_waiting_time), serverMonitorIntervalTime(server_monitor_interval_time), 
		manageContextTypes(manange_contexts) {
	
	ADD_SELECTORS("eMonitor::eMonitor");

	macedbg(1) << "periodTime=" << periodTime << ", gemWaitingTime=" << gem_waiting_time << ", GEMs=" << GEMs << Log::endl;

	eConfig = new ElasticityConfiguration();
	pthread_mutex_init( &migrationMutex, NULL );
	pthread_cond_init( &lemCond, NULL );

	if( period_time > 0 ) {
		ASSERT(  pthread_create( &lemThreadKey , NULL, eMonitor::startLEMThread, static_cast<void*>(this) ) == 0 );

		mace::MaceAddr addr = Util::getMaceAddr();
		for( uint32_t i=0; i<GEMs.size(); i++ ){
			if( addr == GEMs[i] ){
				isGEM = true;
				break;
			}
		}

		// const mace::vector<mace::MaceAddr>& server_addrs = mace::ContextMapping::serverAddrs;
		// for( uint32_t i=0; i < server_addrs.size(); i++ ) {
		// 	if( server_addrs[i] == addr ) {
		// 		mace::vector<mace::MaceAddr> copy_gems = GEMs;
		// 		GEMs.clear();

		// 		uint32_t mid = copy_gems.size() / 2;
		// 		if( i % 2 == 0 ){
		// 			for( uint32_t j = 0; j<mid; j++ ) {
		// 				GEMs.push_back( copy_gems[j] );
		// 			}
		// 		} else {
		// 			for( uint32_t j = mid; j<copy_gems.size(); j++ ) {
		// 				GEMs.push_back( copy_gems[j] );
		// 			}
		// 		}
		// 	}
		// }

		macedbg(1) << "GEMs: " << GEMs << Log::endl;
	}

	if( serverMonitorIntervalTime > 0 ) {
		ASSERT(  pthread_create( &serverThreadKey , NULL, eMonitor::startServerMonitorThread, static_cast<void*>(this) ) == 0 );
	}

	this->toAcceptLEMReportFlag = true;
	cpu_upper_bound = 100;
	cpu_lower_bound = 0;

	
	if( GEMs.size() == 0 ) {
		lastGEMAddr = SockUtil::NULL_MACEADDR;
	} else {
		lastGEMAddr = GEMs[0];
	}
}

void mace::eMonitor::addElasticityRule( const mace::ElasticityRule& rule ) {
	if( rule.ruleType == mace::ElasticityRule::RESOURCE_RULE && rule.behavior.behaviorType == mace::ElasticityBehavior::WORKLOAD_BALANCE ) {
		double upper_bound = rule.getServerCPUUpperBound();
		double lower_bound = rule.getServerCPULowerBound();
		if( upper_bound < cpu_upper_bound ) {
			cpu_upper_bound = upper_bound;
		}

		if( lower_bound > cpu_lower_bound ) {
			cpu_lower_bound = lower_bound;
		}
	}

	eConfig->addElasticityRule( rule );
}

void* mace::eMonitor::startLEMThread(void* arg) {
	ADD_SELECTORS("eMonitor::startLEMThread");
	eMonitor* monitor = static_cast<eMonitor*>(arg);
	monitor->runInfoCollection();
	return 0;
}

void* mace::eMonitor::startGEMThread(void* arg) {
	ADD_SELECTORS("eMonitor::startGEMThread");
	eMonitor* monitor = static_cast<eMonitor*>(arg);
	SysUtil::sleep(monitor->getGEMWaitingTime(), 0);
	monitor->processLEMReports();
	return 0;
}

void* mace::eMonitor::startServerMonitorThread(void* arg) {
	ADD_SELECTORS("eMonitor::startServerMonitorThread");
	eMonitor* monitor = static_cast<eMonitor*>(arg);
	
	uint64_t interval_time = monitor->getServerMonitorIntervalTime();
	if( interval_time == 0 ) {
		return 0;
	}

	std::ofstream server_file;
	server_file.open( "server-info");

	uint64_t cur_time_second = 0;

	while( true ){
		CPUInformation last_cpu_info = monitor->getCurrentCPUInfo();
		SysUtil::sleep( interval_time, 0);
		cur_time_second += interval_time;
		CPUInformation cur_cpu_info = monitor->getCurrentCPUInfo();

		double cpu_usage = monitor->computeCPUUsage( last_cpu_info, cur_cpu_info );
		macedbg(1) << "cpu_usage=" << cpu_usage << Log::endl;
	}

	server_file.close();

	return 0;
}

void mace::eMonitor::runInfoCollection() {
	ADD_SELECTORS("eMonitor::runInfoCollection");
	macedbg(1) << "Start elasticity processing thread with time period("<< periodTime <<")!" << Log::endl;
	lastcpuInfo = this->getCurrentCPUInfo();
	while( true ){		
		SysUtil::sleep(periodTime, 0);
		this->processContextElasticity();
	}
}

void mace::eMonitor::updateCPUInfo() {
	lastcpuInfo = this->getCurrentCPUInfo();
}

void mace::eMonitor::processContextElasticity( ) {
	ADD_SELECTORS("eMonitor::processContextElasticity");

	macedbg(1) << "Start to process contexts elasticity!" << Log::endl;

	currentcpuUsage = this->computeCurrentCPUUsage() * 0.01;
	double total_cpu_time = this->computeCPUTotalTime() * 0.01;

	ContextService* _service = static_cast<ContextService*>(sv);
	std::vector<mace::ContextBaseClass*> contexts;
	_service->getContextObjectsByTypes(contexts, manageContextTypes);

	macedbg(1) << "total_cpu_time: " << total_cpu_time << " CPUUsage: " << currentcpuUsage << ", n_contexts=" << contexts.size() << Log::endl;

	const mace::ContextMapping& snapshot = _service->getLatestContextMapping();
	const ContextStructure& ctxStructure = _service->contextStructure;

	ScopedLock sl(migrationMutex);
	predictCPUTotalTime = total_cpu_time;
	predictCPUTime = predictCPUTotalTime * currentcpuUsage;
	
	contextMigrationQueries.clear();
	contextsRuntimeInfo.clear();

	contextMigrationRequests.clear();
	migratingContextNames.clear();
	
	predictLocalContexts.clear();
	readyToProcessMigrationQuery = false;
	migrationQueryThreadIsIdle = true;

	double total_ctx_exec_time = 0.0;

	for( uint32_t i=0; i<contexts.size(); i++ ) {
		ContextRuntimeInfoForElasticity ctx_runtime_info;
		ctx_runtime_info.contextName = contexts[i]->contextName;
		ctx_runtime_info.currAddr = Util::getMaceAddr();
		ctx_runtime_info.contextExecTime = contexts[i]->getCPUTime() * 0.000001;
		ctx_runtime_info.fromAccessCount = contexts[i]->getFromAccessCountByTypes(manageContextTypes);
		ctx_runtime_info.toAccessCount = contexts[i]->getToAccessCountByTypes(manageContextTypes);

		ctx_runtime_info.externalMessageCount = contexts[i]->getExternalMessageCount();

		// macedbg(1) << "context("<< ctx_runtime_info.contextName <<") externalMessageCount=" << ctx_runtime_info.externalMessageCount << Log::endl;

		total_ctx_exec_time += ctx_runtime_info.contextExecTime;
		
		contextsRuntimeInfo[ contexts[i]->contextName ] = ctx_runtime_info;
		(contexts[i]->runtimeInfo).clear();
		contexts[i] = NULL;
	}

	uint32_t n_contexts = contexts.size();
	contexts.clear();
	sl.unlock();

	serverRuntimeInfo.CPUUsage = currentcpuUsage;
	serverRuntimeInfo.totalCPUTime = total_cpu_time;
	serverRuntimeInfo.contextsNumber = n_contexts;

	macedbg(1) << "Server's CPU usage: " << currentcpuUsage << Log::endl;

	for( std::map< mace::string, ContextRuntimeInfoForElasticity >::iterator iter = contextsRuntimeInfo.begin(); iter != contextsRuntimeInfo.end(); iter ++ ) {
		ContextRuntimeInfoForElasticity& ctx_rt_info = iter->second;
		double adj_ctx_exec_time = total_cpu_time * ctx_rt_info.contextExecTime / total_ctx_exec_time;
		if( adj_ctx_exec_time < ctx_rt_info.contextExecTime ) {
			ctx_rt_info.contextExecTime = adj_ctx_exec_time;
		}
	}

	lastcpuInfo = this->getCurrentCPUInfo();

	bool return_here = false;
	if( return_here ) {
		return;
	}

	// process actor elasticity rules
	const mace::vector< mace::ElasticityRule >& rules = eConfig->getElasticityRules();
	for( std::map< mace::string, ContextRuntimeInfoForElasticity >::iterator iter = contextsRuntimeInfo.begin(); 
			iter != contextsRuntimeInfo.end(); iter ++ ){
		ContextRuntimeInfoForElasticity& ctx_rt_info = iter->second;

  		for( mace::vector< mace::ElasticityRule >::const_iterator iter = rules.begin(); iter != rules.end(); iter ++ ) {
    		const mace::ElasticityRule& rule = *iter;
    		if( rule.ruleType == mace::ElasticityRule::ACTOR_RULE && rule.relatedContext(ctx_rt_info.contextName) ) {
      			mace::ElasticityBehaviorAction* maction = rule.proposeMigrationAction( ctx_rt_info, snapshot, ctxStructure );
      			if( maction != NULL ){
        			if( mactions.find(ctx_rt_info.contextName) == mactions.end() ) {
          				mactions[ctx_rt_info.contextName] = maction;
        			} else {
          				if( mactions[ctx_rt_info.contextName]->priority > maction->priority ) {
            				delete maction;
          				} else {
            				delete mactions[ctx_rt_info.contextName];
            				mactions[ctx_rt_info.contextName] = maction;
            				macedbg(1) << "Replace with: " << *maction << Log::endl;
          				}
        			}
      			}
    		}
  		}

	}

	// check if migration action is reasonable
	// mace::map<mace::string, mace::MaceAddr> adjust_contexts;
	// mace::vector<mace::string> pin_contexts;
	// for( std::map<mace::string, mace::ElasticityBehaviorAction*>::iterator iter = mactions.begin(); iter != mactions.end(); iter ++ ){
	// 	if( (iter->second)->ruleType == mace::ElasticityBehaviorAction::RULE_METHOD_COUNT ) {
	// 		const ContextRuntimeInfoForElasticity& ctx_rt_info = contextsRuntimeInfo[ iter->first ];
	// 		uint64_t ret_inter_count = ctx_rt_info.getNodeInterCount( snapshot, (iter->second)->toNode, adjust_contexts);
	// 		uint64_t loc_inter_count = ctx_rt_info.getNodeInterCount( snapshot, Util::getMaceAddr(), adjust_contexts);

	// 		if( ret_inter_count > loc_inter_count ) {
	// 			adjust_contexts[ iter->first ] = (iter->second)->toNode;
	// 		} else {
	// 			pin_contexts.push_back( iter->first );
	// 		}
	// 	} else {
	// 		pin_contexts.push_back( iter->first );
	// 	}
	// }
	
	// for( uint32_t i=0; i<pin_contexts.size(); i++ ) {
	// 	macedbg(1) << "Delete " << *mactions[ pin_contexts[i] ] << Log::endl; 
	// 	delete mactions[ pin_contexts[i] ];
	// 	mactions[ pin_contexts[i] ] = NULL;
	// 	mactions.erase( pin_contexts[i] );
	// }


	// process resource elasticity rules
	mace::vector<ContextRuntimeInfoForElasticity> report_ctx_rt_infos;
	mace::set<mace::string> report_ctx_names;

	// if( serverRuntimeInfo.CPUUsage > 0.8 ){
	// 	macedbg(1) << "The server is overloaded!" << Log::endl; 
	// 	mace::vector<ContextRuntimeInfoForElasticity> sorted_contexts;
	// 	for( std::map<mace::string, ContextRuntimeInfoForElasticity>::const_iterator iter = contextsRuntimeInfo.begin(); iter != contextsRuntimeInfo.end(); iter++ ){
	// 		if( Util::extractContextType(iter->first) != "DirNode" ) {
	// 			continue;
	// 		}
	// 		bool inserted = false;
	// 		for( mace::vector<ContextRuntimeInfoForElasticity>::iterator siter = sorted_contexts.begin(); siter != sorted_contexts.end(); siter++ ) {
	// 			if( (*siter).externalMessageCount <= (iter->second).externalMessageCount ) {
	// 				sorted_contexts.insert(siter, iter->second);
	// 				inserted = true;
	// 				break;
	// 			}
	// 		}	

	// 		if( !inserted ){
	// 			sorted_contexts.push_back(iter->second);
	// 		}
	// 	}

	// 	for(uint32_t i=0; i<sorted_contexts.size(); i++) {
	// 		double d = i+1;
	// 		if( d/sorted_contexts.size() <= 0.1 || i == 0 ) {
	// 			macedbg(1) << "Report context(" << sorted_contexts[i].contextName << ")!" << Log::endl;
	// 			report_ctx_rt_infos.push_back( sorted_contexts[i] );
	// 		} else {
	// 			break;
	// 		}
	// 	}
	// }

	
	for( mace::vector< mace::ElasticityRule >::const_iterator iter = rules.begin(); iter != rules.end(); iter ++ ) {
    	const mace::ElasticityRule& rule = *iter;
    	if( rule.ruleType == mace::ElasticityRule::RESOURCE_RULE ) {
      		mace::vector<ContextRuntimeInfoForElasticity> ctx_rt_infos = rule.aggregateContextRuntimeInfos( serverRuntimeInfo, contextsRuntimeInfo );

      		for( uint32_t i=0; i<ctx_rt_infos.size(); i++ ) {
      			if( report_ctx_names.count(ctx_rt_infos[i].contextName) == 0 ) {
      				report_ctx_rt_infos.push_back( ctx_rt_infos[i] );
      				report_ctx_names.insert( ctx_rt_infos[i].contextName );
      			}
      		}
      	}
  	}



  	macedbg(1) << "To pick up a GEM randomly!" << Log::endl;
  	uint32_t rIter = RandomUtil::randInt() % GEMs.size();
  	lastGEMAddr = GEMs[rIter];
  	macedbg(1) << "Report runtime information to GEM("<< lastGEMAddr <<") with "<< report_ctx_rt_infos.size() <<" contexts!" << Log::endl;
	_service->reportLEMInfos( lastGEMAddr, serverRuntimeInfo, report_ctx_rt_infos );

	if(isGEM) {
		macedbg(1) << "I am a GEM!!" << Log::endl;
		ASSERT(  pthread_create( &gemThreadKey , NULL, eMonitor::startGEMThread, static_cast<void*>(this) ) == 0 );
	}

	sl.lock();
	pthread_cond_wait( &lemCond, &migrationMutex );
}

void mace::eMonitor::enqueueContextMigrationQuery( const mace::MaceAddr& src, 
		const mace::vector<mace::ElasticityBehaviorAction>& query_mactions, const ServerRuntimeInfo& server_info ) {
	ADD_SELECTORS("eMonitor::enqueueContextMigrationQuery");
	ScopedLock sl(migrationMutex);
	macedbg(1) << "Receive context migration query from " << src << ", query: " << query_mactions << Log::endl;
	ContextMigrationQuery query(src, query_mactions, server_info);
	contextMigrationQueries.push_back( query );
	sl.unlock();

	processContextsMigrationQuery();
}

void mace::eMonitor::processContextsMigrationQuery() {
	ADD_SELECTORS("eMonitor::processContextsMigrationQuery");
	ContextService* _service = static_cast<ContextService*>(sv);
	// const mace::ContextMapping& snapshot = _service->getLatestContextMapping();
		
	ScopedLock sl(migrationMutex);
	while( contextMigrationQueries.size() > 0 && readyToProcessMigrationQuery && migrationQueryThreadIsIdle ){
		ContextMigrationQuery query = contextMigrationQueries[0];
		macedbg(1) << "Process migration query from " << query.srcAddr << " with "<< query.queryMactions << Log::endl;

		contextMigrationQueries.erase( contextMigrationQueries.begin() );
		migrationQueryThreadIsIdle = false;
		sl.unlock();

		const mace::MaceAddr& src = query.srcAddr;
		const mace::vector<mace::ElasticityBehaviorAction>& query_mactions = query.queryMactions;
		const ServerRuntimeInfo& server_info = query.serverInfo;

		mace::vector<mace::string> accept_m_ctxs;

		for( mace::vector<mace::ElasticityBehaviorAction>::const_iterator iter = query_mactions.begin(); iter != query_mactions.end();
				iter++ ) {
			const mace::ElasticityBehaviorAction& maction = *iter;
			if( maction.actionType == mace::ElasticityBehaviorAction::EBACTION_COLOCATE && maction.specialRequirement != mace::ElasticityBehavior::REQ_PIN_ONE_ACTOR ) {
				if( serverRuntimeInfo.CPUUsage > server_info.CPUUsage  ) {
					continue;
				} else if( serverRuntimeInfo.CPUUsage == server_info.CPUUsage && src < Util::getMaceAddr() ) {
					continue;
				}
			}


			// if( maction.ruleType == mace::ElasticityBehaviorAction::RULE_METHOD_COUNT ) {
			// 	mace::set<mace::string> empty_set;
			// 	uint64_t src_node_method_count = maction.getNodeMethodCount( maction.fromNode, empty_set, snapshot );
			// 	uint64_t dest_node_method_count = maction.getNodeMethodCount( Util::getMaceAddr(), predictLocalContexts, snapshot );

			// 	if( src_node_method_count >= dest_node_method_count ) {
			// 		continue;
			// 	}
			// }

			if( hasEnoughResource( maction ) ) {
				this->toAcceptContext( maction );
				accept_m_ctxs.push_back( maction.contextName );
			}
		}
			
		if( accept_m_ctxs.size() > 0 ){
			macedbg(1) << "Accept contexts: " << accept_m_ctxs << Log::endl;
			_service->send__elasticity_contextMigrationQueryReply( src, accept_m_ctxs);
		}
		
		sl.lock();
		migrationQueryThreadIsIdle = true;
	}
}

void mace::eMonitor::enqueueLEMReport( const ServerRuntimeInfo& server_info, const mace::vector<mace::ContextRuntimeInfoForElasticity>& ctx_rt_infos,
    	const mace::MaceAddr& src ) {
	ADD_SELECTORS("eMonitor::enqueueLEMReport");
	ScopedLock sl(migrationMutex);
	if( toAcceptLEMReportFlag ) {
		mace::LEMReportInfo lem_report( server_info, ctx_rt_infos, src );
		lemReports.push_back( lem_report );
		macedbg(1) << "Enqueue runtime report("<< server_info <<") from " << src << ",  total LEM report number: " << lemReports.size() << Log::endl;
	} else {
		ContextService* _service = static_cast<ContextService*>(sv);
		mace::vector<ElasticityBehaviorAction> mactions;
		_service->replyLEMReport( src, mactions );
		macedbg(1) << "Ingore runtime report from " << src << Log::endl;
	}
}

void mace::eMonitor::processLEMReportReply( const mace::vector<mace::ElasticityBehaviorAction>& orig_gem_mactions ) {
	ADD_SELECTORS("eMonitor::processLEMReportReply");

	macedbg(1) << "Receive LEM report reply: " << orig_gem_mactions << Log::endl;
	ContextService* _service = static_cast<ContextService*>(sv);

	const ContextStructure& ctxStructure = _service->contextStructure;
	const mace::ContextMapping& snapshot = _service->getLatestContextMapping();

	mace::vector<mace::ElasticityBehaviorAction> gem_mactions;

	for( uint32_t i=0; i<orig_gem_mactions.size(); i++ ) {
		const mace::ElasticityBehaviorAction& maction = orig_gem_mactions[i];
		gem_mactions.push_back( maction );

		if( maction.specialRequirement == mace::ElasticityBehaviorAction::REQ_COL_CHILDREN ) {
			mace::vector<mace::string> children = ctxStructure.getAllChildContexts( maction.contextName );
			for( uint32_t j=0; j<children.size(); j++ ) {
				if( mace::ContextMapping::getNodeByContext(snapshot, children[j]) == Util::getMaceAddr() ) {
					mace::ElasticityBehaviorAction cmaction( mace::ElasticityBehaviorAction::EBACTION_MIGRATION, maction.ruleType, maction.priority, mace::ElasticityBehaviorAction::UNDEF,
						Util::getMaceAddr(), maction.toNode, children[j], contextsRuntimeInfo[ children[j] ].contextExecTime  );
					gem_mactions.push_back(cmaction);
				}
			}
		}
	}

	mace::vector<mace::ElasticityBehaviorAction> final_mactions;
	for( uint32_t i=0; i<gem_mactions.size(); i++ ) {
		const mace::string& ctx_name = gem_mactions[i].contextName;
		if( mactions.find(ctx_name) != mactions.end() ) {
			if( mactions[ctx_name]->priority > gem_mactions[i].priority ) {
				final_mactions.push_back(*mactions[ctx_name]);
			} else {
				final_mactions.push_back( gem_mactions[i] );
			}

			macedbg(1) << "Ignore action: " << gem_mactions[i] << Log::endl;
			delete mactions[ctx_name];
			mactions[ctx_name] = NULL;
			mactions.erase( ctx_name );
		} else {
			final_mactions.push_back( gem_mactions[i] );
		}
	}

	for( std::map<mace::string, mace::ElasticityBehaviorAction*>::iterator iter = mactions.begin(); iter != mactions.end(); iter ++ ) {
		final_mactions.push_back( *(iter->second) );
		delete iter->second;
		iter->second = NULL;
	}
	mactions.clear();

	mace::map<mace::MaceAddr, mace::vector<mace::ElasticityBehaviorAction> > query_mactions;
	for( uint32_t i=0; i<final_mactions.size(); i++ ) {
		query_mactions[ final_mactions[i].toNode ].push_back( final_mactions[i] );
	}

	macedbg(1) << "Query: " << query_mactions << Log::endl;
	for( mace::map<mace::MaceAddr, mace::vector<mace::ElasticityBehaviorAction> >::iterator iter = query_mactions.begin(); 
			iter != query_mactions.end(); iter ++ ) {
		_service->send__elasticity_contextMigrationQuery( iter->first, iter->second, serverRuntimeInfo );
	}


	ScopedLock sl(migrationMutex);
	readyToProcessMigrationQuery = true;
	pthread_cond_signal(&lemCond);
	sl.unlock();

	this->processContextsMigrationQuery();
}

void mace::eMonitor::processContextsMigrationQueryReply( const mace::MaceAddr& dest, const mace::vector<mace::string>& accept_m_contexts ) {
	ADD_SELECTORS("eMonitor::processContextsMigrationQueryReply");
	for( uint64_t i=0; i<accept_m_contexts.size(); i++ ){
		this->requestContextMigration( accept_m_contexts[i], dest );
	}
}

mace::CPUInformation mace::eMonitor::getCurrentCPUInfo() const {
	CPUInformation cpuInfo;

	FILE* file = fopen("/proc/stat", "r");
	unsigned long long int user, low, sys, idle;
    fscanf(file, "cpu %llu %llu %llu %llu", &user, &low, &sys, &idle);
    fclose(file);

    // Time units are in USER_HZ or Jiffies (typically hundredths of a second)
    cpuInfo.totalUserCPUTime = (uint64_t)user;
    cpuInfo.totalUserLowCPUTime = (uint64_t)low;
    cpuInfo.totalSysCPUTime = (uint64_t)sys;
    cpuInfo.totalIdleCPUTime = (uint64_t)idle;

    return cpuInfo;
}

double mace::eMonitor::computeCurrentCPUUsage() const {
	CPUInformation cpuInfo = this->getCurrentCPUInfo();
	double percent = 0.0;

	if ( cpuInfo.totalUserCPUTime < lastcpuInfo.totalUserCPUTime || cpuInfo.totalUserLowCPUTime < lastcpuInfo.totalUserLowCPUTime ||
    		cpuInfo.totalSysCPUTime < lastcpuInfo.totalSysCPUTime || cpuInfo.totalIdleCPUTime < lastcpuInfo.totalIdleCPUTime ){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    } else{
        percent = (cpuInfo.totalUserCPUTime - lastcpuInfo.totalUserCPUTime) + (cpuInfo.totalUserLowCPUTime - lastcpuInfo.totalUserLowCPUTime)
    		+ (cpuInfo.totalSysCPUTime - lastcpuInfo.totalSysCPUTime);
        double total = percent + ( cpuInfo.totalIdleCPUTime - lastcpuInfo.totalIdleCPUTime );
        percent = (percent / total) * 100;
    }

    return percent;
}

double mace::eMonitor::computeCPUUsage( const CPUInformation& lastcpuInfo, const CPUInformation& cpuInfo ) {
	double percent = 0.0;

	if ( cpuInfo.totalUserCPUTime < lastcpuInfo.totalUserCPUTime || cpuInfo.totalUserLowCPUTime < lastcpuInfo.totalUserLowCPUTime ||
    		cpuInfo.totalSysCPUTime < lastcpuInfo.totalSysCPUTime || cpuInfo.totalIdleCPUTime < lastcpuInfo.totalIdleCPUTime ){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    } else{
        percent = (cpuInfo.totalUserCPUTime - lastcpuInfo.totalUserCPUTime) + (cpuInfo.totalUserLowCPUTime - lastcpuInfo.totalUserLowCPUTime)
    		+ (cpuInfo.totalSysCPUTime - lastcpuInfo.totalSysCPUTime);
        double total = percent + ( cpuInfo.totalIdleCPUTime - lastcpuInfo.totalIdleCPUTime );
        percent = (percent / total) * 100;
    }

    return percent;
}

uint64_t mace::eMonitor::computeCPUTotalTime() const {
	CPUInformation cpuInfo = this->getCurrentCPUInfo();

	uint64_t total_cpu_time = (cpuInfo.totalUserCPUTime - lastcpuInfo.totalUserCPUTime) + (cpuInfo.totalUserLowCPUTime - lastcpuInfo.totalUserLowCPUTime)
    	+ (cpuInfo.totalSysCPUTime - lastcpuInfo.totalSysCPUTime) + ( cpuInfo.totalIdleCPUTime - lastcpuInfo.totalIdleCPUTime );
    return total_cpu_time;
}

double mace::eMonitor::computeCurrentMemUsage() const {
	struct sysinfo memInfo;

	sysinfo(&memInfo);
	uint64_t totalVirtualMem = memInfo.totalram;
	//Add other values in next statement to avoid int overflow on right hand side...
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;

	uint64_t virtualMemUsed = memInfo.totalram - memInfo.freeram;
	//Add other values in next statement to avoid int overflow on right hand side...
	virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
	virtualMemUsed *= memInfo.mem_unit;

	double percent = (virtualMemUsed / totalVirtualMem) * 100;
	return percent;
}

void mace::eMonitor::requestContextMigration( const mace::string& contextName, const MaceAddr& destNode ){
  	ADD_SELECTORS("eMonitor::requestContextMigration");
  	macedbg(1) << "Migrating context " << contextName << " to " << destNode <<Log::endl;

  	ContextService* _service = static_cast<ContextService*>(sv);
  	const ContextMapping& snapshot = _service->getLatestContextMapping();

  	mace::MaceAddr addr = mace::ContextMapping::getNodeByContext(snapshot, contextName);
  	if( addr == destNode ) {
  		macedbg(1) << "Context("<< contextName <<") is already on node("<< destNode <<")!" << Log::endl;
  		return;
  	} else if( addr != Util::getMaceAddr() ) {
  		_service->send__migration_contextMigrationRequest( contextName, destNode );
  		return;
  	}
  	uint32_t contextId = mace::ContextMapping::hasContext2( snapshot, contextName );
  	ASSERT( contextId > 0 );

  	ScopedLock sl(migrationMutex);
  	if( contextMigrationRequests.find(contextName) != contextMigrationRequests.end() ){
  		maceout << "context("<< contextName <<") is already in the migration waiting list!" << Log::endl;
  		return;
  	} 

  	MigrationRequest request;
  	request.contextName = contextName;
  	request.destNode = destNode;
  	contextMigrationRequests[contextName] = request;

  	migratingContextNames.push_back( contextName );
  	sl.unlock();

  	this->processContextMigration();
}

void mace::eMonitor::processContextMigration(){
	ADD_SELECTORS("eMonitor::processContextMigration");
	ScopedLock sl(migrationMutex);
	if( migratingContextNames.size() > 0 && migratingContextName == "" ) {
		mace::string ctx_name = migratingContextNames[0];
		migratingContextName = ctx_name;

		migratingContextNames.erase( migratingContextNames.begin() );
		ASSERT( contextMigrationRequests.find(ctx_name) != contextMigrationRequests.end() );
		const MigrationRequest& request = contextMigrationRequests[ctx_name];
		sl.unlock();

		macedbg(1) << "Start to migrate context("<< request.contextName <<") to " << request.destNode << Log::endl;

		ContextService* _service = static_cast<ContextService*>(sv);
  		const ContextMapping& snapshot = _service->getLatestContextMapping();

  		uint32_t contextId = mace::ContextMapping::hasContext2( snapshot, ctx_name );

  		_service->handleContextMigrationRequest( request.destNode, request.contextName, contextId );
	}
}

void mace::eMonitor::wrapupCurrentContextMigration() {
	ADD_SELECTORS("eMonitor::wrapupCurrentContextMigration");
	macedbg(1) << "Finish context("<< migratingContextName <<") migration!" << Log::endl;
	ScopedLock sl(migrationMutex);
	ASSERT( migratingContextName != "" );	
	contextMigrationRequests.erase( migratingContextName );
	migratingContextName = "";
	sl.unlock();

	this->processContextMigration();
}

bool mace::eMonitor::checkContextCreateRule( const mace::string& ctx_name ) const {
	if( eConfig->checkContextCreateRule(ctx_name) ) {
		return true;
	} else {
		return false;
	}
}

mace::MaceAddr mace::eMonitor::getNewContextAddr( const mace::string& ctx_name ) {
	ADD_SELECTORS("eMonitor::getNewContextAddr");

	mace::string ctx_type = Util::extractContextType( ctx_name );

	ElasticityRuleInfo rule_info = eConfig->getContextCreateRule( ctx_type );
	macedbg(1) << "context("<< ctx_name <<")'s related rule: " << rule_info << Log::endl; 

	ContextService* _service = static_cast<ContextService*>(sv);
	const mace::ContextMapping& snapshot = _service->getLatestContextMapping();

	MaceAddr dest_addr = SockUtil::NULL_MACEADDR;

	if( rule_info.ruleType == mace::ElasticityRuleInfo::BALANCE_SERVER_CPU_USAGE ) {
		double cpu_usage = 1.0;
		for( uint32_t i=0; i<latestLEMReports.size(); i++ ) {
			if( latestLEMReports[i].serverRuntimeInfo.CPUUsage < cpu_usage ) {
				cpu_usage = latestLEMReports[i].serverRuntimeInfo.CPUUsage;
				dest_addr = latestLEMReports[i].fromAddress;
			}
		}
	} else if( rule_info.ruleType == mace::ElasticityRuleInfo::REFERENCE_COLOCATE ) {
		const ContextStructure& ctxStructure = _service->contextStructure;

		mace::map<mace::MaceAddr, uint32_t> server_count; 

		mace::vector<mace::string> parents = ctxStructure.getAllParentContexts( ctx_name );
		macedbg(1) << "context("<< ctx_name <<")'s parents: " << parents << Log::endl;
		for( uint32_t i=0; i<parents.size(); i++ ) {
			mace::string p_ctx_type = Util::extractContextType( parents[i] );
			if( rule_info.contextTypes.count(p_ctx_type) > 0 ) {
				const mace::MaceAddr& addr = mace::ContextMapping::getNodeByContext(snapshot, parents[i]);
				if( server_count.find(addr) != server_count.end() ) {
					server_count[addr] ++;
				} else {
					server_count[addr] = 1;
				}
			}
		}

		mace::vector<mace::string> children = ctxStructure.getAllChildContexts( ctx_name );
		macedbg(1) << "context("<< ctx_name <<")'s children: " << children << Log::endl;
		for( uint32_t i=0; i<children.size(); i++ ) {
			mace::string c_ctx_type = Util::extractContextType( children[i] );
			if( rule_info.contextTypes.count(c_ctx_type) > 0 ) {
				const mace::MaceAddr& addr = mace::ContextMapping::getNodeByContext(snapshot, children[i]);
				if( server_count.find(addr) != server_count.end() ) {
					server_count[addr] ++;
				} else {
					server_count[addr] = 1;
				}
			}
		}

		uint32_t max_count = 0;
		for( mace::map<mace::MaceAddr, uint32_t>::iterator iter = server_count.begin(); iter != server_count.end(); iter ++ ) {
			if( max_count == 0 || max_count < iter->second ){
				dest_addr = iter->first;
				max_count = iter->second;
			}
		}

		macedbg(1) << "To colocate context("<< ctx_name <<") with its parents on " << dest_addr << Log::endl;
	}

	return dest_addr;
}

mace::MaceAddr mace::eMonitor::getNewContextAddrForActorRules( const mace::string& ctx_name ) {
	ADD_SELECTORS("eMonitor::getNewContextAddrForActorRules");

	mace::string ctx_type = Util::extractContextType( ctx_name );

	ElasticityRuleInfo rule_info = eConfig->getContextCreateRule( ctx_type );
	macedbg(1) << "context("<< ctx_name <<")'s related rule: " << rule_info << Log::endl; 

	ContextService* _service = static_cast<ContextService*>(sv);
	const mace::ContextMapping& snapshot = _service->getLatestContextMapping();

	MaceAddr dest_addr = SockUtil::NULL_MACEADDR;

	if( rule_info.ruleType == mace::ElasticityRuleInfo::REFERENCE_COLOCATE ) {
		const ContextStructure& ctxStructure = _service->contextStructure;

		mace::map<mace::MaceAddr, uint32_t> server_count; 

		mace::vector<mace::string> parents = ctxStructure.getAllParentContexts( ctx_name );
		macedbg(1) << "context("<< ctx_name <<")'s parents: " << parents << Log::endl;
		for( uint32_t i=0; i<parents.size(); i++ ) {
			mace::string p_ctx_type = Util::extractContextType( parents[i] );
			if( rule_info.contextTypes.count(p_ctx_type) > 0 ) {
				const mace::MaceAddr& addr = mace::ContextMapping::getNodeByContext(snapshot, parents[i]);
				if( server_count.find(addr) != server_count.end() ) {
					server_count[addr] ++;
				} else {
					server_count[addr] = 1;
				}
			}
		}

		mace::vector<mace::string> children = ctxStructure.getAllChildContexts( ctx_name );
		macedbg(1) << "context("<< ctx_name <<")'s children: " << children << Log::endl;
		for( uint32_t i=0; i<children.size(); i++ ) {
			mace::string c_ctx_type = Util::extractContextType( children[i] );
			if( rule_info.contextTypes.count(c_ctx_type) > 0 ) {
				const mace::MaceAddr& addr = mace::ContextMapping::getNodeByContext(snapshot, children[i]);
				if( server_count.find(addr) != server_count.end() ) {
					server_count[addr] ++;
				} else {
					server_count[addr] = 1;
				}
			}
		}

		uint32_t max_count = 0;
		for( mace::map<mace::MaceAddr, uint32_t>::iterator iter = server_count.begin(); iter != server_count.end(); iter ++ ) {
			if( max_count == 0 || max_count < iter->second ){
				dest_addr = iter->first;
				max_count = iter->second;
			}
		}

		macedbg(1) << "To colocate context("<< ctx_name <<") with its parents on " << dest_addr << Log::endl;
	}

	return dest_addr;
}

void mace::eMonitor::processLEMReports() {
	ADD_SELECTORS("eMonitor::processLEMReports");
	macedbg(1) << "Start to process LEM reports!" << Log::endl;

	ScopedLock sl(migrationMutex);
	toAcceptLEMReportFlag = false;
	sl.unlock();

	mace::vector<mace::ContextRuntimeInfoForElasticity> mig_ctx_cands;
	mace::map< MaceAddr, mace::vector<ElasticityBehaviorAction> > mig_ctx_addrs;

	mace::set<mace::string> chosen_contexts;

	mace::map<mace::string, mace::MaceAddr > ctx_addrs;
	mace::map<mace::MaceAddr, mace::ServerRuntimeInfo> server_infos;


	// for( uint32_t m=0; m<lemReports.size(); m++ ) {
	// 	const mace::vector<mace::ContextRuntimeInfoForElasticity>& mig_ctx_cands1 = lemReports[m].contextRuntimeInfos;
	// 	for( uint32_t j=0; j<mig_ctx_cands1.size(); j++ ) {
	// 		ctx_addrs[ mig_ctx_cands1[j].contextName ] = lemReports[m].fromAddress;

	// 		bool insert_flag = false;
	// 		for( mace::vector<mace::ContextRuntimeInfoForElasticity>::iterator iter = mig_ctx_cands.begin(); iter!=mig_ctx_cands.end(); iter ++ ) {
	// 			if( mig_ctx_cands1[j].externalMessageCount >= (*iter).externalMessageCount ) {
	// 				mig_ctx_cands.insert(iter, mig_ctx_cands1[j] );
	// 				insert_flag = true;
	// 				break;					
	// 			}
	// 		}

	// 		if( !insert_flag ){ 
	// 			mig_ctx_cands.push_back( mig_ctx_cands1[j] );
	// 		}

	// 	}

	// 	server_infos[lemReports[m].fromAddress] = lemReports[m].serverRuntimeInfo;
	// }
	
	// macedbg(1) << "Candidate migration contexts: " << mig_ctx_cands << Log::endl;

	// while( mig_ctx_cands.size() > 0 ) {
	// 	const ContextRuntimeInfoForElasticity& ctx_rt_info = mig_ctx_cands[0];

	// 	mace::MaceAddr addr = SockUtil::NULL_MACEADDR;
	// 	double cpu_usage = 1.0;
	// 	for( mace::map<mace::MaceAddr, ServerRuntimeInfo>::iterator iter = server_infos.begin(); iter != server_infos.end(); iter ++ ) {
	// 		macedbg(1) << "server("<< iter->first <<")'s usage=" << (iter->second).CPUUsage << ", min_usgae=" << cpu_usage << ", context("<< ctx_rt_info.contextName <<") from " << ctx_addrs[ ctx_rt_info.contextName ] << Log::endl;
	// 		if( (iter->second).CPUUsage < cpu_usage && iter->first != ctx_addrs[ ctx_rt_info.contextName ] ) {
	// 			cpu_usage = (iter->second).CPUUsage;
	// 			addr = iter->first;
	// 		}
	// 	}

	// 	if( addr != SockUtil::NULL_MACEADDR ) {
	// 		uint8_t spec_req = mace::ElasticityBehaviorAction::REQ_COL_CHILDREN;
				
	// 		ElasticityBehaviorAction maction( mace::ElasticityBehaviorAction::EBACTION_MIGRATION, mace::ElasticityBehaviorAction::UNDEF, 9, spec_req, 
	// 			ctx_addrs[ ctx_rt_info.contextName ], addr,  ctx_rt_info.contextName, ctx_rt_info.contextExecTime  );
	// 		mig_ctx_addrs[ ctx_addrs[ctx_rt_info.contextName]  ].push_back( maction );

	// 		server_infos[addr].CPUUsage = ( server_infos[addr].CPUUsage * server_infos[addr].totalCPUTime + ctx_rt_info.contextExecTime  ) / server_infos[addr].totalCPUTime;
	// 	} else {
	// 		macedbg(1) << "Fail to find new server for " << ctx_rt_info << Log::endl;
	// 	}

	// 	mig_ctx_cands.erase( mig_ctx_cands.begin() );			
	// }


	const mace::vector<mace::ElasticityRule>& rules = eConfig->getElasticityRules();
	for( uint64_t i=0; i<rules.size(); i++ ){
		if( rules[i].ruleType != mace::ElasticityRule::RESOURCE_RULE ){
			continue;
		} 

		mig_ctx_cands.clear();

		if( rules[i].behavior.behaviorType == ElasticityBehavior::WORKLOAD_BALANCE && rules[i].behavior.resourceType == mace::ElasticityBehavior::RES_CPU ) {
			const mace::set<mace::string>& selected_ctx_types = rules[i].relatedContextTypes;
			
			double upper_bound = rules[i].getServerCPUUpperBound();
			double lower_bound = rules[i].getServerCPULowerBound();

			macedbg(1) << "CPU upper_bound=" << upper_bound <<", lower_bound=" << lower_bound << ", relatedContextTypes: "<< selected_ctx_types <<Log::endl;

			// balance workload when some servers are overloaded
			for( uint32_t j=0; j<lemReports.size(); j++ ) {
				mace::vector<mace::ContextRuntimeInfoForElasticity> mig_ctx_cands1 = lemReports[j].findMigrationContextForCPU( upper_bound, selected_ctx_types );
				// macedbg(1) << "contexts: " << mig_ctx_cands1 << Log::endl;
				for( uint32_t k=0; k<mig_ctx_cands1.size(); k++ ) {
					bool insert_flag = false;
					for( mace::vector<mace::ContextRuntimeInfoForElasticity>::iterator iter = mig_ctx_cands.begin(); iter!=mig_ctx_cands.end(); iter ++ ) {
						if( mig_ctx_cands1[k].contextExecTime >= (*iter).contextExecTime ) {
							mig_ctx_cands.insert(iter, mig_ctx_cands1[k] );
							insert_flag = true;
							break;
						}
					}

					if( !insert_flag ){
						mig_ctx_cands.push_back( mig_ctx_cands1[k] );
					}
				}
			}

			macedbg(1) << "Candidate migration contexts for scale out: " << mig_ctx_cands << Log::endl;
			
			for( uint32_t j=0; j<lemReports.size(); j++ ) {
				mace::vector<mace::ElasticityBehaviorAction> mactions = lemReports[j].balanceServerCPUUsage(upper_bound, lower_bound, mig_ctx_cands, rules[i].priority);
				for( uint32_t k=0; k<mactions.size(); k++ ) {
					mig_ctx_addrs[ mactions[k].fromNode ].push_back( mactions[k] );
					macedbg(1) << "Generating " << mactions[k] << " for " << mactions[k].fromNode << Log::endl; 
					chosen_contexts.insert(mactions[k].contextName);
				}
			}	


			// scale in
			for( uint32_t j=0; j<lemReports.size(); j++ ) {
				mig_ctx_cands.clear();
				macedbg(1) << "LEM["<< lemReports[j].fromAddress <<"] cpu_usage: " << lemReports[j].curCPUUsage << Log::endl;
				if( lemReports[j].curCPUUsage >= lower_bound ){
					continue;
				}

				mig_ctx_cands = lemReports[j].getContextRuntimeInfos(selected_ctx_types);
				macedbg(1) << "Candidate contexts for scale in: " << mig_ctx_cands << Log::endl;
				for( uint32_t m=0; m<lemReports.size(); m++ ) {
					if(mig_ctx_cands.size() == 0){
						break;
					}

					if( j == m || lemReports[m].curCPUUsage < lemReports[j].curCPUUsage ){
						continue;
					}
				
					mace::vector<mace::ElasticityBehaviorAction> mactions = lemReports[m].balanceServerCPUUsage(upper_bound, lower_bound, mig_ctx_cands, rules[i].priority);
					for( uint32_t k=0; k<mactions.size(); k++ ) {
						if( chosen_contexts.count(mactions[k].contextName) > 0 ){
							mace::vector<mace::ElasticityBehaviorAction>& mactions1 = mig_ctx_addrs[ mactions[k].fromNode ];
							bool updated_addr = false;
							for( uint32_t n=0; n<mactions1.size(); n++ ) {
								if( mactions1[n].contextName == mactions[k].contextName ){
									mactions1[n] = mactions[k];
									macedbg(1) << "Updating " << mactions[k] << " for " << mactions[k].fromNode << Log::endl;
									updated_addr = true;
									break;
								}
							}
							ASSERT(updated_addr);
						} else {
							mig_ctx_addrs[ mactions[k].fromNode ].push_back( mactions[k] );
							macedbg(1) << "Generating " << mactions[k] << " for " << mactions[k].fromNode << Log::endl; 
						}

					}
				}
			}

		} 
		// else if( rules[i].behavior.behaviorType == ElasticityBehavior::ISOLATE && rules[i].behavior.resourceType == mace::ElasticityBehavior::RES_CPU 
		// 		&& rules[i].ruleType == mace::ElasticityRule::RESOURCE_RULE ) {

		// 	double upper_bound = rules[i].getServerCPUUpperBound();
		// 	const mace::set<mace::string>& selected_ctx_types = rules[i].relatedContextTypes;

		// 	for( uint32_t j=0; j<lemReports.size(); j++ ) {
		// 		mace::vector<mace::ContextRuntimeInfoForElasticity> mig_ctx_cands1 = lemReports[j].findMigrationContextForCPU( upper_bound, selected_ctx_types );
		// 		// macedbg(1) << "contexts: " << mig_ctx_cands1 << Log::endl;
		// 		for( uint32_t k=0; k<mig_ctx_cands1.size(); k++ ) {
		// 			bool insert_flag = false;
		// 			for( mace::vector<mace::ContextRuntimeInfoForElasticity>::iterator iter = mig_ctx_cands.begin(); iter!=mig_ctx_cands.end(); iter ++ ) {
		// 				if( mig_ctx_cands1[k].contextExecTime >= (*iter).contextExecTime ) {
		// 					mig_ctx_cands.insert(iter, mig_ctx_cands1[k] );
		// 					insert_flag = true;
		// 					break;
		// 				}
		// 			}

		// 			if( !insert_flag ){
		// 				mig_ctx_cands.push_back( mig_ctx_cands1[k] );
		// 			}
		// 		}
		// 	}

		// 	macedbg(1) << "Candidate migration contexts: " << mig_ctx_cands << Log::endl;
			
		// 	for( uint32_t j=0; j<lemReports.size(); j++ ) {
		// 		mace::vector<mace::ElasticityBehaviorAction> mactions = lemReports[j].balanceServerCPUUsage(upper_bound, 0, mig_ctx_cands, rules[j].priority);
		// 		for( uint32_t k=0; k<mactions.size(); k++ ) {
		// 			mig_ctx_addrs[ mactions[k].fromNode ].push_back( mactions[k] );
		// 			macedbg(1) << "Generating " << mactions[k] << " for " << mactions[k].fromNode << Log::endl; 
		// 		}
		// 	}

		// } 
		else if( rules[i].behavior.behaviorType == ElasticityBehavior::ISOLATE && rules[i].behavior.resourceType == mace::ElasticityBehavior::RES_CPU ) {
			mace::map<mace::string, mace::MaceAddr > ctx_addrs;
			mace::map<mace::MaceAddr, mace::ServerRuntimeInfo> server_infos;

			for( uint32_t m=0; m<lemReports.size(); m++ ) {
				const mace::vector<mace::ContextRuntimeInfoForElasticity>& mig_ctx_cands1 = lemReports[m].contextRuntimeInfos;
				// macedbg(1) << "contexts: " << mig_ctx_cands1 << Log::endl;
				for( uint32_t j=0; j<mig_ctx_cands1.size(); j++ ) {
					ctx_addrs[ mig_ctx_cands1[j].contextName ] = lemReports[m].fromAddress;

					bool insert_flag = false;
					for( mace::vector<mace::ContextRuntimeInfoForElasticity>::iterator iter = mig_ctx_cands.begin(); iter!=mig_ctx_cands.end(); iter ++ ) {
						if( mig_ctx_cands1[j].contextExecTime >= (*iter).contextExecTime ) {
							mig_ctx_cands.insert(iter, mig_ctx_cands1[j] );
							insert_flag = true;
							break;
						}
					}

					if( !insert_flag ){
						mig_ctx_cands.push_back( mig_ctx_cands1[j] );
					}
				}

				server_infos[lemReports[m].fromAddress] = lemReports[m].serverRuntimeInfo;
			}

			macedbg(1) << "Candidate migration contexts: " << mig_ctx_cands << Log::endl;

			while( mig_ctx_cands.size() > 0 ) {
				const ContextRuntimeInfoForElasticity& ctx_rt_info = mig_ctx_cands[0];

				mace::MaceAddr addr = SockUtil::NULL_MACEADDR;
				double cpu_usage = 1;
				for( mace::map<mace::MaceAddr, ServerRuntimeInfo>::iterator iter = server_infos.begin(); iter != server_infos.end(); iter ++ ) {
					if( (iter->second).CPUUsage < cpu_usage && iter->first != ctx_addrs[ ctx_rt_info.contextName ] ) {
						cpu_usage = (iter->second).CPUUsage;
						addr = iter->first;
					}
				}

				if( addr != SockUtil::NULL_MACEADDR ) {
					uint8_t spec_req = mace::ElasticityBehaviorAction::UNDEF;
					if( rules[i].behavior.specialRequirement == mace::ElasticityBehavior::REQ_COL_CHILDREN ) {
						spec_req = mace::ElasticityBehaviorAction::REQ_COL_CHILDREN;
					}

					ElasticityBehaviorAction maction( mace::ElasticityBehaviorAction::EBACTION_MIGRATION, mace::ElasticityBehaviorAction::UNDEF, rules[i].priority, spec_req, 
						ctx_addrs[ ctx_rt_info.contextName ], addr,  ctx_rt_info.contextName, ctx_rt_info.contextExecTime  );
					mig_ctx_addrs[ ctx_addrs[ctx_rt_info.contextName]  ].push_back( maction );

					server_infos[addr].CPUUsage = ( server_infos[addr].CPUUsage * server_infos[addr].totalCPUTime + ctx_rt_info.contextExecTime  ) / server_infos[addr].totalCPUTime;
				}

				mig_ctx_cands.erase( mig_ctx_cands.begin() );
			
			}

		}
	}

	mace::set<MaceAddr> replied_addrs;
	ContextService* _service = static_cast<ContextService*>(sv);
	for( mace::map< MaceAddr, mace::vector<ElasticityBehaviorAction> >::iterator iter = mig_ctx_addrs.begin(); 
			iter != mig_ctx_addrs.end(); iter ++ ) {
		_service->replyLEMReport( iter->first, iter->second );
		replied_addrs.insert( iter->first );
	}	

	mace::vector<ElasticityBehaviorAction> empty_mactions;
	for( uint32_t i = 0; i<lemReports.size(); i++ ) {
		if( replied_addrs.count( lemReports[i].fromAddress ) == 0 ) {
			_service->replyLEMReport( lemReports[i].fromAddress, empty_mactions );
		}
	} 

	sl.lock();
	toAcceptLEMReportFlag = true;
	latestLEMReports = lemReports;
	lemReports.clear();
}

bool mace::eMonitor::hasEnoughResource( const ElasticityBehaviorAction& maction ) const {
	double new_cpu_usage = (maction.contextExecTime + predictCPUTime ) / predictCPUTotalTime;
	if( new_cpu_usage >= cpu_upper_bound ) {
		return false;
	} else {
		return true;
	}
}

void mace::eMonitor::toAcceptContext( const ElasticityBehaviorAction& maction ) {
	predictCPUTime += maction.contextExecTime;
}
