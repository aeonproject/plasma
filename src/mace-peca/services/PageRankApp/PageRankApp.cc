#include <stdlib.h>
#include <fstream>
#include <sstream>

configurable std::string graph_file_name = "input-graph";
configurable std::string partition_file_name = "input-graph-part";
configurable uint32_t N_PARTITION = 4;
configurable uint32_t N_GRAPH_READER = 4;
configurable uint64_t N_LINE_OUTPUT = 10000;
configurable uint64_t N_PROCESS_ELASTICITY = 10;


actorclass GraphPartition {
	uint32_t partitionId;

    mace::map< uint64_t, double > vertex_values;
    mace::map< uint64_t, mace::set<uint64_t> > edges;
    mace::set< uint64_t > p_vertexes;
    mace::set< uint64_t > active_vertexes;
    
    mace::map< uint32_t, mace::set<uint64_t> > vertex_required_by_neighbor;
        
    uint64_t curr_round;
    
    mace::set< uint32_t > active_neighbors;
    mace::set< uint32_t > updated_neighbors;
    
    bool computation_ready_flag;

    void initGraphPartition( mace::map<uint64_t, double> const& vvals, mace::vector<uint64_t> const& sub_graph_from_v, mace::vector<uint64_t> const& sub_graph_to_v, 
      mace::map< uint64_t, uint32_t > const& vmap )
};

actorclass GraphReader {
	mace::set<uint32_t> localGraphPartitions;

	void readGraphFile();
};

