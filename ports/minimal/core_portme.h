/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/
/* Topic: Description
        Port for a bare WebAssembly module: no WASI, no libc I/O, no command
        line. The host supplies the clock as an `env.clock_ms` import, and the
        module exports `run`, which returns the CoreMark score as an f32.

        Derived from the `barebones` port.
*/
#ifndef CORE_PORTME_H
#define CORE_PORTME_H

#include <stddef.h>

/************************/
/* Data types and settings */
/************************/
/* Configuration: HAS_FLOAT
        Define to 1 if the platform supports floating point.
*/
#ifndef HAS_FLOAT
#define HAS_FLOAT 1
#endif
/* Configuration: HAS_TIME_H
        There is no time.h here; timing goes through the clock_ms import.
*/
#ifndef HAS_TIME_H
#define HAS_TIME_H 0
#endif
/* Configuration: USE_CLOCK
        clock() would resolve to the process CPU clock, which a bare module
        has no access to.
*/
#ifndef USE_CLOCK
#define USE_CLOCK 0
#endif
/* Configuration: HAS_STDIO
        Define to 1 if the platform has stdio.h.
*/
#ifndef HAS_STDIO
#define HAS_STDIO 0
#endif
/* Configuration: HAS_PRINTF
        Nothing to print to; ee_printf is a stub in core_portme.c.
*/
#ifndef HAS_PRINTF
#define HAS_PRINTF 0
#endif

/* Definitions: COMPILER_VERSION, COMPILER_FLAGS, MEM_LOCATION
        Initialize these strings per platform
*/
#ifndef COMPILER_VERSION
#ifdef __clang__
#define COMPILER_VERSION "Clang " __clang_version__
#elif defined(__GNUC__)
#define COMPILER_VERSION "GCC" __VERSION__
#else
#define COMPILER_VERSION "Please put compiler version here (e.g. gcc 4.1)"
#endif
#endif
#ifndef COMPILER_FLAGS
#define COMPILER_FLAGS FLAGS_STR
#endif
#ifndef MEM_LOCATION
#define MEM_LOCATION "STATIC"
#endif

/* Data Types:
        To avoid compiler issues, define the data types that need ot be used for
   8b, 16b and 32b in <core_portme.h>.

        *Imprtant*:
        ee_ptr_int needs to be the data type used to hold pointers, otherwise
   coremark may fail!!!
*/
typedef signed short   ee_s16;
typedef unsigned short ee_u16;
typedef signed int     ee_s32;
typedef double         ee_f32;
typedef unsigned char  ee_u8;
typedef unsigned int   ee_u32;
typedef ee_u32         ee_ptr_int;
typedef size_t         ee_size_t;
/* align_mem:
        This macro is used to align an offset to point to a 32b value. It is
   used in the Matrix algorithm to initialize the input memory blocks.
*/
#define align_mem(x) (void *)(4 + (((ee_ptr_int)(x)-1) & ~3))

/* Configuration: CORE_TICKS
        Milliseconds, as handed out by the clock_ms import.
 */
#define CORETIMETYPE unsigned long long
typedef unsigned long long CORE_TICKS;

/* Configuration: SEED_METHOD
        There is no command line, so the seeds are compiled in.
*/
#ifndef SEED_METHOD
#define SEED_METHOD SEED_VOLATILE
#endif

/* Configuration: MEM_METHOD
        A static block keeps the module free of a heap.
*/
#ifndef MEM_METHOD
#define MEM_METHOD MEM_STATIC
#endif

/* Configuration: MULTITHREAD
        Define for parallel execution. A bare module runs one context.
*/
#ifndef MULTITHREAD
#define MULTITHREAD 1
#define USE_PTHREAD 0
#define USE_FORK    0
#define USE_SOCKET  0
#endif

/* Configuration: MAIN_HAS_NOARGC
        Needed if platform does not support getting arguments to main.
*/
#ifndef MAIN_HAS_NOARGC
#define MAIN_HAS_NOARGC 1
#endif

/* Configuration: MAIN_HAS_NORETURN
        The score leaves through coremark_report_score, not through a return
        value, so main returns nothing.
*/
#ifndef MAIN_HAS_NORETURN
#define MAIN_HAS_NORETURN 1
#endif

/* Configuration: CORE_REPORT_SCORE
        Makes core_main.c hand the final score to coremark_report_score. See
        core_main-report-score.patch.
*/
#define CORE_REPORT_SCORE 1
void coremark_report_score(double score);

/* Variable: default_num_contexts
        Not used for this simple port, must contain the value 1.
*/
extern ee_u32 default_num_contexts;

typedef struct CORE_PORTABLE_S
{
    ee_u8 portable_id;
} core_portable;

/* target specific init/fini */
void portable_init(core_portable *p, int *argc, char *argv[]);
void portable_fini(core_portable *p);

#if !defined(PROFILE_RUN) && !defined(PERFORMANCE_RUN) \
    && !defined(VALIDATION_RUN)
#if (TOTAL_DATA_SIZE == 1200)
#define PROFILE_RUN 1
#elif (TOTAL_DATA_SIZE == 2000)
#define PERFORMANCE_RUN 1
#else
#define VALIDATION_RUN 1
#endif
#endif

int ee_printf(const char *fmt, ...);

#endif /* CORE_PORTME_H */
