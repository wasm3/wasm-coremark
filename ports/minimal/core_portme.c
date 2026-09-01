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
#include "coremark.h"

/* The one thing the host has to provide: a millisecond clock. */
__attribute__((import_module("env"), import_name("clock_ms")))
extern unsigned long long
clock_ms(void);

/* core_main.c, renamed by -Dmain=coremark_main in core_portme.mak. */
void coremark_main(void);

void *
portable_malloc(size_t size)
{
    (void)size;
    return NULL;
}

void
portable_free(void *p)
{
    (void)p;
}

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile = 0x0;
volatile ee_s32 seed2_volatile = 0x0;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif
volatile ee_s32 seed4_volatile = ITERATIONS;
volatile ee_s32 seed5_volatile = 0;

/* Porting: Timing functions */
#define EE_TICKS_PER_SEC 1000

static CORE_TICKS start_time_val, stop_time_val;

void
start_time(void)
{
    start_time_val = clock_ms();
}

void
stop_time(void)
{
    stop_time_val = clock_ms();
}

CORE_TICKS
get_time(void)
{
    return stop_time_val - start_time_val;
}

secs_ret
time_in_secs(CORE_TICKS ticks)
{
    return ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
}

ee_u32 default_num_contexts = MULTITHREAD;

/* Nothing to print to, but core_main.c and core_util.c call this on error
   paths, so it has to exist. */
int
ee_printf(const char *fmt, ...)
{
    (void)fmt;
    return 0;
}

void
portable_init(core_portable *p, int *argc, char *argv[])
{
    (void)argc;
    (void)argv;
    p->portable_id = 1;
}

void
portable_fini(core_portable *p)
{
    p->portable_id = 0;
}

/* Exported entry point: run the benchmark and hand back the score. */
static double reported_score;

void
coremark_report_score(double score)
{
    reported_score = score;
}

__attribute__((export_name("run"))) float
run(void)
{
    reported_score = 0;
    coremark_main();
    return (float)reported_score;
}
