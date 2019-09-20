file(GLOB DDNFCore_Root_Head *.h)
file(GLOB DDNFCore_Root_Hpp *.hpp)
file(GLOB DDNFCore_Root_CC *.cc)

add_library(Core STATIC ${DDNFCore_Root_Head} ${DDNFCore_Root_Hpp} ${DDNFCore_Root_CC})

set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)

#add_definitions(-DDDNF_NONCLIENT_BUILD)

set_target_properties( Core PROPERTIES OUTPUT_NAME_DEBUG "Core")
set_target_properties( Core PROPERTIES PREFIX "")

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