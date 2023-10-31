import random
from dataclasses import dataclass
from traffic_jelly.data_types import GraphicsCar

from .simulation import Simulation


@dataclass
class SimulationCar:
    id: int

    x: float
    v: float
    a: float

    lane: int


class BasicSimulation(Simulation):
    __dt = 1  # seconds
    __scale = 1
    __road_length = 1000
    __lane_count = 3

    def __init__(self):
        self.__cars = {
            i: SimulationCar(
                id=i,
                x=i * 30, v=50, a=0,
                lane=int(random.random() < 0.4),
            ) for i in range(50)
        }

    def get_delta_time(self):
        return self.__dt

    def step_forward(self):
        car_ids_to_delete = []
        for car_id, car in self.__cars.items():
            car.v += car.a * self.__dt
            car.x += car.v * self.__dt
            if not 0 <= car.x <= self.__road_length:
                car_ids_to_delete.append(car_id)
            if random.random() < 0.1:
                car.lane = 1 - car.lane
        for car_id in car_ids_to_delete:
            del self.__cars[car_id]

    def get_road_length(self):
        return self.__road_length

    def get_lane_count(self):
        return self.__lane_count

    def get_car(self, car_id):
        return self.__cars.get(car_id, None)

    def get_graphics_cars_in_interval(self, interval: tuple[float, float]):
        return {
            car_id: GraphicsCar(id=car_id, x=car.x, y=car.lane)
            for car_id, car in self.__cars.items()
            if interval[0] <= car.x <= interval[1]
        }