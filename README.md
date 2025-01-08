# Process Management Program

## Overview

The Process Management Program is a C++ application designed to handle and monitor multiple child processes. It utilizes threading, inter-process communication (IPC), and synchronization to efficiently manage process creation and lifecycle events.

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
   - If a leaf process (child) dies, the intermediate process (parent) should recognize this and terminate itself or take appropriate action.

2. **Kill Leaf Process Without Intermediate Dying**:
   - It should be possible to terminate a leaf process without automatically causing the intermediate process to terminate.

### Implementation Considerations

- **Monitoring and Signaling**:
  - The intermediate process should actively monitor the state of its leaf processes. This can be achieved through signals or other IPC mechanisms.
  - Implement logic to distinguish between different processes and handle their specific termination rules.

- **Handling Process Termination**:
  - When a leaf process terminates, the intermediate process should detect this (using `waitpid` with appropriate flags) and decide whether to terminate itself based on the program's logic.
  - To allow killing a leaf process without affecting the intermediate process, use signal handlers or flags to manage and control the process termination flow.

## Features

- **Process Creation**: Dynamically spawns a specified number of child processes.
- **Threaded Monitoring**: Each child process is monitored in its own thread to track its status and handle termination effectively.
- **IPC and Synchronization**: Ensures smooth communication and synchronization between processes using Messenger and Synchro.
- **Event Handling**: The main thread waits for and processes events as child processes exit.

## Files

- **`src/waitPidProc.cpp`**: Implements the main process creation and management logic.
- **`CMakeLists.txt`**: Configuration file for CMake, specifying project settings and build instructions.

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
build/waitPidProc [numprocs]
```

- **`[numprocs]`**: Optional parameter to specify the number of child processes to create. Defaults to 4 if not provided.

Upon execution, the program will display output detailing the roles of the child and parent processes, along with the exit status of each child process.