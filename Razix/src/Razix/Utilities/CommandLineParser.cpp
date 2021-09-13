#include "rzxpch.h"
#include "CommandLineParser.h"

namespace Razix
{
    CommandLineParser::CommandLineParser()
    {
        AddCommand("help", { "--help" }, 0, "Show help");
        AddCommand("project filename", { "-f", "--project-file" }, 1, "The project file to start the application with");
        AddCommand("engine config filename", { "-cf", "--config-file" }, 1, "The engine config file to for the engine runtime settings");
        AddCommand("rendering api", { "-a", "--api" }, 1, "The Rendering API to use");
        AddCommand("validation", { "-v", "--validation" }, 0, "Enable Graphics API validation layers");
        AddCommand("vsync", { "-vs", "--vsync" }, 0, "Enable V-Sync");
        AddCommand("width", { "-w", "--width" }, 1, "Set window width");
        AddCommand("height", { "-h", "--height" }, 1, "Set window height");
        //AddCommand("gpuselection", { "-gs", "--gpu-selected" }, 1, "the gpu be");
        AddCommand("gpulist", { "-gl", "--gpu-list" }, 0, "Display a list of available Vulkan devices");
        AddCommand("profile", { "-p", "--profile" }, 0, "Run game in profiling mode");
        AddCommand("profile-frames", { "-pf", "--profile-frames" }, 1, "Profiles the game upto the specified frames");
        AddCommand("profile-time", { "-pt", "--profile-time" }, 1, "Profiles the game upto a specified time");
    }

    void CommandLineParser::PrintHelp()
    {
        std::cout << "Available command line options:\n";
        for (auto& option : m_CommandOptions) {
            std::cout << " ";
            for (size_t i = 0; i < option.second.commandFlags.size(); i++) {
                std::cout << option.second.commandFlags[i];
                if (i < option.second.commandFlags.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << ": " << option.second.helpDesc << "\n";
        }
        std::cout << "\n";
    }

    bool CommandLineParser::IsSet(std::string name)
    {
        return ((m_CommandOptions.find(name) != m_CommandOptions.end()) && m_CommandOptions[name].set);
    }

    std::string CommandLineParser::GetValueAsString(std::string name)
    {
        RAZIX_CORE_ASSERT(m_CommandOptions.find(name) != m_CommandOptions.end(), "No value has been passed to the argument");
        std::string value = m_CommandOptions[name].value;
        return value;
    }

    int32_t CommandLineParser::GetValueAsInt(std::string name)
    {
        RAZIX_CORE_ASSERT(m_CommandOptions.find(name) == m_CommandOptions.end(), "No value has been passed to the argument");
        std::string value = m_CommandOptions[name].value;
        char* numConvPtr;
        int32_t intVal = strtol(value.c_str(), &numConvPtr, 10);
        return intVal;
    }

    void CommandLineParser::AddCommand(std::string name, std::vector<std::string> commands, bool hasValue, std::string help)
    {
        m_CommandOptions[name].commandFlags = commands;
        m_CommandOptions[name].helpDesc = help;
        m_CommandOptions[name].set = false;
        m_CommandOptions[name].hasValue = hasValue;
        m_CommandOptions[name].value = "";
    }

    void CommandLineParser::parse(std::vector<const char*>& arguments)
    {
        bool printHelp = false;
        // Known arguments
        for (auto& option : m_CommandOptions) {
            for (auto& command : option.second.commandFlags) {
                for (size_t i = 0; i < arguments.size(); i++) {
                    if (strcmp(arguments[i], command.c_str()) == 0) {
                        option.second.set = true;
                        // Get value
                        if (option.second.hasValue) {
                            if (arguments.size() > i + 1) {
                                option.second.value = arguments[i + 1];
                            }
                            if (option.second.value == "") {
                                printHelp = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        // Print help for unknown arguments or missing argument values
        if (printHelp) {
            m_CommandOptions["help"].set = true;
        }
        
        // If help is set print it
        if (IsSet("help"))
            PrintHelp();
    }
}