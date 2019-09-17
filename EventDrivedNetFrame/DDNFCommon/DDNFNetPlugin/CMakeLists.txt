# 将源代码添加到此项目的可执行文件。

file(GLOB NetPluginH *.h)
file(GLOB NetPluginCC *.cc)

ADD_LIBRARY (NetPlugin SHARED ${NetPluginCC} ${NetPluginH})

if(CMAKE_HOST_WIN32)
target_link_libraries(NetPlugin )
else()
target_link_libraries(NetPlugin libevent.a libevent_core.a)
endif()