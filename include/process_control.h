#ifndef PROCESS_CONTROL_H
#define PROCESS_CONTROL_H
#include <string>
namespace process::controller
{
    void main();
    void parseArguments(int argc, char *argv[], int &numProcesses, std::string &processType, int &rndUpper);
} // namespace process::controller

#endif // PROCESS_CONTROL_H