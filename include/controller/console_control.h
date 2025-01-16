#ifndef PROCESS_CONTROL_H
#define PROCESS_CONTROL_H
#include <string>
namespace cli::driver
{
    void main();
    void parseArguments(int argc, char *argv[], int &numProcesses, std::string &processType);
    template<typename T> void printpid(const std::string &str, const T &x = T());
    int LoggerExample();

} // namespace cli::driver

#endif // PROCESS_CONTROL_H