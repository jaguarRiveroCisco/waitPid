#include "cli_controller.h"

namespace cli::driver
{
    void CLIController::runThread()
    {
        std::string input;
        while (!stopFlag)
        {
            try
            {
                if (std::cin.peek() != EOF)
                {
                    std::getline(std::cin, input);
                    if (validateInput(input))
                    {
                        commandFunc(input);
                    }
                    else
                    {
                        std::cerr << "Invalid input: " << input << std::endl;
                    }
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    bool CLIController::validateInput(const std::string &input)
    {
        // Perform input validation
        return !input.empty(); // Example validation
    }
} // namespace cli::driver
