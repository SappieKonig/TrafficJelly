from abc import ABCMeta, abstractmethod


class Simulation(metaclass=ABCMeta):
    @abstractmethod
    def get_car(self, car_id: int): pass

    @abstractmethod
    def get_graphics_cars_in_interval(self, interval: tuple[float, float]): pass

    @abstractmethod
    def get_road_length(self) -> float: pass

    @abstractmethod
    def get_lane_count(self) -> int: pass

    @abstractmethod
    def get_delta_time(self) -> float: pass

    @abstractmethod
    def step_forward(self): pass
