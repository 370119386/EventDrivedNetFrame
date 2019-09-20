file(GLOB DDNFCore_Root_Hpp *.hpp)

add_library(Core STATIC ${DDNFCore_Root_Hpp})

add_definitions(-DDDNF_NONCLIENT_BUILD)

set_target_properties( Core PROPERTIES OUTPUT_NAME_DEBUG "Core_d")
set_target_properties( Core PROPERTIES PREFIX "")
set_target_properties( DDNFCore PROPERTIES
       FOLDER "DDNFSDK"
       ARCHIVE_OUTPUT_DIRECTORY ${DDNFOutPutDir}
       RUNTIME_OUTPUT_DIRECTORY ${DDNFOutPutDir}
       LIBRARY_OUTPUT_DIRECTORY ${DDNFOutPutDir} )

if(UNIX)
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        target_link_libraries(Core )
    else()
        target_link_libraries(Core )
    endif()

    target_link_libraries(Core -static-libstdc++)
else()
    target_link_libraries(Core )
    target_link_libraries(Core -static-libstdc++)
endif()