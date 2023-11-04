from __future__ import annotations
from traffic_model import TrafficModel
import time

from graphviz.game import Game
from graphviz.simulation import Simulation
from graphviz.view.game_graph_view import GameGraphView
from graphviz.view.game_road_view import GameRoadView


DELTA_TIME = 0.2


def main():
    simulation = TrafficModel("graph.txt", DELTA_TIME)
    start = time.time()
    for i in range(30000):
        if i % 100 == 0:
            time_taken = time.time() - start
            print(f"Time taken for 100 steps: {time_taken:.2f}")
            start = time.time()
            n_cars = simulation.get_n_cars_in_simulation()
            print(f"Number of cars in simulation: {n_cars}")
            print(i)
        simulation.step_forward()
    # simulation = create_simulation()
    timings = simulation.get_travel_stats()
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
