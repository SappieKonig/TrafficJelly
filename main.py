from traffic_jelly.simulation import BasicSimulation, ComplexSimulation
from traffic_jelly.camera import StationaryCamera
from traffic_jelly.visualization import Game

import time


def main():
    simulation = ComplexSimulation(length=5000, spawn_prob_per_sec=.6)
    car_updates = 0
    start = time.time()
    for _ in range(int(1e3 / simulation.get_delta_time())):
        car_updates += len(simulation.vehicles)
        simulation.step_forward()
    end = time.time()
    print(f'Average car updates per second: {car_updates / (end - start):.2f}')
    simulation.cause_breakdown(1000)
    camera = StationaryCamera(500)
    Game(
        simulation=simulation,
        camera=camera
    ).main()


if __name__ == '__main__':
    main()
