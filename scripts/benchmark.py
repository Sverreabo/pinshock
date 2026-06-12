from time import perf_counter
from os import system

EXECUTABLE = "./new_pinshock.out -q"
T_STOP = 120

t0 = perf_counter()
runs = 0

times = []

while perf_counter() - t0 < T_STOP:
    if system(EXECUTABLE):
        exit(1)
    runs += 1

t_total = perf_counter() - t0
ms_per_run = 1000 * t_total / runs

print(f"Completed {runs} runs in {t_total:.2f} seconds.")
print(f"Avg. time per run: {ms_per_run:.2f} ms")