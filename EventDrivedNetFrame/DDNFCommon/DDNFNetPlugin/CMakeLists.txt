set(ProjectName "DDNFNetPlugin")

file(GLOB DDNFNet_ROOT_Cpp *.cc)
file(GLOB DDNFNet_ROOT_Hpp *.h)

#Exclude this file
#file(GLOB RemoveItems_Cpp TestClient.cpp TestServer.cpp)
#list(REMOVE_ITEM NFNet_ROOT_Cpp ${RemoveItems_Cpp})

add_library(NetPlugin STATIC ${DDNFNet_ROOT_Cpp} ${DDNFNet_ROOT_Hpp})
#add_definitions(-DNF_NONCLIENT_BUILD)
set_target_properties( NetPlugin PROPERTIES OUTPUT_NAME_DEBUG "NetPlugin")
set_target_properties( NetPlugin PROPERTIES PREFIX "")

add_dependencies(NetPlugin Core)
	
if (APPLE)
	if (CMAKE_BUILD_TYPE MATCHES "Release")
	else()
	endif()

		target_link_libraries(NetPlugin NFCore)
		target_link_libraries(NetPlugin libevent.a)
		target_link_libraries(NetPlugin libevent_core.a)
		target_link_libraries(NetPlugin libevent_extra.a)
		#target_link_libraries(NFNetPlugin libevent_openssl.a)
		target_link_libraries(NetPlugin libprotobuf.a)
		#target_link_libraries(NFNetPlugin libssl.a)
		#target_link_libraries(NFNetPlugin libcrypto.a)
		#target_link_libraries(NFNetPlugin NFMessageDefine)

		target_link_libraries(NetPlugin -static-libstdc++)
	add_definitions(
		-D_THREAD_SAFE
	)
elseif(UNIX)
	if (CMAKE_BUILD_TYPE MATCHES "Release")
	else()
	endif()

		target_link_libraries(NetPlugin NFCore)
		target_link_libraries(NetPlugin libevent.a)
		target_link_libraries(NetPlugin libevent_core.a)
		target_link_libraries(NetPlugin libevent_extra.a)
		#target_link_libraries(NFNetPlugin libevent_openssl.a)
		target_link_libraries(NetPlugin libprotobuf.a)
		#target_link_libraries(NFNetPlugin libssl.a)
		#target_link_libraries(NFNetPlugin libcrypto.a)
		#target_link_libraries(NFNetPlugin NFMessageDefine)
		target_link_libraries(NetPlugin -static-libstdc++)
	add_definitions(
		-D_THREAD_SAFE
	)
else()
	target_link_libraries(NetPlugin  libprotobuf libevent_core)
	add_definitions("-D_LIB -DWIN32 -DWIN")
endif()