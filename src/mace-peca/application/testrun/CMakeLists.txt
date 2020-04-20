SET(APPS contextrun)
SET(SERVICES TcpTransport 
	#BTreeServer BTreeClient
	#BinaryTreeApp
	#HaloPresenceServer HaloPresenceClient
	#TPCWMinApp
	#PageRankApp
	#HaloPresenceClient HaloPresenceServer
	#TreeTPCCAppServer TreeTPCCAppClient
	#TagItemShareAppServer TagItemShareAppClient
	#DistributedDAGTestAppServer DistributedDAGTestAppClient
	#WTFAppClient WTFAppServer
	MetadataAppClient MetadataAppServer
	#TagAppServer TagAppClient
	#ChattingRoomAppServer
	#MediaServiceClient MediaServiceServer
	#PingPong
)

INCLUDE(${CMAKE_CURRENT_BINARY_DIR}/../application.cmake)
