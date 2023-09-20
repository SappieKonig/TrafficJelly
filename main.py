import colorsys
import random
from abc import ABCMeta, abstractmethod
from dataclasses import dataclass

import pygame


@dataclass(kw_only=True)
class SimulationCar:
    id: int

    x: float
    v: float
    a: float

    lane: int

    color: tuple[int, int, int]


def get_random_color() -> tuple[int, int, int]:
    r, g, b = colorsys.hsv_to_rgb(random.random(), 1, 1)
    return int(r * 255), int(g * 255), int(b * 255)


class Simulation:
    __dt = 1  # seconds
    __scale = 1
    __road_length = 1000
    __lane_count = 2

    def __init__(self):
        self.__cars = {
            i: SimulationCar(
                id=i,
                x=i * 30, v=50, a=0,
                lane=0,
                color=get_random_color()
            ) for i in range(50)
        }

    def get_scale(self):
        return self.__scale

    def get_delta_time(self):
        return self.__dt

    def step_forward(self):
        car_ids_to_delete = []
        for car_id, car in self.__cars.items():
            car.v += car.a * self.__dt
            car.x += car.v * self.__dt
            if not 0 <= car.x <= self.__road_length:
                car_ids_to_delete.append(car_id)
        for car_id in car_ids_to_delete:
            del self.__cars[car_id]

    def get_road_length(self):
        return self.__road_length

    def get_lane_count(self):
        return self.__lane_count

    def get_car(self, car_id):
        return self.__cars.get(car_id, None)

    def get_car_count_in_interval(self, interval: tuple[float, float]):
        pass

    def get_cars_in_interval(self, interval: tuple[float, float]):
        return {
            car_id: car
            for car_id, car in self.__cars.items()
            if interval[0] <= car.x <= interval[1]
        }


@dataclass(kw_only=True)
class GraphicsCar:
    id: int
    x: float
    y: float
    color: tuple[int, int, int]


class Camera(metaclass=ABCMeta):
    @abstractmethod
    def get_x_center(self) -> float: pass


class StationaryCamera(Camera):
    def __init__(self, x_center):
        self.__x_center = x_center

    def get_x_center(self) -> float:
        return self.__x_center


class CarCamera(Camera):
    def __init__(self, simulation: Simulation, car_id):
        self.__simulation = simulation

    def get_x_center(self) -> float:
        return self.simulation


MAX_SPEED = 300  # m/s

LANE_WIDTH = 3.5

CAR_LENGTH = 4.5  # m
CAR_WIDTH = 2  # m
# CAR_WIDTH = LANE_WIDTH  # m


@dataclass(kw_only=True)
class GameState:
    time: float
    time_step: int
    scale: float
    pan_offset_x: float
    pan_offset_y: float
    prev_cars: dict[int, GraphicsCar]
    next_cars: dict[int, GraphicsCar]


