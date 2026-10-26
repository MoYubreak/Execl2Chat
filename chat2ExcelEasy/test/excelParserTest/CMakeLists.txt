# 设置Cmake的最小版本
cmake_minimum_required(VERSION 3.10)
# 设置项目名称
project(excelParserTest)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置构建类型
set(CMAKE_BUILD_TYPE Debug)
# 添加可执行文件
add_executable(excelParserTest excelParserTest.cc
    ../../source/execlParser.cc)
# 设置可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../source)
# 查找OpenXLSX库
find_package(OpenXLSX REQUIRED)
# 查找bite_scaffold库
find_package(bite_scaffold REQUIRED)
# 链接库
target_link_libraries(excelParserTest PUBLIC
    OpenXLSX::OpenXLSX
    bite_scaffold::bite_scaffold
    gtest
    pthread
)
