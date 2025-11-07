// clang-format off
#include "rzxpch.h"
// clang-format on

#if defined(RAZIX_DEBUG) && defined(RAZIX_PLATFORM_MACOS)

    #include <csignal>        // For signal handling (sigaction, SIGSEGV, etc.)
    #include <ctime>          // For generating timestamps (std::time, std::strftime)
    #include <exception>      // For handling specific exceptions like EXC_BAD_ACCESS
    #include <fstream>        // For writing to a file (std::ofstream)
    #include <iostream>       // For std::cerr and std::cout
    #include <mach/exc.h>     // For EXC_BAD_ACCESS, EXC_MASK_BAD_ACCESS, etc.
    #include <mach/mach.h>    // For macOS-specific thread and register access (mach_thread_state_t)
    #include <sys/types.h>    // For process and thread types (pid_t, etc.)
    #include <unistd.h>       // For getpid (process ID)

    #include "Razix/Core/Containers/string.h"

namespace Razix::CrashDumpHandler {

    static void writeToOutput(std::ostream& output, const RZString& message)
    {
        output << message.c_str();
        std::cout << message.c_str();    // Simultaneously print to console
    }

    // Function to write crash dump to file and console
    static void writeCrashDump(const RZString& exception, const RZString& description, const siginfo_t* info, ucontext_t* context)
    {
        std::time_t now = std::time(nullptr);
        char        filename[64];
        std::strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S.crashdump", std::localtime(&now));

        std::ofstream dumpFile(filename);
        if (!dumpFile.is_open()) {
            std::cerr << "Failed to write crash dump file: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }

        // Unified output stream (file and console)
        auto write = [&dumpFile](const RZString& message) {
            writeToOutput(dumpFile, message);
        };

        // Write exception info
        write("Exception: SIGNAL " + rz_to_string(info->si_signo) + "\n");
        write("Description: " + description + "\n\n");

        // Write register and thread information (ARM64 specific)
        write("Control Registers:\n");
        write("RIP = 0x" + rz_to_string(context->uc_mcontext->__ss.__pc) + "\n");         // Program Counter (equivalent of RIP)
        write("RSP = 0x" + rz_to_string(context->uc_mcontext->__ss.__sp) + "\n");         // Stack Pointer
        write("FP  = 0x" + rz_to_string(context->uc_mcontext->__ss.__fp) + "\n");         // Frame Pointer
        write("CPSR = 0x" + rz_to_string(context->uc_mcontext->__ss.__cpsr) + "\n\n");    // Current Program Status Register

        write("Integer Registers:\n");
        for (int i = 0; i < 29; ++i) {    // ARM64 has 29 general-purpose registers (x0-x28)
            write("X" + rz_to_string(i) + " = 0x" + rz_to_string(context->uc_mcontext->__ss.__x[i]) + "\n");
        }
        write("X29 (FP) = 0x" + rz_to_string(context->uc_mcontext->__ss.__fp) + "\n");
        write("X30 (LR) = 0x" + rz_to_string(context->uc_mcontext->__ss.__lr) + "\n");    // Link Register

        // Signal-specific information
        write("\nAttempt to access memory address: 0x" + rz_to_string(reinterpret_cast<uintptr_t>(info->si_addr)) + "\n");

        // Close the dump file
        dumpFile.close();
        std::cerr << "Crash dump written to: " << filename << std::endl;
    }

    // Mach exception handler
    static kern_return_t exceptionHandler(exception_type_t exception, exception_data_t code, mach_msg_type_number_t codeCount)
    {
        if (exception == EXC_BAD_ACCESS) {
            std::cerr << "EXC_BAD_ACCESS: Invalid memory access detected!" << std::endl;

            // Capture relevant information (code, address, etc.)
            //            uint64_t address = code[0]; // address causing the issue
            //            uint64_t accessType = code[1]; // type of access (read/write)

            // Write crash dump to file
            RZString description = "EXC_BAD_ACCESS: Invalid memory access.";
            writeCrashDump("EXC_BAD_ACCESS", description, nullptr, nullptr);
        } else {
            std::cerr << "Other exception: " << exception << std::endl;
        }

        // Return to default exception handling (crash program)
        return EXCEPTION_DEFAULT;
    }

    // Signal handler for crash handling
    static void signalHandler(int signal, siginfo_t* info, void* context)
    {
        RZString description;
        switch (signal) {
            case SIGSEGV:    // Segmentation Fault
                description = "Segmentation fault or invalid memory access.";
                break;
            case SIGFPE:    // Floating-point exception (e.g., division by zero)
                description = "Floating point exception (e.g., division by zero).";
                break;
            case SIGILL:    // Illegal instruction
                description = "Illegal instruction.";
                break;
            default:
                description = "Unknown signal received.";
                break;
        }

        // Write the crash dump
        writeCrashDump("Signal " + rz_to_string(signal), description, info, static_cast<ucontext_t*>(context));

        // Exit after handling the crash
        exit(signal);
    }

    // Setup Mach exception handler and signal handlers
    void Initialize()
    {
        // Set up Mach exception handler
        exception_mask_t mask = EXC_MASK_BAD_ACCESS;
        mach_port_t      exceptionPort;
        task_set_exception_ports(mach_task_self(), mask, exceptionPort, EXCEPTION_DEFAULT, THREAD_STATE_NONE);

        struct sigaction sa;
        sa.sa_flags     = SA_SIGINFO;
        sa.sa_sigaction = signalHandler;
        sigemptyset(&sa.sa_mask);

        // Handle common crash signals
        if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
            std::cerr << "Failed to set up SIGSEGV handler." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (sigaction(SIGFPE, &sa, nullptr) == -1) {
            std::cerr << "Failed to set up SIGFPE handler." << std::endl;
            exit(EXIT_FAILURE);
        }
        if (sigaction(SIGILL, &sa, nullptr) == -1) {
            std::cerr << "Failed to set up SIGILL handler." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Cross-platform WriteCrashDump function
    void WriteCrashDump(int signal, const RZString& description)
    {
        // For macOS, this is automatically handled by the signal and Mach exception handlers.
    }

}    // namespace Razix::CrashDumpHandler

#endif
