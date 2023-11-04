import math
from typing import TYPE_CHECKING

import pygame

from graphviz.simulation import Simulation
from graphviz.state.game_road_state import GameRoadState, VizCar
from graphviz.util.color import get_random_color
from graphviz.view.game_view import GameView

if TYPE_CHECKING:
    from graphviz.game import Game


class GameRoadView(GameView):
    def __init__(self, *,
                 game: 'Game', edge_id: int,
                 pan_offset_x: float = 0):
        self.game = game
        self.edge_id = edge_id
        self.screen = game.screen
        self.simulation = game.simulation

        self.clock = pygame.time.Clock()
        self.display = pygame.display
        self.color_mapping = dict()

        self.max_speed = 300
        self.lane_width = 3.5
        self.car_length = 4.5
        self.car_width = self.lane_width
        # self.car_width = 2  # True car width

        self.lane_count = game.simulation.get_edge_lane_count(edge_id)
        self.road_length = game.simulation.get_edge_road_length(edge_id)

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

        self.state = GameRoadState(
            scale=1,
            pan_offset_x=pan_offset_x,
            pan_offset_y=0,
            prev_cars={},
            next_cars={},
        )

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

    def update_viewport(self):
        dt = self.game.dt
        if dt != 0:
            scale_factor = self.scale_rate ** (dt * self.scale_rate_multiplier)
            self.state.scale *= scale_factor
        self.state.pan_offset_x += dt * self.pan_rate_x * self.pan_rate_multiplier / self.state.scale
        self.state.pan_offset_y += dt * self.pan_rate_y * self.pan_rate_multiplier / self.state.scale

    def update_state(self):
        while self.game.state.alpha >= 2:
            self.simulation.step_forward()
            self.game.state.alpha -= 1
        if self.game.state.alpha >= 1:
            interval = self.get_displayed_cars_interval()
            self.state.prev_cars = self.get_graphics_cars(interval)
            self.simulation.step_forward()
            self.game.state.alpha -= 1
            self.state.next_cars = self.get_graphics_cars(interval)

    def render(self):
        self.screen.fill('darkgreen')
        self.draw_road()
        self.draw_cars()
        self.draw_info()

    def draw_road(self):
        offset = self.get_offset()
        road_rect = pygame.rect.Rect((
            0, 0,
            self.road_length * self.state.scale,
            max((1, self.lane_count * self.lane_width * self.state.scale)),
        ))
        road_rect.move_ip(offset)

        # most accurate color is gray30, but we'll use black instead
        pygame.draw.rect(self.screen, 'black', road_rect)

    def draw_cars(self):
        offset = self.get_offset()

        scale = self.state.scale
        # alpha = 0 -> interpolation is in next_cars
        # alpha = 1 -> interpolation is in prev_cars
        alpha = self.game.state.alpha
        beta = 1 - alpha

        cars = self.state.next_cars
        prev_cars = self.state.prev_cars

        for car_id in set(prev_cars.keys()) & set(cars.keys()):
            if car_id not in self.color_mapping:
                self.color_mapping[car_id] = get_random_color()

            prev_x = self.state.prev_cars[car_id].x
            next_x = self.state.next_cars[car_id].x
            x = alpha * next_x + beta * prev_x

            prev_y = self.state.prev_cars[car_id].y
            next_y = self.state.next_cars[car_id].y
            y = alpha * next_y + beta * prev_y

            car_rect = pygame.rect.Rect(
                (x - self.car_width * 0.5) * scale,
                (self.lane_width * (self.lane_count - y - 0.5) - self.car_width * 0.5) * scale,
                self.car_length * scale,
                self.car_width * scale
            )
            car_rect.move_ip(offset)

            pygame.draw.rect(self.screen, self.color_mapping[car_id], car_rect)

    def draw_info(self):
        text_surface = self.game.font.render(f'Time scale: {self.game.state.time_scale}', True, (255, 255, 255))
        text_surface.set_alpha(192)
        text_rect = text_surface.get_rect()
        text_rect.bottomleft = self.screen.get_rect().bottomleft
        text_rect.move_ip(10, -10)
        self.screen.blit(text_surface, text_rect)

    def get_graphics_cars(self, interval: tuple[float, float]) -> dict[int, VizCar]:
        cars = dict()
        for car_id in self.simulation.get_car_ids_in_edge_on_interval(self.edge_id, interval):
            cars[car_id] = self.get_graphics_car_from_id(car_id)
        return cars

    def get_graphics_car_from_id(self, car_id: int):
        return VizCar(id=car_id, x=self.simulation.get_car_x(car_id), y=self.simulation.get_car_lane(car_id))

    def get_displayed_cars_interval(self):
        screen_rect = self.screen.get_rect()
        max_speed_buffer = self.game.dt * self.max_speed
        center = -self.get_offset().x / self.state.scale + self.get_screen_center().x / self.state.scale
        radius = (screen_rect.width * 0.5 + max_speed_buffer) / self.state.scale + self.marginal_buffer
        return center - radius, center + radius

    def get_offset(self):
        return self.get_screen_center() + pygame.Vector2(
            self.state.pan_offset_x,
            self.state.pan_offset_y,
        ) * self.state.scale

    def get_screen_center(self) -> pygame.Vector2:
        return pygame.Vector2(self.screen.get_rect().center)
