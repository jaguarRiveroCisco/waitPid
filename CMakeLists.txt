cmake_minimum_required(VERSION 3.10)

project(DynamicControlOfChildProcesses)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(
    include/controller
    include/process
    include/synchro
    include/tools
    include/logger
    include/networkprocess
)

# Find libcurl
find_package(CURL REQUIRED)

# Add executable
add_executable(ProcessController 
    # Main source file
    src/ProcessController.cpp 

    # Process sources
    src/process/messenger.cpp 
    src/process/base_handler.cpp
    src/process/communicator.cpp
    src/process/simul_process.cpp
    src/process/process.cpp
    src/process/base_process.cpp

    src/networkprocess/network_process.cpp

    # Controller sources
    src/controller/process_handler.cpp
    src/controller/console_control.cpp
    src/controller/controller_base.cpp

    # Tools
    src/tools/timestamp.cpp
    src/tools/semaphore_guard.cpp

    # Logger
    src/logger/logger.cpp
    src/logger/console_logger.cpp
    src/logger/file_logger.cpp
    src/logger/logger_instance.cpp
)

# Link libcurl
target_link_libraries(ProcessController PRIVATE CURL::libcurl)