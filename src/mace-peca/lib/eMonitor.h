#ifndef __EMONITOR_h
#define __EMONITOR_h

#include "mace.h"
#include "Message.h"
#include "pthread.h"
#include "ThreadPool.h"
/**
 * \file ContextDispatch.h
 * \brief declares the ContextEventTP class
 */

namespace mace{

class ElasticityBehaviorAction: public Serializable, public PrintPrintable {
public:
  static const uint8_t UNDEF = 0;

public:
  static const uint8_t EBACTION_MIGRATION = 1;
  static const uint8_t EBACTION_PIN = 2;
  static const uint8_t EBACTION_COLOCATE = 3;

public:
  static const uint8_t RULE_METHOD_COUNT = 1;
  static const uint8_t RULE_CPU_BALANCE = 2;
  static const uint8_t RULE_REFERENCE = 3;

public:
  static const uint8_t REQ_PIN_ONE_ACTOR = 1;
  static const uint8_t REQ_COL_CHILDREN = 2;

public:
  uint8_t actionType;
  uint8_t ruleType;
  uint8_t priority;
  uint8_t specialRequirement;
  mace::MaceAddr fromNode;
  mace::MaceAddr toNode;
  mace::string contextName;

  uint64_t contextExecTime;
  mace::map<mace::string, uint64_t> contextsInterCount;
  
public:
  ElasticityBehaviorAction(): actionType(UNDEF), ruleType(UNDEF), priority(0), specialRequirement(UNDEF) { }
  ElasticityBehaviorAction( const uint8_t& action_type, const uint8_t& rule_type, const uint8_t& prio, const uint8_t& spec_req, const mace::MaceAddr& from_node, 
    const mace::MaceAddr& to_node, const mace::string& ctx_name, const uint64_t& ctx_exec_time ): 
    actionType(action_type), ruleType(rule_type), priority(prio), specialRequirement(spec_req), fromNode(from_node), toNode(to_node), contextName(ctx_name),
    contextExecTime(ctx_exec_time) { }
  ~ElasticityBehaviorAction() { }

  uint64_t getNodeMethodCount( const mace::MaceAddr& addr, const mace::set<mace::string>& predict_local_context, const mace::ContextMapping& snapshot) const;

public:
  virtual void serialize(std::string& str) const;
  virtual int deserialize(std::istream & is) throw (mace::SerializationException);

  void print(std::ostream& out) const;

  void printNode(PrintNode& pr, const std::string& name) const { }

};

class eMonitor;
struct ElasticityThreadArg {
  eMonitor* elasticityMonitor;
};

class CPUInformation: public Serializable, public PrintPrintable {
public:
  uint64_t totalUserCPUTime;
  uint64_t totalUserLowCPUTime;
  uint64_t totalSysCPUTime;
  uint64_t totalIdleCPUTime;
  
public:
  CPUInformation(): totalUserCPUTime(0), totalUserLowCPUTime(0), totalSysCPUTime(0), totalIdleCPUTime(0) { }
  ~CPUInformation() { }

  virtual void serialize(std::string& str) const;
  virtual int deserialize(std::istream & is) throw (mace::SerializationException);
  void print(std::ostream& out) const { }
  void printNode(PrintNode& pr, const std::string& name) const { }  

  CPUInformation& operator=(const CPUInformation& orig){
    ASSERTMSG( this != &orig, "Self assignment is forbidden!" );
    this->totalUserCPUTime = orig.totalUserCPUTime;
    this->totalUserLowCPUTime = orig.totalUserLowCPUTime;
    this->totalSysCPUTime = orig.totalSysCPUTime;
    this->totalIdleCPUTime = orig.totalIdleCPUTime;
    return *this;
  }
};

class MigrationRequest {
public:
  mace::string contextName;
  mace::MaceAddr destNode;

public:
  MigrationRequest() { }
  ~MigrationRequest() { }