actorclass Barrier {
	uint64_t curr_round;
	vector<GraphPartition> graphPartitions;
	vector<GraphReader> graphReaders;

    mace::set<uint32_t> ready_partitions;
    
    uint64_t start_timestamp;

    void initialize();

    void toSync( const uint32_t& pId, const uint64_t& round );
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Barrier::initialize() {
	maceout << "Initialize barrier!" << Log::endl;
  curr_round = 1;

  for( uint32_t pid=0; pid<N_PARTITION; pid++ ){
    GraphPartition graph_partition = createActor<GraphPartition>();
    graphPartitions.push_back( graph_partition ); 

    graph_partition.initialize();
  }

  for( uint32_t rid=0; rid<N_GRAPH_READER; rid++ ){
    GraphReader gReader = createActor<GraphReader>();
    graphReaders.push_back( gReader );

    gReader.initialize();
  }
}

void Barrier::toSync( const uint32_t& pId, const uint64_t& round ){
  if( round > 1 ){
    maceout << "Partition("<< pId <<") is ready to start round " << round << ", its round("<< round-1 <<") latency=" << mace::getmtime() - start_timestamp << Log::endl;
  }
  ASSERT( curr_round == round );
    
  if( ready_partitions.count(pId) == 0 ) {
    ready_partitions.insert(pId);
  }

  if( ready_partitions.size() == graphPartitions.size() ) {
    if( curr_round > 1 ) {
      uint64_t round_period = mace::getmtime() - start_timestamp;
      maceout << "Round " << curr_round - 1 << " period_time=" << round_period << Log::endl;
    }

    start_timestamp = mace::getmtime();
    for( uint32_t i=0; i<graphPartitions.size(); i++ ) {
      event graphPartitions[i].startNextRoundComputation( curr_round );
    }

    curr_round ++;
    ready_partitions.clear();
  } 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphReader::readGraphFile( ) {
  sleep(5);

  localGraphPartitions = getLocalActorIDs<GraphPartition>();

  uint32_t rId = this->getActorID();

  maceout << "GraphReader("<< rId <<") has local GraphPartition: " << localGraphPartitions << Log::endl;
  if( localGraphPartitions.size() == 0 ){
    return;
  }

  mace::map< uint64_t, uint32_t > vertex_map;
  mace::map< uint32_t, set<uint64_t> > p_vertexes;
    
  maceout << "GraphReader("<< rId <<") start read partition from " << partition_file_name << Log::endl;
    

  std::ifstream partition_file( partition_file_name.c_str(), std::ifstream::in );

  std::string line;
  uint64_t line_number = MIN_VERTEX_ID;
  maceout << "minimum vertex id is: " << line_number << Log::endl;
  while( std::getline(partition_file, line) ) {
    std::stringstream ss(line);
    int pid = 0;
    ss >> pid;

    pid = pid % N_PARTITION;
    vertex_map[line_number] = pid;
    if( localGraphPartitions.count(pid) > 0 && p_vertexes[pid].count(line_number) == 0 ) {
      p_vertexes[pid].insert(line_number);
    }

    line_number ++;
  }
  partition_file.close();

  maceout << "GraphReader("<< rId <<") start read graph from " << graph_file_name << Log::endl;
    
  std::ifstream graph_file( graph_file_name.c_str(), std::ifstream::in );

  mace::map<uint32_t, vector<uint64_t> > sub_graph_from_vs;
  mace::map<uint32_t, vector<uint64_t> > sub_graph_to_vs;

  line_number = 0;
  while( std::getline(graph_file, line) ) {
    line_number ++;
    if( line_number % N_LINE_OUTPUT == 0 ) {
      maceout << "GraphReader("<< rId <<") has read "<< line_number <<" lines!" << Log::endl;
    }
    std::vector<std::string> strs = split_string(line, "\t");

    if( strs.size() == 2 ){
      std::stringstream ss1(strs[0]);
      std::stringstream ss2(strs[1]);

      int from_v, to_v;
      ss1 >> from_v;
      ss2 >> to_v;

      if( localGraphPartitions.count(vertex_map[from_v]) == 0 && localGraphPartitions.count(vertex_map[to_v]) == 0 ){
        continue;
      }

      if( vertex_map[from_v] == vertex_map[to_v] ){
        sub_graph_from_vs[ vertex_map[from_v] ].push_back(from_v);
        sub_graph_to_vs[ vertex_map[to_v] ].push_back(to_v);
      } else {
        sub_graph_from_vs[ vertex_map[from_v] ].push_back(from_v);
        sub_graph_to_vs[ vertex_map[from_v] ].push_back(to_v);

        sub_graph_from_vs[ vertex_map[to_v] ].push_back(from_v);
        sub_graph_to_vs[ vertex_map[to_v] ].push_back(to_v);
      }
    }
  }
  graph_file.close();

  maceout << "GraphReader("<< rId <<") finish graph reading!" << Log::endl;
    
  for( mace::set<uint32_t>::iterator iter = localGraphPartitions.begin(); iter != localGraphPartitions.end(); iter ++ ){
    uint32_t pid = *iter;
    maceout << "GraphReader("<< rId <<") start to handle Partition " << pid << Log::endl;

    ASSERT( p_vertexes.find(pid) != p_vertexes.end() );
    mace::map<uint64_t, double> vvals;
    mace::set<uint64_t>& vset = p_vertexes[pid];
      
      
    for( mace::set<uint64_t>::iterator iter=vset.begin(); iter!=vset.end(); iter++ ){
      vvals[ *iter ] = 1.0;
    }

    GraphPartition graph_partition = getActor<GraphPartition>( pid );
    event graph_partition.initGraphPartition( vvals, sub_graph_from_vs[pid], sub_graph_to_vs[pid], vertex_map );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  GraphPartition::initGraphPartition(mace::map<uint64_t, double> const& vvals, mace::vector<uint64_t> const& sub_graph_from_v, mace::vector<uint64_t> const& sub_graph_to_v, 
    mace::map< uint64_t, uint32_t > const& vmap ) {
    
  maceout<< "To initialize GraphPartition " << pId << Log::endl;
  partitionId = pId;
    
  curr_round = 1;
    
  vertex_values = vvals;
    
  for( mace::map< uint64_t, double >::iterator iter=vertex_values.begin(); iter!=vertex_values.end(); iter++ ) {
    active_vertexes.insert( iter->first );
    p_vertexes.insert( iter->first );
  }

  for( uint64_t i=0; i<sub_graph_from_v.size(); i++ ) {
    uint64_t from_v = sub_graph_from_v[i];
    uint64_t to_v = sub_graph_to_v[i];

    mace::map< uint64_t, uint32_t >::const_iterator from_iter = vmap.find(from_v);
    mace::map< uint64_t, uint32_t >::const_iterator to_iter = vmap.find(to_v);

    uint32_t from_p_id = from_iter->second;
    uint32_t to_p_id = to_iter->second;
    if( from_p_id == partitionId ) {
      if( edges[from_v].count(to_v) == 0 ) {
        edges[from_v].insert(to_v);
      }

      if( to_p_id != partitionId && active_neighbors.count( to_p_id ) == 0 ) {
        active_neighbors.insert( to_p_id );
      }
    } else if( to_p_id == partitionId ) {
      if( vertex_required_by_neighbor[ from_p_id ].count(to_v) == 0 ){
        vertex_required_by_neighbor[ from_p_id ].insert(to_v);
      }
    }
  }
  maceout<< "Done initialize GraphPartition("<< pId <<"). edge_size=" << edges.size() << " vertex_size="<< vertex_values.size() << Log::endl;

  Barrier barrier = getActor<Barrier>(1);
  event barrier.toSync(this->getActorID(), curr_round, true);
}


void GraphPartition::sendVertexVals( const uint32_t& src_pid, const uint64_t& round, const mace::map<uint64_t, double>& updated_vertex_values ) {
  ASSERT( curr_round == round );

  for( mace::map<uint64_t, double>::const_iterator iter=updated_vertex_values.begin(); iter!=updated_vertex_values.end(); iter++ ) {
    vertex_values[ iter->first ] = iter->second;
  }
    
  if( updated_neighbors.count( src_pid ) == 0 ) {
    updated_neighbors.insert( src_pid );
  }
  
  if( updated_neighbors.size() == active_neighbors.size() && active_vertexes.size() != 0 && computation_ready_flag ) {
    event this->startComputation();      
  }
}

void GraphPartition::startComputation() {
  computePageRank( vertex_values, edges, active_vertexes );
  maceout << "Finish computation on Partition("<< pId <<") for round " << curr_round <<", latency=" << getMarkerAvgLatency(PAGERANK_MARKER) << Log::endl;

  curr_round ++;

  updated_neighbors.clear();
    
  Barrier barrier = getActor<Barrier>(1);
  barrier.toSync( this->getActorID(), curr_round);
  computation_ready_flag = false;
}

void GraphPartition::startNextRoundComputation( const uint64_t& round ) {
  maceout << "GraphPartition("<< pId <<") start computation for round("<< round <<")" << Log::endl;


  if( curr_round != round ) {
    maceout << "GraphPartition("<< pId <<") round=" << curr_round << ", global round=" << round << Log::endl;
    ASSERT(false);
  }

  mace::vector<uint32_t> no_comm_neighbors;
  for( mace::map< uint32_t, mace::set<uint64_t> >::iterator iter=vertex_required_by_neighbor.begin(); iter!=vertex_required_by_neighbor.end(); iter++ ) {
    if( active_partitions.count(iter->first) == 0 ) {
      continue;
    }

    mace::map< uint64_t, double > required_vertex_values;

    mace::set<uint64_t>& vset = iter->second;
    for( mace::set<uint64_t>::iterator sIter=vset.begin(); sIter!=vset.end(); sIter++ ) {
      if( active_vertexes.count(*sIter) == 0 ) {
        continue;
      }
      required_vertex_values[ *sIter ] = vertex_values[ *sIter ];
    }
    
    GraphPartition graphPartition = getActor<GraphPartition>(iter->first);
    graphPartition.sendVertexVals( this->getActorID(), curr_round, required_vertex_values );
    if( required_vertex_values.size() == 0 ) {
      no_comm_neighbors.push_back(iter->first);
    }
  }

  for( uint32_t i=0; i<no_comm_neighbors.size(); i++ ) {
    vertex_required_by_neighbor.erase( no_comm_neighbors[i] );
    maceout << "Partition("<< pId <<") have no active vertexes for Partition("<< no_comm_neighbors[i] <<")!" << Log::endl;
  }
    
  computation_ready_flag = true;

  if( updated_neighbors.size() == active_neighbors.size() && active_vertexes.size() != 0 ) {
    this->startComputation();      
  }
}


void computePageRank( mace::map<uint64_t, double>& vertex_values, mace::map<uint64_t, mace::set<uint64_t> >& edges, mace::set<uint64_t>& active_vertexes ) {
  mace::map<uint64_t, double> new_vertex_values;
  for( mace::set<uint64_t>::iterator iter1=active_vertexes.begin(); iter1!=active_vertexes.end(); iter1++ ) {
    double new_val = 0;
    const mace::set<uint64_t>& nvs = edges[*iter1];
    for( mace::set<uint64_t>::const_iterator iter2=nvs.begin(); iter2!=nvs.end(); iter2++ ) {
      ASSERT( vertex_values.find(*iter2) != vertex_values.end() );
      new_val += vertex_values[*iter2];
    }
    new_vertex_values[*iter1] = new_val;
  }

  mace::set<uint64_t> new_inactive_vertexes;
  for( mace::map<uint64_t, double>::iterator iter=new_vertex_values.begin(); iter!=new_vertex_values.end(); iter++ ) {
    vertex_values[iter->first] = iter->second;
  }

  for( mace::set<uint64_t>::iterator iter=new_inactive_vertexes.begin(); iter!=new_inactive_vertexes.end(); iter++ ) {
    active_vertexes.erase(*iter);
  }

  return;
}

std::vector<std::string> split_string(const std::string& str, const std::string& tok){
  std::vector<std::string> strs;

  std::string to_split_string = str;
  int len = tok.size();
  while(true) {
    size_t pos = to_split_string.find(tok);
    if( pos == std::string::npos ){
      break;
    }

    std::string str1 = to_split_string.substr(0, pos);
    to_split_string = to_split_string.substr(pos+len);

    strs.push_back(str1);
  }

  strs.push_back(to_split_string);
  return strs;
}