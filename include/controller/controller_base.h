#include "base_handler.h"
#include "process_interface.h"

namespace process
{
    enum class LoggingType {
        Console,
        File
    };

    class ControllerBase : public BaseHandler 
    {
        public:
            static bool    &running();
            static bool    &respawn();
            static LoggingType &loggingType();
            static void  terminateAll();
            static void  terminateProcessByPid(pid_t pid);
            static void intProcessByPid(pid_t pid);
            static void intAll();
            static void  killAll();
            static void  killProcessByPid(pid_t pid);
            static void  displayAllPids();
            static Synchro *synchro();
            void            init(Synchro *synchro, std::unique_ptr<IProcess> process);
            void            start();
            static void     setProcessType(const std::string &processType);
            static void     setLoggingType(LoggingType type);
            static LoggingType getLoggingType();
            static std::string loggingTypeToString(); // New method
            static void         stopAllMonitors();

        protected:
            static std::vector<std::unique_ptr<ControllerBase>> handlers_;
            static void setNumProcesses(int numProcesses) { numProcesses_ = numProcesses; }
            static int                                          numProcesses_;
            static std::string                                  processType_;
            static LoggingType loggingType_;
        private:
            void                      createChild();
            std::unique_ptr<IProcess> process_;
            static bool               running_;
            static bool               respawn_;
    };
}