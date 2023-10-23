from __future__ import annotations
from traffic_model import TrafficModel

from graphviz.game import Game
from graphviz.simulation import Simulation
# noinspection PyUnresolvedReferences
from graphviz.view.game_graph_view import GameGraphView
# noinspection PyUnresolvedReferences
from graphviz.view.game_road_view import GameRoadView


DELTA_TIME = 0.2


def main():
    simulation = TrafficModel("graph.txt", DELTA_TIME)
    # simulation = create_simulation()
    game = Game(simulation=simulation, delta_time=DELTA_TIME)
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
