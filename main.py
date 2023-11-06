import os
os.system("./build.sh")

from traffic_model import TrafficModel

from graphviz.game import Game
from graphviz.simulation import Simulation
from graphviz.view.game_graph_view import GameGraphView


DELTA_TIME = 0.5
SCALE = 1


def main():
    simulation = TrafficModel("graph.txt", DELTA_TIME, SCALE)
    steps_per_day = int(3600 * 24 / DELTA_TIME * SCALE)
    until = int(7 / 24 * steps_per_day)
    for i in range(until):
        simulation.step_forward()
    game = Game(simulation=simulation)
    game.push_view(GameGraphView(game=game))
    game.main()
  

if __name__ == '__main__':
    main()
