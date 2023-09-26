from traffic_jelly.simulation import BasicSimulation, ComplexSimulation
from traffic_jelly.camera import StationaryCamera
from traffic_jelly.visualization import Game


def main():
    simulation = ComplexSimulation(length=5000, spawn_prob_per_sec=.3)
    for _ in range(int(1e3 / simulation.get_delta_time())):
        simulation.step_forward()
    simulation.cause_breakdown(1000)
    camera = StationaryCamera(500)
    Game(
        simulation=simulation,
        camera=camera
    ).main()


if __name__ == '__main__':
    main()