  void runtest() { }

};

class ContextRuntimeInfo;
class ElasticityConfiguration;

class ServerRuntimeInfo: public Serializable, public PrintPrintable {
public:
  double CPUUsage;
  double totalCPUTime;
  uint32_t contextsNumber;
  
public:
  ServerRuntimeInfo(): CPUUsage(0.0), totalCPUTime(0.0), contextsNumber(0) { }
  ServerRuntimeInfo( const double& cpu_usage, const double& total_cpu_time, const uint32_t& n_ctx ): CPUUsage(cpu_usage), 
    totalCPUTime( total_cpu_time ), contextsNumber(n_ctx) { }
  ~ServerRuntimeInfo() { }

  virtual void serialize(std::string& str) const;
  virtual int deserialize(std::istream & is) throw (mace::SerializationException);

  void print(std::ostream& out) const;

  void printNode(PrintNode& pr, const std::string& name) const { } 

public:
  double adjustCPUUsageWhenToAddContext( const double& exec_time ) const { 
    return 100*(exec_time + totalCPUTime*CPUUsage*0.01) / totalCPUTime; 
  }
};

class ContextMigrationQuery {
public:
  mace::MaceAddr srcAddr;
  mace::vector<mace::ElasticityBehaviorAction> queryMactions;
  mace::ServerRuntimeInfo serverInfo;

public:
  ContextMigrationQuery() { }
  ContextMigrationQuery( const mace::MaceAddr& src_Addr, const mace::vector<mace::ElasticityBehaviorAction>& query_mactions,
    const ServerRuntimeInfo& server_info ): srcAddr( src_Addr ), queryMactions( query_mactions ), serverInfo( server_info ) { }
  ~ContextMigrationQuery() { queryMactions.clear(); }

};

class ContextRuntimeInfoForElasticity: public Serializable, public PrintPrintable {
public:
  mace::string contextName;
  mace::MaceAddr currAddr;

  mace::map< mace::string, uint64_t > fromAccessCount;
  mace::map< mace::string, uint64_t > fromMessageSize;

  mace::map< mace::string, uint64_t > toAccessCount;

  double currLatency;
  double contextExecTime;
  double avgLatency;
  uint64_t count;
  uint64_t externalMessageCount;

public:
  ContextRuntimeInfoForElasticity(): currLatency(0.0), contextExecTime(0.0), avgLatency(0.0), count(0), externalMessageCount(0)  { }
  ~ContextRuntimeInfoForElasticity() { }

public:
  virtual void serialize(std::string& str) const;
  virtual int deserialize(std::istream & is) throw (mace::SerializationException);
  void print(std::ostream& out) const;
  void printNode(PrintNode& pr, const std::string& name) const { }

  mace::map< mace::string, uint64_t > getFromAccessCounts() { return fromAccessCount; }

  mace::map< mace::MaceAddr, uint64_t > computeExchangeBenefit( const mace::ContextMapping& snapshot );
  uint64_t getTotalFromAccessCount() const;

  bool isStrongConnected( const mace::string& ctx_name ) const;
  uint64_t getTotalClientRequestNumber() const;
  mace::set<mace::string> getStrongConnectContexts() const;

  mace::map<mace::string, uint64_t> getContextMethodCounts( const double& threshold, const mace::set<mace::string>& contextTypes ) const;
  uint64_t getMethodCount( const mace::string& ctx_name ) const;

  uint64_t getNodeInterCount( const mace::ContextMapping& snapshot, const MaceAddr& nodeAddr, 
    const mace::map<mace::string, mace::MaceAddr>& adjust_contexts ) const;
};

class LEMReportInfo {
public:
  ServerRuntimeInfo serverRuntimeInfo;
  mace::vector<mace::ContextRuntimeInfoForElasticity> contextRuntimeInfos;
  mace::MaceAddr fromAddress;

  double curCPUUsage;
  double curCPUTime;

