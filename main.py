from traffic_jelly.simulation import ComplexSimulation
from traffic_jelly.camera import StationaryCamera
from traffic_jelly.visualization import Game

import numpy as np
import time

L = 5000
p = 0.5


def main():
    simulation = ComplexSimulation(length=L, spawn_prob_per_sec=p)
    car_updates = 0
    start = time.time()
    for _ in range(int(1e3 / simulation.get_delta_time())):
        car_updates += len(simulation.vehicles)
        simulation.step_forward()
    end = time.time()
    print(f'Average car updates per second: {car_updates / (end - start):.2f}')
    camera = StationaryCamera(500)
    Game(
        simulation=simulation,
        camera=camera
    ).main()

def graphs():
    for p in [i * 0.1 for i in range(1, 11)]:
        simulation = ComplexSimulation(length=L, spawn_prob_per_sec=p, mode = "single")
        for _ in range(L * 100):
            simulation.step_forward()
            if _ == L * 50:
                simulation.time_taken = []
        simulation.create_histogram()

if __name__ == '__main__':
    graphs()
