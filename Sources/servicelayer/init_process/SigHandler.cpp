#include "SigHandler.hpp"

#include <ALX64/defines.h>
#include <ALX64/kmsg.h>
#include <ALX64/Power.h>

#include <csignal>
#include <execinfo.h>
#include <unistd.h>

static kmsg_t* kmsg = nullptr;

static void sigsegv_handler_impl(siginfo_t* info) {
    if (info->si_code > 0) {
        if (getpid() == 1) {
            kmsg_write(kmsg, LOGLEVEL_EMERG, "init: fault captured - forcing reboot");
            Power__PerformForcedReboot();
            for (;;);
        }

        kmsg_write(kmsg, LOGLEVEL_CRIT, "init: fault captured in user-interactive shell - exiting");

        struct sigaction sa{};
        sa.sa_handler = SIG_DFL;
        sigaction(SIGSEGV, &sa, nullptr);
        raise(SIGSEGV);
        return;
    }

    kmsg_write(kmsg, LOGLEVEL_WARNING, "init: fault captured, no data captured");
}

NAKED_METHOD static void signal_handler_sigsegv_impl(int sig) {
#ifdef __x86_64__
    asm volatile (
        "mov %rdx, %rdi\n\t"
        "jmp sigsegv_handler_impl\n\t"
    );
#elif defined(__aarch64__)
    asm volatile (
        "mov x0, x2\n\t"
        "b sigsegv_handler_impl\n\t"
    );
#else
#error "Unsupported architecture detected"
#endif
}

static void signal_handler_impl(int sig) {
    if ((getpid() == 1 && getuid() == 0) && (sig != SIGUSR1 && sig != SIGUSR2)) {
        Power__PerformReboot("signal_catch");
        return;
    }

    _exit(sig);
}

namespace Boot {
    void RegisterSignalHandler() {
        kmsg = kmsg_open();
        signal(SIGSEGV, signal_handler_sigsegv_impl);
        signal(SIGTERM, signal_handler_impl);
        signal(SIGINT, signal_handler_impl);
        signal(SIGABRT, signal_handler_impl);
        signal(SIGUSR1, signal_handler_impl);
        signal(SIGUSR2, signal_handler_impl);
    }
}
