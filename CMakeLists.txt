cmake_minimum_required(VERSION 3.27)
project(Project)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static-libgcc -static-libstdc++")  # 静态连接，避免报错

set(CMAKE_CXX_STANDARD 14)

#include_directories(.)



add_executable(
        Project
#        main.cpp
#        program1.cpp
#        program2.cpp
#        program3.cpp
#        test1.cpp
#        test2.cpp
        install.cpp
#        test3.cpp
#        lianxi1.cpp
)

#    # 隐藏控制台窗口
#    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
#            WIN32_EXECUTABLE ON
#            MACOSX_BUNDLE ON
#    )
