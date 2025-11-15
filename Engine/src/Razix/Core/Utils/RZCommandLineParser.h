#pragma once

#include "Razix/Core/Containers/string.h"

namespace Razix {
    /**
     * Parses the command line arguments for the Razix Application
     */
    class RAZIX_API RZCommandLineParser
    {
    public:
        /* Command Line Option information */
        struct CommandLineOption
        {
            RZDynamicArray<RZString> commandFlags;     /* The list of flags that can be used to call the command                               */
            RZString                 value;            /* The value passed along with the command flag                                         */
            RZString                 helpDesc;         /* The description of the command                                                       */
            bool                     set      = false; /* A boolean to indicate whether the command was set or not                             */
            bool                     hasValue = false; /* Boolean to check is the command has any value, if set true, it read the value passed */
        };

    public:
        RZCommandLineParser();
        ~RZCommandLineParser() {}

        /**
         * Parses all the list of arguments passed from the command line
         *
         * @param arguments The list of all the arguments to parse
         */
        void parse(RZDynamicArray<const char*>& arguments);

        /**
         * prints the description of all command available and their description
         */
        void printHelp();
        /**
         * Tells whether or not if the options was passed in the command line or not
         * 
         * @param name The name of the command to check it it was set
         * 
         * @returns True, if the flag was set
         */
        bool     isSet(RZString name);
        RZString getValueAsString(RZString name);
        int32_t  getValueAsInt(RZString name);

        /**
         * Adds the command which can be parsed from the command line
         *
         * @param name      The name of the command
         * @param commands  List of flags that can be used to invoke the command
         * @param hasValue  Denotes that the command takes a value after the flag
         * @param help      Description of the command
         */
        void addCommand(RZString name, RZDynamicArray<RZString> commands, bool hasValue, RZString help);

    private:
        RZHashMap<RZString, CommandLineOption> m_CommandOptions; /* The list of all command line options available, if passed via command line, it is set */
    };
}    // namespace Razix