  mace::vector<mace::ContextRuntimeInfoForElasticity> toAcceptContexts;

public:
  LEMReportInfo( const ServerRuntimeInfo& server_info, const mace::vector<mace::ContextRuntimeInfoForElasticity>& ctx_rt_infos,
    const mace::MaceAddr& src ): serverRuntimeInfo( server_info ), contextRuntimeInfos(ctx_rt_infos), fromAddress( src ), curCPUUsage( serverRuntimeInfo.CPUUsage), 
    curCPUTime( serverRuntimeInfo.totalCPUTime ) { }
  ~LEMReportInfo() { }

public:
  mace::vector<mace::ContextRuntimeInfoForElasticity> findMigrationContextForCPU( const double& upper_bound, const mace::set<mace::string>& selected_ctx_types );
  mace::vector<mace::ElasticityBehaviorAction> balanceServerCPUUsage( const double& upper_bound, const double& lower_bound, 
    mace::vector<mace::ContextRuntimeInfoForElasticity>& mig_ctx_cands, const uint8_t& rule_priority );

  double getCPUUsage() const { return serverRuntimeInfo.CPUUsage; }
  mace::vector<mace::ContextRuntimeInfoForElasticity> getContextRuntimeInfos(const mace::set<mace::string>& ctx_types) const;

};

class ElasticityRuleInfo: public PrintPrintable {
public:
  static const uint8_t INVALID = 0;
  static const uint8_t BALANCE_SERVER_CPU_USAGE = 1;
  static const uint8_t REFERENCE_COLOCATE = 2;

public:
  uint8_t ruleType;
  uint64_t priority;
  mace::set<mace::string> contextTypes;

public:
  ElasticityRuleInfo(): ruleType( INVALID ), priority(0) { }
  ~ElasticityRuleInfo() { }

public:
  void print(std::ostream& out) const;
  void printNode(PrintNode& pr, const std::string& name) const { }

};

class ElasticityRule;
class eMonitor {
public:
  static const uint64_t EVENT_COACCESS_THRESHOLD = 10;
  static const uint64_t MIGRATION_CPU_THRESHOLD = 100;
  static const uint64_t CONTEXT_STRONG_CONNECTED_PER_THREAHOLD = 0.6;
  static const uint64_t CONTEXT_STRONG_CONNECTED_NUM_THREAHOLD = 6000;
  static const uint64_t INTER_CONTEXTS_STRONG_CONNECTED_THREAHOLD = 1;
  static const double CPU_BUSY_THRESHOLD = 70;
  static const double CPU_IDLE_THRESHOLD = 30;

  static const uint32_t MAJOR_HANDLER_THREAD_ID = 1;

  static const double DECREASE_MIGRATION_THRESHOLD_PERCENT = 0.7;
  static const double INCREASE_MIGRATION_THRESHOLD_PERCENT = 0.5;
  static const double MIGRATION_THRESHOLD_STEP = 1.0;

  static const double CLOSE_LATENCY_PERCENT = 0.7;

  static const double CONEXT_NUMBER_BALANCE_THRESHOLD = 0.1;
  static const int MIN_CONEXT_NUMBER_BALANCE = 2;

private:
  AsyncEventReceiver* sv; 
  bool isGEM; 
  mace::vector<mace::MaceAddr> GEMs; 
    
  pthread_t lemThreadKey;
  pthread_t gemThreadKey;
  pthread_t serverThreadKey;

  uint32_t periodTime; 
  uint32_t gemWaitingTime;
  uint32_t serverMonitorIntervalTime;
  
  mace::map< mace::string, mace::ContextRuntimeInfo> contextRuntimeInfos;

  CPUInformation lastcpuInfo;
  double currentcpuUsage; //bs

  // LEM Elasticity
  std::map< mace::string, ContextRuntimeInfoForElasticity > contextsRuntimeInfo; 
  std::map<mace::string, mace::ElasticityBehaviorAction*> mactions;

