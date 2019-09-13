# 将源代码添加到此项目的可执行文件。

file(GLOB NetPluginH *.h)
file(GLOB NetPluginCC *.cc)

ADD_LIBRARY (NetPlugin SHARED ${NetPluginCC} ${NetPluginH})
target_link_libraries(NetPlugin )