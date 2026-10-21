#ifndef ALX64_PROCESS_HPP
#define ALX64_PROCESS_HPP

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int id;
    int uid;
    int tty;
    int threadCount;
} ProcessInfo;

#ifdef __cplusplus
}
#endif

#endif //ALX64_PROCESS_HPP