  mace::set<mace::string> manageContextTypes;
  ServerRuntimeInfo serverRuntimeInfo;

  mace::MaceAddr lastGEMAddr;
  pthread_cond_t lemCond;

  // GEM Elasticity
  std::vector<LEMReportInfo> lemReports;
  std::vector<LEMReportInfo> latestLEMReports;
  bool toAcceptLEMReportFlag;

  
  // migration
  pthread_mutex_t migrationMutex;
  std::map< mace::string, MigrationRequest > contextMigrationRequests;
  std::vector< mace::string > migratingContextNames;
  mace::string migratingContextName;
  bool readyToProcessMigrationQuery;
  bool migrationQueryThreadIsIdle;
  std::vector< mace::ContextMigrationQuery > contextMigrationQueries; //bs
  bool toMigrateContext;
  double contextMigrationThreshold;
  

  double cpu_upper_bound;
  double cpu_lower_bound;

  double predictCPUTime;
  double predictCPUTotalTime;
  mace::set< mace::string > predictLocalContexts;

public:
  mace::ElasticityConfiguration* eConfig; 

public:
  static void* startLEMThread( void* arg ); 
  static void* startGEMThread( void* arg ); 
  static void* startServerMonitorThread( void* arg );
    
public:
  eMonitor( AsyncEventReceiver* sv, const uint32_t& period_time, const uint32_t& gem_waiting_time, const mace::vector<mace::MaceAddr>& gems, 
    const mace::set<mace::string>& manange_contexts, const uint32_t& server_monitor_interval_time );
  ~eMonitor() { }

  void addElasticityRule( const mace::ElasticityRule& rule );

  void setIsGEM( bool is_gem ) { isGEM = is_gem; }

  double computeCurrentCPUUsage() const; 
  uint64_t computeCPUTotalTime() const; 

  

  double computeCurrentMemUsage() const;
  double getCurrentCPUUsage() const { return currentcpuUsage; }

  void runInfoCollection(); 
  void updateCPUInfo();

  void processContextElasticity(); 

  // Elasticity control
  void processContextsMigrationQuery();
  void processContextsMigrationQueryReply( const mace::MaceAddr& dest, const mace::vector<mace::string>& accept_m_contexts );
  void enqueueContextMigrationQuery( const mace::MaceAddr& src, const mace::vector<mace::ElasticityBehaviorAction>& query_mactions, 
    const ServerRuntimeInfo& server_info );
  void enqueueLEMReport( const ServerRuntimeInfo& server_info, const mace::vector<mace::ContextRuntimeInfoForElasticity>& ctx_rt_infos,
    const mace::MaceAddr& src );
  void processLEMReportReply( const mace::vector<mace::ElasticityBehaviorAction>& gem_mactions );
  uint64_t getGEMWaitingTime() const { return gemWaitingTime; }
  void processLEMReports();

  // migration
  void requestContextMigration(const mace::string& contextName, const MaceAddr& destNode);
  void processContextMigration();
  void wrapupCurrentContextMigration();

  // context creation
  bool checkContextCreateRule( const mace::string& ctx_name ) const;
  const mace::MaceAddr& getGEMAddr() const { return lastGEMAddr; }
  mace::MaceAddr getNewContextAddr( const mace::string& ctx_name );
  mace::MaceAddr getNewContextAddrForActorRules( const mace::string& ctx_name );

  // server
  CPUInformation getCurrentCPUInfo() const; 
  static double computeCPUUsage( const CPUInformation& last_cpu_info, const CPUInformation& cur_cpu_info );
  uint32_t getServerMonitorIntervalTime() const { return serverMonitorIntervalTime; }

private:
  bool hasEnoughResource( const ElasticityBehaviorAction& maction ) const;
  void toAcceptContext( const ElasticityBehaviorAction& maction );
};

}
#endif

