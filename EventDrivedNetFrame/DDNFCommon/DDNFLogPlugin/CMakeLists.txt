set(ProjectName "DDNFLogPlugin")
file(GLOB DDNFLogPlugin_ROOT_Cpp *.cpp)
file(GLOB DDNFLogPlugin_ROOT_Hpp *.h)

add_library(LogPlugin STATIC
	${DDNFLogPlugin_ROOT_Cpp}
	${DDNFLogPlugin_ROOT_Hpp})

if(UNIX)

	if (CMAKE_BUILD_TYPE MATCHES "Release")
	else()
	endif()
	
	add_dependencies(LogPlugin Core )

	target_link_libraries(LogPlugin -static-libstdc++)
else()
	add_dependencies(LogPlugin Core )
	
	target_link_libraries(LogPlugin -static-libstdc++)
endif()

set_target_properties( LogPlugin PROPERTIES OUTPUT_NAME_DEBUG "LogPlugin_d")
set_target_properties( LogPlugin PROPERTIES PREFIX "")
set_target_properties( LogPlugin PROPERTIES
	FOLDER "SDK"
	ARCHIVE_OUTPUT_DIRECTORY ${DDNFOutPutDir}
	RUNTIME_OUTPUT_DIRECTORY ${DDNFOutPutDir}
	LIBRARY_OUTPUT_DIRECTORY ${DDNFOutPutDir} )