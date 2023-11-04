import itertools
from collections import defaultdict
from dataclasses import dataclass
from typing import TypeVar, TYPE_CHECKING

import numpy as np
import pygame

from graphviz.view.game_road_view import GameRoadView
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


@dataclass(kw_only=True)
class GraphStyle:
    node_radius_base: int
    node_radius_inc: int
    node_border_width: int
    edge_width_base: int
    edge_offset_base: int
    edge_inner_width: int


DEFAULT_STYLE = GraphStyle(
    node_radius_base=5,
    node_radius_inc=7,
    node_border_width=1,
    edge_width_base=10,
    edge_inner_width=10-2,
    edge_offset_base=7*2,
)


COMPACT_STYLE = GraphStyle(
    node_radius_base=3,
    node_radius_inc=3,
    node_border_width=1,
    edge_width_base=4,
    edge_inner_width=4-2,
    edge_offset_base=3*2,
)


class GameGraphView(GameView):

    FPS = 60

    def __init__(self, game: 'Game'):
        self.game = game
        self.screen = pygame.display.set_mode((1280, 720))
        self.screen = game.screen
        self.simulation = game.simulation

        self.nodes = dict[int, VizNode]()
        self.edges = dict[int, VizEdge]()

        self.style = DEFAULT_STYLE

        self.pixels_per_density_interval = 5
        self.density_interval_pixel_padding = 5

        self.load_viz_graph()

        self.dt = 0

        self.marginal_buffer = 100  # m
        self.scale_rate_base = 2  # multiplier per second
        self.scale_rate_multiplier_base = 2
        self.pan_rate_base = 100  # pixels
        self.pan_rate_multiplier_base = 4

        self.scale_rate = 1
        self.scale_rate_multiplier = 1
        self.pan_rate_x = 0
        self.pan_rate_y = 0
        self.pan_rate_multiplier = 1

        self.scale = .05
        self.pan_offset_x = 0
        self.pan_offset_y = 0

    def load_viz_graph(self):
        self.nodes.clear()
        self.edges.clear()

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
            offset_dist = (inserted_edges_count - (edges_count - 1) / 2) * self.style.edge_offset_base * sign
            offset_x = dy / dist * offset_dist
            offset_y = -dx / dist * offset_dist

            self.edges[edge_id] = VizEdge(id=edge_id,
                                          pos0=pygame.Vector2(x0, y0),
                                          pos1=pygame.Vector2(x1, y1),
                                          offset=pygame.Vector2(offset_x, offset_y))
            inserted_edges_count_per_node_id_pair[edge_key] += 1

    def on_event(self, event: pygame.event.Event):
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LSHIFT or event.key == pygame.K_RSHIFT:
                self.pan_rate_multiplier = self.pan_rate_multiplier_base
                self.scale_rate_multiplier = self.scale_rate_multiplier_base
            if event.key == pygame.K_EQUALS or event.key == pygame.K_PLUS:
                self.scale_rate = self.scale_rate_base
            if event.key == pygame.K_MINUS or event.key == pygame.K_UNDERSCORE:
                self.scale_rate = 1 / self.scale_rate_base
            if event.key == pygame.K_LEFT:
                self.pan_rate_x = self.pan_rate_base
            if event.key == pygame.K_RIGHT:
                self.pan_rate_x = -self.pan_rate_base
            if event.key == pygame.K_UP:
                self.pan_rate_y = self.pan_rate_base
            if event.key == pygame.K_DOWN:
                self.pan_rate_y = -self.pan_rate_base
            if event.key == pygame.K_k:
                self.style = COMPACT_STYLE if self.style == DEFAULT_STYLE else DEFAULT_STYLE
                self.load_viz_graph()
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_LSHIFT or event.key == pygame.K_RSHIFT:
                self.pan_rate_multiplier = 1
                self.scale_rate_multiplier = 1
            if (event.key == pygame.K_EQUALS or event.key == pygame.K_PLUS or
                    event.key == pygame.K_MINUS or event.key == pygame.K_UNDERSCORE):
                self.scale_rate = 1
            if event.key == pygame.K_LEFT or event.key == pygame.K_RIGHT:
                self.pan_rate_x = 0
            if event.key == pygame.K_UP or event.key == pygame.K_DOWN:
                self.pan_rate_y = 0
        if event.type == pygame.MOUSEBUTTONDOWN:
            self.on_mouse_down(event)

    def on_mouse_down(self, event):
        offset = self.get_offset()
        mouse_pos = event.pos
        sim_position = (mouse_pos - offset) / self.scale
        for node in self.nodes.values():
            if (node.pos - sim_position).length() * self.scale <= self.get_node_radius(node):
                # DEBUG: You can add some debug print statements here that extract information from the model
                # If the user clicks on a node, there is no need to check whether the user clicked on an edge
                # As a result. We will return.
                return
        for edge_id, edge in self.edges.items():
            distance, ratio = self.get_distance_in_pixels_and_start_to_end_ratio_of_edge(edge_id, mouse_pos)
            edge_is_clicked = self.distance_is_within_edge_width(distance) and 0 <= ratio <= 1
            if edge_is_clicked:
                # DEBUG: You can add some debug print statements here that extract information from the model
                pan_offset_x = ratio * -(edge.pos1 - edge.pos0).length()
                self.game.push_view(GameRoadView(game=self.game, edge_id=edge_id, pan_offset_x=pan_offset_x))

    def distance_is_within_edge_width(self, distance):
        return distance < self.style.edge_width_base / 2

    def get_distance_in_pixels_and_start_to_end_ratio_of_edge(self, edge_id: int, pos: pygame.Vector2) -> tuple[float, float]:
        edge = self.edges[edge_id]
        offset = self.get_offset() + edge.offset
        p0_to_p1 = (edge.pos1 - edge.pos0) * self.scale
        p0_to_pos = pos - (edge.pos0 * self.scale + offset)
        # noinspection PyUnreachableCode

        # distance in pixels between the edge and pos if edge were to be infinitely extended
        distance_in_pixels = np.linalg.norm(np.cross(p0_to_p1, p0_to_pos)) / np.linalg.norm(p0_to_p1)

        # start_to_end_ratio is equal to 0 when clicked on the start, and equal to 1 when clicked on the end of the edge
        # If the position when projected onto the edge is outside the edge, then it is of course less than 0 if the
        # projection is before the start of the edge and greater than 1 in the projection is after the end of the edge.
        #
        # -1    0    1    2
        #       |----|
        start_to_end_ratio = np.dot(p0_to_p1, p0_to_pos) / np.dot(p0_to_p1, p0_to_p1)

        return distance_in_pixels, start_to_end_ratio

    def update_viewport(self):
        dt = self.game.dt
        if dt != 0:
            scale_factor = self.scale_rate ** (dt * self.scale_rate_multiplier)
            self.scale *= scale_factor
        self.pan_offset_x += dt * self.pan_rate_x * self.pan_rate_multiplier / self.scale
        self.pan_offset_y += dt * self.pan_rate_y * self.pan_rate_multiplier / self.scale

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
        offset_in_pixels = self.get_offset() + edge.offset
        dist_in_meters = sim.get_edge_road_length(edge.id)
        dist_in_pixels = (edge.pos0 - edge.pos1).length() * self.scale
        meter_per_pixel = dist_in_meters / dist_in_pixels
        bin_distance = meter_per_pixel * self.pixels_per_density_interval

        hist, edges = sim.get_car_count_histogram_in_edge(edge.id, bin_distance)

        pygame.draw.line(self.screen, 'black',
                         offset_in_pixels + self.scale_and_round(edge.pos0),
                         offset_in_pixels + self.scale_and_round(edge.pos1),
                         width=self.style.edge_width_base)
        pygame.draw.line(self.screen, 'white',
                         offset_in_pixels + self.scale_and_round(edge.pos0),
                         offset_in_pixels + self.scale_and_round(edge.pos1),
                         width=self.style.edge_inner_width)

        density_interval_dist_padding = self.density_interval_pixel_padding * meter_per_pixel

        sorted_colors = ['green', 'orange']
        indices_per_color = defaultdict(list)
        for i, count in enumerate(hist):
            if count == 1:
                indices_per_color['green'].append(i)
            elif count > 1:
                indices_per_color['orange'].append(i)

        for color in sorted_colors:
            for i in indices_per_color[color]:
                dist_start = max(0, edges[i] - density_interval_dist_padding)
                dist_end = min(edges[i+1] + density_interval_dist_padding, dist_in_meters)

                pos_start = edge.pos0 + (dist_start / dist_in_meters) * (edge.pos1 - edge.pos0)
                pos_end = edge.pos0 + (dist_end / dist_in_meters) * (edge.pos1 - edge.pos0)
                pygame.draw.line(self.screen, color,
                                 offset_in_pixels + self.scale_and_round(pos_start),
                                 offset_in_pixels + self.scale_and_round(pos_end),
                                 width=self.style.edge_inner_width)

    def draw_node(self, node: VizNode):
        offset = self.get_offset()
        radius = self.get_node_radius(node)
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
        pygame.draw.circle(self.screen, 'black', center, radius, self.style.node_border_width)

    def get_node_radius(self, node):
        return self.style.node_radius_base + self.style.node_radius_inc * node.size

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
        return self.get_screen_center() + pygame.Vector2(self.pan_offset_x, self.pan_offset_y) * self.scale
