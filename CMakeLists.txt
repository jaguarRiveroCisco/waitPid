cmake_minimum_required(VERSION 3.10)

project(DynamicControlOfChildProcesses)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(ProcessController 
src/waitPidProc.cpp 
src/messenger.cpp 
src/process_base.cpp
src/process_helper.cpp
src/simul_process.cpp
src/process_handler.cpp
src/process.cpp
src/process_control.cpp
src/controller_base.cpp
)