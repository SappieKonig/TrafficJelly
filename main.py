from traffic_jelly.simulation import ComplexSimulation
from traffic_jelly.camera import StationaryCamera
from traffic_jelly.visualization import Game

import time

L = 5000
p = 1


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
    simulation = ComplexSimulation(length=L, spawn_prob_per_sec=p)
    for _ in range(L * 1000):
        simulation.step_forward()
    simulation.create_histogram()

if __name__ == '__main__':
    graphs()
