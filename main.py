import os
#os.system("./build.sh")

from traffic_model import TrafficModel
import time
import matplotlib.pyplot as plt
import numpy as np

from graphviz.game import Game
from graphviz.simulation import Simulation
from graphviz.view.game_graph_view import GameGraphView
from graphviz.view.game_road_view import GameRoadView


DELTA_TIME = 0.5
SCALE = 1

# fromNode, toNode, timeTaken, startTime
ArrivalStats = tuple[int, int, float, float]


def plot_arrival_stats_hist(arrival_stats: list[ArrivalStats], from_node_id: int, to_node_id: int):
    filtered_arrival_stats = [arrival_stat for arrival_stat in arrival_stats if arrival_stat[0] == from_node_id and arrival_stat[1] == to_node_id]
    times_taken = [arrival_stat[2] for arrival_stat in filtered_arrival_stats]
    plt.hist(times_taken, bins=30)
    # save plot to file as svg
    plt.savefig("arrival_stats.svg")


def main():
    cars_per_edge = []
    simulation = TrafficModel("graph.txt", DELTA_TIME, SCALE)
    start = time.time()
    steps_per_day = int(3600 * 24 / DELTA_TIME * SCALE)
    until = int(10 / 24 * steps_per_day)
    for i in range(until):
        if i % 100 == 0:
            cars_per_edge.append(simulation.get_n_cars_per_edge())
            time_taken = time.time() - start
            print(f"Time taken for 100 steps: {time_taken:.2f}")
            start = time.time()
            n_cars = simulation.get_n_cars_in_simulation()
            print(f"Number of cars in simulation: {n_cars}")
            print(i / steps_per_day * 24)
        simulation.step_forward()
    # simulation = create_simulation()
    arrival_stats = simulation.get_travel_stats()
    # save arrival stats to file
    with open("arrival_stats.txt", "w") as f:
        for arrival_stat in arrival_stats:
            f.write(f"{arrival_stat[0]} {arrival_stat[1]} {arrival_stat[2]} {arrival_stat[3]}\n")
    filtered_arrival_stats = [arrival_stat for arrival_stat in arrival_stats if arrival_stat[0] == 0 and arrival_stat[1] == 1]

#   STABILIZATION PROOF:
#   for i in range(len(cars_per_edge[0])):
#       plt.plot(np.linspace(0, 23, len(cars_per_edge)), [cars_per_edge[j][i] for j in range(len(cars_per_edge))])
#   plt.xlabel("Hour of the day")
#   plt.ylabel("Amount of cars on road")
#   plt.show()

#   EDGE CHARACTERISTICS:
    plt.bar(range(len(cars_per_edge[0])), [sum([cars_per_edge[i][j] for i in range(int(7/24*steps_per_day)//100, len(cars_per_edge))])/(len(cars_per_edge)-int(7/24*steps_per_day)//100)/simulation.get_edge_road_length(j) for j in range(len(cars_per_edge[0]))])
    #plt.tick_params(axis='both', which='minor', labelsize=6)
    plt.xticks(range(len(cars_per_edge[0])), [simulation.get_label_from_node_id(simulation.get_edge_start_node_id(j)) + " - " + simulation.get_label_from_node_id(simulation.get_edge_end_node_id(j)) for j in range(len(cars_per_edge[0]))], rotation='vertical')
    plt.ylabel("Amount of cars per meter by road")
    plt.show()

#   plot_arrival_stats_hist(arrival_stats, 0, 1)
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
