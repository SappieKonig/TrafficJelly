from __future__ import annotations
from traffic_model import TrafficModel
import time
import matplotlib.pyplot as plt

from graphviz.game import Game
from graphviz.simulation import Simulation
from graphviz.view.game_graph_view import GameGraphView
from graphviz.view.game_road_view import GameRoadView


DELTA_TIME = 0.2
SCALE = 1/6

# fromNode, toNode, timeTaken, startTime
ArrivalStats = tuple[int, int, float, float]

def plot_arrival_stats_hist(arrival_stats: list[ArrivalStats], from_node_id: int, to_node_id: int):
    filtered_arrival_stats = [arrival_stat for arrival_stat in arrival_stats if arrival_stat[0] == from_node_id and arrival_stat[1] == to_node_id]
    times_taken = [arrival_stat[2] for arrival_stat in filtered_arrival_stats]
    plt.hist(times_taken, bins=30)
    plt.show()


def main():
    cars_per_edge = []
    simulation = TrafficModel("graph.txt", DELTA_TIME, SCALE)
    start = time.time()
    steps_per_day = int(3600 * 9 / DELTA_TIME * SCALE)
    for i in range(steps_per_day):
        if i % 100 == 0:
            cars_per_edge.append(simulation.get_n_cars_per_edge())
            time_taken = time.time() - start
            print(f"Time taken for 100 steps: {time_taken:.2f}")
            start = time.time()
            n_cars = simulation.get_n_cars_in_simulation()
            print(f"Number of cars in simulation: {n_cars}")
            print(i)
        simulation.step_forward()
    # simulation = create_simulation()
    arrival_stats = simulation.get_travel_stats()
    filtered_arrival_stats = [arrival_stat for arrival_stat in arrival_stats if arrival_stat[0] == 0 and arrival_stat[1] == 1]
    # plot_arrival_stats_hist(arrival_stats, 0, 1)
    game = Game(simulation=simulation)
    game.push_view(GameGraphView(game=game))
    game.main()


def create_simulation():
    simulation = Simulation()
    edge_ids = simulation.get_edge_ids()
    assert edge_ids
    # noinspection PyProtectedMember
    simulation._add_cars_to_edge(edge_ids[0], 50)
    return simulation


if __name__ == '__main__':
    main()
