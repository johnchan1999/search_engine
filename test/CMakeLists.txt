cmake_minimum_required(VERSION 3.0)
project(TEST2)

set(EXEC_PATH ${PROJECT_SOURCE_DIR}/../bin)

# 添加头文件搜索路径
include_directories(
        ../src/threadPool
        ../src/webonline
        ../src/wordonline
        ../src/LRUcache
        ../src/ConnectionPool
)

# 添加源文件
file(GLOB SOURCES
        Service.cc
        ../src/threadPool/*.cc
        ../src/webonline/*.cc
        ../src/wordonline/*.cc
        ../src/LRUcache/*.cc
        ../src/ConnectionPool.cc
)
# aux_source_directory(${PROJECT_SOURCE_DIR}/../src/ SRC)
#aux_source_directory(${PROJECT_SOURCE_DIR}/ TEST)

# 添加链接库搜索路径
# link_directories(${LIB_PATH})
# link_libraries(${SRC_LIB})

# 添加可执行文件
set(EXECUTABLE_OUTPUT_PATH ${EXEC_PATH})
add_executable(Service.out ${SOURCES})

# 链接库
target_link_libraries(Service.out
        PUBLIC
        -lhiredis
        -llog4cpp
)
