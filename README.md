# Process Management Program

## Overview

The Process Management Program is a C++ application designed to handle and monitor multiple child processes. It utilizes threading, inter-process communication (IPC), and synchronization to efficiently manage process creation and lifecycle events.

This program manages and monitors a specified number of processes through multi-threading. It maintains a constant number of processes by respawning them as needed. The program can create both simulated processes, which sleep for random durations, and real processes that perform tasks. Each process is managed by a ProcessHandler to ensure its lifecycle is controlled.


Inter-process communication (IPC) is used for message exchange between processes, with synchronization achieved through mutexes and condition variables. A console-based controller allows users to interact with processes, enabling commands to view status, terminate processes by PID, or manage all processes.

## Key Components

- **Messenger**: Facilitates message passing between processes for communication.
- **Synchro**: Utilizes mutexes and condition variables to synchronize process events.
- **ProcessBase**: Provides core functionalities for checking process status and handling termination.
- **SimulProcess**: Simulates workload for child processes with randomized sleep durations.
- **ProcessHelper**: Offers utilities for messaging and other process management tasks.
- **ProcessHandler**: Integrates the functionalities of all components to create, monitor, and manage child processes.

## Intended Behavior

### Intermediate and Leaf Processes

- **Leaf Process**: A child process that performs specific tasks and can be terminated independently.
- **Intermediate Process**: A parent or managing process that oversees leaf processes.

### Desired Behaviors

1. **Intermediate Process Dies if Leaf Dies**:  
   If a leaf process (child) dies, the intermediate process (parent) should recognize this and terminate itself or take appropriate action.

2. **Kill Leaf Process Without Intermediate Dying**:  
   It should be possible to terminate a leaf process without automatically causing the intermediate process to terminate.

### Implementation Considerations

- **Monitoring and Signaling**:  
  The intermediate process should actively monitor the state of its leaf processes through signals or other IPC mechanisms.  
  Implement logic to distinguish between different processes and handle their specific termination rules.

- **Handling Process Termination**:  
  When a leaf process terminates, the intermediate process should detect this (using `waitpid` with appropriate flags) and decide whether to terminate itself based on the program's logic.  
  To allow killing a leaf process without affecting the intermediate process, use signal handlers or flags to manage and control the process termination flow.

## Features

- **Process Creation**: Dynamically spawns a specified number of child processes.
- **Threaded Monitoring**: Each child process is monitored in its own thread to track its status and handle termination effectively.
- **IPC and Synchronization**: Ensures smooth communication and synchronization between processes using Messenger and Synchro.
- **Event Handling**: The main thread waits for and processes events as child processes exit.

## User Commands

Once the program is running, you can use the following commands to control processes:

- **`print on`**: Turn on the display of process progress.
- **`print off`**: Turn off the display of process progress.
- **`exit`**: Exit the program once all processes are done.
- **`terminate all`**: Terminate all processes and exit the program.
- **`terminate <pid>`**: Terminate a specific process by PID.
- **`kill all`**: Kill all processes and exit the program.
- **`kill <pid>`**: Kill a specific process by PID.
- **`respawn on`**: Turn on respawn.
- **`respawn off`**: Turn off respawn.
- **`display pids`**: Display all current process IDs (PIDs).
- **`help`**: Display the help message with available commands.

## Files

- **`src/waitPidProc.cpp`**: Implements the main process creation and management logic.
- **`CMakeLists.txt`**: Configuration file for CMake, specifying project settings and build instructions.
- **`src/messenger.cpp`**: Implements the messaging functionality.
- **`src/process_base.cpp`**: Contains base process functionalities.
- **`src/process_helper.cpp`**: Provides helper functions for process management.
- **`src/simul_process.cpp`**: Simulates process workloads.
- **`src/process_handler.cpp`**: Handles the integration and management of processes.
- **`src/process_interface.cpp`**: Defines the interface for process management.
- **`src/process.cpp`**: Implements the core process functionalities.
- **`include/messenger.h`**: Header file for the Messenger class.
- **`include/process_base.h`**: Header file for the ProcessBase class.
- **`include/process_helper.h`**: Header file for the ProcessHelper class.
- **`include/simul_process.h`**: Header file for the SimulProcess class.
- **`include/process_handler.h`**: Header file for the ProcessHandler class.
- **`include/process_interface.h`**: Header file for the ProcessInterface class.
- **`include/process.h`**: Header file for the Process class.
- **`include/synchro.h`**: Header file for synchronization utilities.
- **`src/synchro.cpp`**: Implements synchronization utilities.

## Building the Project

To build the project, perform the following steps:

1. **Install CMake**: Ensure CMake is installed on your system.
2. **Navigate to Project Directory**: Open a terminal and go to the project root directory.
3. **Configure the Project**:
   ```bash
   cmake -B build -S .
   ```
4. **Build the Project**:
   ```bash
   cmake --build build --parallel 3
   ```

## Running the Program

Once the project is built, execute the program using the command below. This will run the executable located in the `build` directory:

```bash
build/waitPidProc <command line options>
```
### Command-Line Options

The following command-line options can be used when running the program to customize its behavior:

- **`-n <number of processes>`**:  
  Specify the number of child processes to create. Defaults to 4 if not provided.  
  Example: `-n 5` to create 5 child processes.

- **`-t <process type>`**:  
  Specify the process type ('real' or 'simul'). Defaults to 'simul'.  
  Example: `-t real` to use real processes instead of simulated ones.

- **`-r <random upper limit>`**:  
  Set the upper limit for randomized sleep durations. Must be greater than 10.  
  Example: `-r 15` to set the random upper limit to 15.

- **`-d <display>`**:  
  Set display mode (0 for off, 1 for on).  
  Example: `-d 1` to turn on the display of process progress.

- **`-h`**:  
  Display usage information and exit the program.  
  Example: `-h` to show the help message with usage instructions.