class Game:
    FPS = 60

    def __init__(self, *, simulation: Simulation, camera: Camera):
        pygame.init()
        self.screen = pygame.display.set_mode((1280, 720))
        self.clock = pygame.time.Clock()
        self.display = pygame.display
        self.simulation = simulation
        self.camera = camera

        self.simulation_dt = simulation.get_delta_time()
        # self.simulation_scale = simulation.get_scale()
        self.lanes = simulation.get_lane_count()

        self.marginal_buffer = 100  # m
        self.scale_rate_base = 2  # multiplier per second
        self.scale_rate_multiplier_base = 2
        self.pan_rate_base = 100  # pixels
        self.pan_rate_multiplier_base = 4

        self.dt = 0
        self.scale_rate = 1
        self.scale_rate_multiplier = 1
        self.pan_rate_x = 0
        self.pan_rate_y = 0
        self.pan_rate_multiplier = 1
        self.state = GameState(
            time=0,
            time_step=-1,
            scale=1,
            pan_offset_x=0,
            pan_offset_y=0,
            prev_cars={},
            next_cars={},
        )

    def main(self):
        self.loop()
        pygame.quit()

    def loop(self):
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                self.on_event(event)
            self.step()
            self.display.flip()
            self.dt = self.clock.tick(self.FPS) / 1000
            self.state.time += self.dt

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

    def step(self):
        self.update_viewport()
        self.update_state()

        self.screen.fill('white')
        self.draw_road()
        self.draw_cars()

    def update_viewport(self):
        if self.dt != 0:
            self.state.scale *= self.scale_rate ** (self.dt * self.scale_rate_multiplier)
        self.state.pan_offset_x += self.dt * self.pan_rate_x * self.pan_rate_multiplier
        self.state.pan_offset_y += self.dt * self.pan_rate_y * self.pan_rate_multiplier

    def update_state(self):
        current_time_step = self.state.time // self.simulation_dt
        while current_time_step - 1 > self.state.time_step:
            self.simulation.step_forward()
            self.state.time_step += 1
        if current_time_step > self.state.time_step:
            interval = self.get_displayed_cars_interval()
            self.state.prev_cars = self.get_graphics_cars(interval)
            self.simulation.step_forward()
            self.state.time_step += 1
            self.state.next_cars = self.get_graphics_cars(interval)

    def draw_road(self):
        offset = self.get_offset()
        road_rect = pygame.rect.Rect((
            0, 0,
            self.simulation.get_road_length() * self.state.scale,
            self.simulation.get_lane_count() * LANE_WIDTH * self.state.scale,
        ))
        road_rect.move_ip(offset)

        # most accurate color is gray30, but we'll use black instead
        pygame.draw.rect(self.screen, 'black', road_rect)

    def draw_cars(self):
        offset = self.get_offset()

        scale = self.state.scale
        # alpha = 0 -> interpolation is in next_cars
        # alpha = 1 -> interpolation is in prev_cars
        alpha = self.state.time % self.simulation_dt / self.simulation_dt
        beta = 1 - alpha

        cars = self.state.next_cars
        prev_cars = self.state.prev_cars

        for car_id in set(prev_cars.keys()) & set(cars.keys()):
            car = cars[car_id]

            prev_x = self.state.prev_cars[car_id].x
            next_x = self.state.next_cars[car_id].x
            x = alpha * next_x + beta * prev_x

            prev_y = self.state.prev_cars[car_id].y
            next_y = self.state.next_cars[car_id].y
            y = alpha * next_y + beta * prev_y

            car_rect = pygame.rect.Rect(
                x * scale,
                (LANE_WIDTH * (self.lanes - y - 0.5) - CAR_WIDTH * 0.5) * scale,
                CAR_LENGTH * scale,
                CAR_WIDTH * scale
            )
            car_rect.move_ip(offset)

            pygame.draw.rect(self.screen, car.color, car_rect)

    def get_graphics_cars(self, interval: tuple[float, float]):
        cars = self.simulation.get_cars_in_interval(interval)
        return self.get_graphics_cars_from_simulation_cars(cars)

    def get_graphics_cars_from_simulation_cars(self, cars: dict[int, SimulationCar]):
        return {
            car_id: GraphicsCar(
                id=car.id,
                x=car.x,
                y=car.lane,
                color=car.color
            ) for car_id, car in cars.items()
        }

    def get_displayed_cars_interval(self):
        screen_rect = self.screen.get_rect()
        max_speed_buffer = self.simulation_dt * MAX_SPEED
        origin = -self.state.pan_offset_x
        radius = (screen_rect.width * 0.5 + max_speed_buffer) / self.state.scale + self.marginal_buffer
        return origin - radius, origin + radius

    def get_offset(self):
        screen_rect = self.screen.get_rect()
        return pygame.Vector2(
            screen_rect.width / 2 + self.state.pan_offset_x - self.camera.get_x_center(),
            screen_rect.height / 2 + self.state.pan_offset_y,
        )


if __name__ == '__main__':
    Game(
        simulation=Simulation(),
        camera=StationaryCamera(400)
    ).main()
