# This is meant to be a genetic algorithm for tuning
# For now it just iterates through possible values for cache_bottom_ignore
# In the future: Tuning cache size

import random
from time import perf_counter, sleep
from os import system

EXECUTABLE = "./new_pinshock.out"

class Proposal:
    def __init__(self, cache_clear_size):
        self.cache_clear_size = cache_clear_size

    def fitness(self):
        t0 = perf_counter()
        if system(f"{EXECUTABLE} -m {self.cache_clear_size} >/dev/null"):
            exit(1)
        return perf_counter() - t0

if system("make -j 8"):
    exit(1)

x_verdier = []
y_verdier = []
for i in range(100, 20000, 200):
    sleep(0.5)
    results = [Proposal(i).fitness() for _ in range(100)]
    fitness = sum(results) / len(results)

    plusminus = (max(results) - min(results)) / 2

    print(f"{i}: {fitness:.3f} +- {plusminus:.3f}", flush=True)


    x_verdier.append(i)
    y_verdier.append(fitness)

import matplotlib.pyplot as plt
plt.plot(x_verdier, y_verdier)
plt.savefig("fig3.png")
