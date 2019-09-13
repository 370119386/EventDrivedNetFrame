file(GLOB DDNFCore_Root_Hpp *.hpp)

add_library(DDNFCore STATIC ${DDNFCore_Root_Hpp})

add_definitions(-DDDNF_NONCLIENT_BUILD)

set_target_properties( DDNFCore PROPERTIES OUTPUT_NAME_DEBUG "DDNFCore_d")
set_target_properties( DDNFCore PROPERTIES PREFIX "")
#set_target_properties( DDNFCore PROPERTIES
 #       FOLDER "DDNFSDK"
 #       ARCHIVE_OUTPUT_DIRECTORY ${DDNFOutPutDir}
 #       RUNTIME_OUTPUT_DIRECTORY ${DDNFOutPutDir}
  #      LIBRARY_OUTPUT_DIRECTORY ${DDNFOutPutDir} )

if(UNIX)
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        target_link_libraries(DDNFCore )
    else()
        target_link_libraries(DDNFCore )
    endif()

    target_link_libraries(DDNFCore -static-libstdc++)
else()
    target_link_libraries(DDNFCore )
    target_link_libraries(DDNFCore -static-libstdc++)
endif()