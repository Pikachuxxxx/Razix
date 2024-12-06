// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCommandLineParser.h"

namespace Razix {
    RZCommandLineParser::RZCommandLineParser()
    {
        AddCommand("help", {"--help"}, 0, "Show help");
        AddCommand("project filename", {"-f", "--project-file"}, 1, "The project file to start the application with");
        AddCommand("project filename", {"-s", "--scene"}, 1, "The scene file to load when the engine is fired");
        AddCommand("engine config filename", {"-cf", "--config-file"}, 1, "The engine config file to for the engine runtime settings");
        AddCommand("rendering api", {"-a", "--api"}, 1, "The Rendering API to use");
        AddCommand("validation", {"-v", "--validation"}, 0, "Enable Graphics API validation layers");
        AddCommand("vsync", {"-vs", "--vsync"}, 0, "Enable V-Sync");
        AddCommand("width", {"-w", "--width"}, 1, "Set window width");
        AddCommand("height", {"-h", "--height"}, 1, "Set window height");
    }

    void RZCommandLineParser::printHelp()
    {
        std::cout << "Available command line options:\n";
        for (auto& option: m_CommandOptions) {
            std::cout << " ";
            for (sz i = 0; i < option.second.commandFlags.size(); i++) {
                std::cout << option.second.commandFlags[i];
                if (i < option.second.commandFlags.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << ": " << option.second.helpDesc << "\n";
        }
        std::cout << "\n";
    }

    bool RZCommandLineParser::isSet(std::string name)
    {
        return ((m_CommandOptions.find(name) != m_CommandOptions.end()) && m_CommandOptions[name].set);
    }

    std::string RZCommandLineParser::getValueAsString(std::string name)
    {
        RAZIX_CORE_ASSERT(m_CommandOptions.find(name) != m_CommandOptions.end(), "No value has been passed to the argument");
        std::string value = m_CommandOptions[name].value;
        return value;
    }

    int32_t RZCommandLineParser::getValueAsInt(std::string name)
    {
        RAZIX_CORE_ASSERT(m_CommandOptions.find(name) == m_CommandOptions.end(), "No value has been passed to the argument");
        std::string value = m_CommandOptions[name].value;
        char*       numConvPtr;
        int32_t     intVal = strtol(value.c_str(), &numConvPtr, 10);
        return intVal;
    }

    void RZCommandLineParser::AddCommand(std::string name, std::vector<std::string> commands, bool hasValue, std::string help)
    {
        m_CommandOptions[name].commandFlags = commands;
        m_CommandOptions[name].helpDesc     = help;
        m_CommandOptions[name].set          = false;
        m_CommandOptions[name].hasValue     = hasValue;
        m_CommandOptions[name].value        = "";
    }

    void RZCommandLineParser::parse(std::vector<cstr>& arguments)
    {
        bool printH = false;
        // Known arguments
        for (auto& option: m_CommandOptions) {
            for (auto& command: option.second.commandFlags) {
                for (sz i = 0; i < arguments.size(); i++) {
                    if (strcmp(arguments[i], command.c_str()) == 0) {
                        option.second.set = true;
                        // Get value
                        if (option.second.hasValue) {
                            if (arguments.size() > i + 1) {
                                option.second.value = arguments[i + 1];
                            }
                            if (option.second.value == "") {
                                printH = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        // Print help for unknown arguments or missing argument values
        if (printH) {
            m_CommandOptions["help"].set = true;
        }

        // If help is set print it
        if (isSet("help"))
            printHelp();
    }
}    // namespace Razix