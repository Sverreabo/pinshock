# This is meant to be a genetic algorithm for tuning
# For now it just iterates through possible values for cache_bottom_ignore
# In the future: Tuning cache size

import random
from time import perf_counter, sleep
from os import system

EXECUTABLE = "./pinshock.out"

class Proposal:
    def __init__(self, cache_bottom_ignore):
        self.cache_bottom_ignore = cache_bottom_ignore

    def fitness(self):
        t0 = perf_counter()
        system(f"{EXECUTABLE} {self.cache_bottom_ignore} 0 >/dev/null")
        return perf_counter() - t0

for i in range(0, 10):
    sleep(1)
    results = [Proposal(i).fitness() for _ in range(10)]
    fitness = sum(results) / len(results)
    print(f"{i}: {fitness:.3f}s")
