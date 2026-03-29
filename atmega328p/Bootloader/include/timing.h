#ifndef TIMING_H
#define TIMING_H

#define CYCLES_PER_MS      (F_CPU / 1000UL)
#define LOOP_CYCLES        6UL
#define ITER_PER_MS        (CYCLES_PER_MS / LOOP_CYCLES)

#endif