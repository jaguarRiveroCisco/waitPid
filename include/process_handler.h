#ifndef PROCESS_HANDLER_H
#define PROCESS_HANDLER_H
#include "controller_base.h"

namespace process
{
    class Controller final : public ControllerBase {
    public:
        static void run(const std::string &processType, int numProcesses);

    private:
        static void waitForEvents();
        static void createHandlers();
        static void createHandlers_(int numHandlers);
        static void createHandler();
    };
} // namespace process

#endif // PROCESS_HANDLER_H
