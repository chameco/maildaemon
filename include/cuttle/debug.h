#ifndef CUTTLE_DEBUG_H
#define CUTTLE_DEBUG_H

#include <stdio.h>
#include <errno.h> 
#include <string.h>

#ifdef NDEBUG

#define debug(M, ...)
#else 
#define debug(M, ...) fprintf(stdout, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__) 

#ifndef REGISTER_DEBUG
#define regdebug(M, ...)
#else 
#define regdebug(M, ...) fprintf(stdout, "REGDEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__) 
#endif

#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) { fprintf(stderr, "Error: " M "\n", ##__VA_ARGS__); }

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__) 

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__) 

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; } 

#endif
