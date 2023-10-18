import itertools
from collections import defaultdict
from dataclasses import dataclass
from typing import TypeVar, TYPE_CHECKING

import pygame

from graphviz.simulation import Simulation
from graphviz.view.game_view import GameView

if TYPE_CHECKING:
    from graphviz.game import Game

ValueType = TypeVar('ValueType')


@dataclass(kw_only=True)
class VizNode:
    id: int
    pos: pygame.Vector2
    size: float


@dataclass(kw_only=True)
class VizEdge:
    id: int
    pos0: pygame.Vector2
    pos1: pygame.Vector2
    offset: pygame.Vector2


class GameGraphView(GameView):

    FPS = 60

    def __init__(self, game: 'Game'):
        self.game = game
        self.screen = pygame.display.set_mode((1280, 720))
        self.screen = game.screen
        self.simulation = game.simulation

        self.nodes = dict[int, VizNode]()
        self.edges = dict[int, VizEdge]()

        self.node_radius_base = 5
        self.node_radius_inc = 7
        self.edge_width_base = 10
        self.edge_offset_base = self.node_radius_inc * 2

        self.pixels_per_density_interval = 5
        self.density_interval_pixel_padding = 5

        self.insert_viz_graph()

        self.dt = 0

        self.scale = .05

    def insert_viz_graph(self):
        sim = self.simulation

        edge_keys_per_node = defaultdict(set)
        edges_count_per_edge = defaultdict(int)
        for edge_id in sim.get_edge_ids():
            node0_id = sim.get_edge_start_node_id(edge_id)
            node1_id = sim.get_edge_end_node_id(edge_id)
            edge_key = frozenset((node0_id, node1_id))
            edge_keys_per_node[node0_id].add(edge_key)
            edge_keys_per_node[node1_id].add(edge_key)
            edges_count_per_edge[edge_key] += 1

        for node_id in sim.get_node_ids():
            x, y = sim.get_node_pos(node_id)
            edge_counts = (edges_count_per_edge[edge_key] for edge_key in edge_keys_per_node[node_id])
            size = max(itertools.chain((0,), edge_counts))
            self.nodes[node_id] = VizNode(id=node_id, pos=pygame.Vector2(x, y), size=size)

        inserted_edges_count_per_node_id_pair = defaultdict(int)
        for edge_id in sim.get_edge_ids():
            node0_id = sim.get_edge_start_node_id(edge_id)
            node1_id = sim.get_edge_end_node_id(edge_id)
            x0, y0 = sim.get_node_pos(node0_id)
            x1, y1 = sim.get_node_pos(node1_id)
            dx = x1 - x0
            dy = y1 - y0
            dist = (dx ** 2 + dy ** 2) ** .5

            edge_key = frozenset((node0_id, node1_id))
            edges_count = edges_count_per_edge[edge_key]
            inserted_edges_count = inserted_edges_count_per_node_id_pair[edge_key]
            sign = 1 if node1_id > node0_id else -1
            offset_dist = (inserted_edges_count - (edges_count - 1) / 2) * self.edge_offset_base * sign
            offset_x = dy / dist * offset_dist
            offset_y = -dx / dist * offset_dist

            self.edges[edge_id] = VizEdge(id=edge_id,
                                          pos0=pygame.Vector2(x0, y0),
                                          pos1=pygame.Vector2(x1, y1),
                                          offset=pygame.Vector2(offset_x, offset_y))
            inserted_edges_count_per_node_id_pair[edge_key] += 1

    def on_event(self, event: pygame.event.Event):
        pass

    def update_viewport(self):
        pass

    def update_state(self):
        while self.game.state.alpha >= 1:
            self.simulation.step_forward()
            self.game.state.alpha -= 1

    def render(self):
        self.screen.fill('gray')
        for edge in self.edges.values():
            self.draw_edge(edge)
        for node in self.nodes.values():
            self.draw_node(node)
        self.draw_info()

    def draw_edge(self, edge: VizEdge):
        sim = self.simulation
        offset = self.get_offset()
        real_offset = offset + edge.offset

        dist_in_meters = sim.get_edge_road_length(edge.id)
        dist_in_pixels = (edge.pos0 - edge.pos1).length() * self.scale
        meter_per_pixel = dist_in_meters / dist_in_pixels
        bin_distance = meter_per_pixel * self.pixels_per_density_interval

        hist, edges = sim.get_car_count_histogram_in_edge(edge.id, bin_distance)

        pygame.draw.line(self.screen, 'black',
                         real_offset + self.scale_and_round(edge.pos0),
                         real_offset + self.scale_and_round(edge.pos1),
                         width=self.edge_width_base)
        pygame.draw.line(self.screen, 'white',
                         real_offset + self.scale_and_round(edge.pos0),
                         real_offset + self.scale_and_round(edge.pos1),
                         width=self.edge_width_base - 4)

        density_interval_dist_padding = self.density_interval_pixel_padding * meter_per_pixel
        for i, count in enumerate(hist):
            if count == 0:
                continue
                # color = 'white'
            elif count == 1:
                color = 'green'
            else:
                color = 'orange'

            dist_start = edges[i]
            dist_end = edges[i+1]
            if count != 0:
                dist_start -= density_interval_dist_padding
                dist_end += density_interval_dist_padding
            dist_start = max(0, dist_start)
            dist_end = min(dist_end, dist_in_meters)

            pos_start = edge.pos0 + (dist_start / dist_in_meters) * (edge.pos1 - edge.pos0)
            pos_end = edge.pos0 + (dist_end / dist_in_meters) * (edge.pos1 - edge.pos0)
            pygame.draw.line(self.screen, color,
                             real_offset + self.scale_and_round(pos_start),
                             real_offset + self.scale_and_round(pos_end),
                             width=self.edge_width_base - 4)

    def draw_node(self, node: VizNode):
        offset = self.get_offset()
        radius = self.node_radius_base + self.node_radius_inc * node.size
        center = offset + self.scale_and_round(node.pos)

        car_count = self.simulation.get_car_count_in_node(node.id)
        if car_count == 0:  # Will probably be done using HSL later, when the model has been integrated.
            color = 'white'
        elif car_count <= 20:
            color = 'green'
        elif car_count <= 40:
            color = 'orange'
        else:
            color = 'red'

        pygame.draw.circle(self.screen, color, center, radius)
        pygame.draw.circle(self.screen, 'black', center, radius, 2)

    def draw_info(self):
        text_surface = self.game.font.render(f'Time scale: {self.game.state.time_scale}', True, (0, 0, 0))
        text_surface.set_alpha(192)
        text_rect = text_surface.get_rect()
        text_rect.bottomleft = self.screen.get_rect().bottomleft
        text_rect.move_ip(10, -10)
        self.screen.blit(text_surface, text_rect)

    def scale_and_round(self, value: ValueType) -> ValueType:
        return round(value * self.scale)

    def scale_and_ceil(self, value: ValueType) -> ValueType:
        return round(value * self.scale + 0.5)

    def get_screen_center(self) -> pygame.Vector2:
        return pygame.Vector2(self.screen.get_rect().center)

    def get_offset(self):
        return self.get_screen_center()
