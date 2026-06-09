# This is meant to be a genetic algorithm for tuning
# For now it just iterates through possible values for cache_clear_size
# In the future: Tuning cache size

import random
from time import perf_counter, sleep
from os import system

EXECUTABLE = ".././new_pinshock.out"

class Proposal:
    def __init__(self, cache_clear_size):
        self.cache_clear_size = cache_clear_size

    def fitness(self):
        t0 = perf_counter()
        if system(f"{EXECUTABLE} -q -m {self.cache_clear_size} >/dev/null"):
            exit(1)
        return perf_counter() - t0

if system("cd .. && make clean && make -j8"):
    exit(1)

x_verdier = []
y_verdier = []
for i in range(1, 100, 1):
    sleep(0.1)
    results = [Proposal(i).fitness() for _ in range(100)]
    results.sort()
    results = results[1:-1]
    fitness = sum(results) / len(results)

    plusminus = (max(results) - min(results)) / 2

    print(f"{i}: {fitness:.3f} +- {plusminus:.3f}", flush=True)


    x_verdier.append(i)
    y_verdier.append(fitness)



from csv_handler import write_data
write_data(x_verdier, y_verdier)
