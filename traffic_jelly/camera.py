from abc import ABCMeta, abstractmethod

from traffic_jelly.simulation import Simulation


class Camera(metaclass=ABCMeta):
    @abstractmethod
    def get_x_center(self, state: 'GameState') -> float: pass


class StationaryCamera(Camera):
    def __init__(self, x_center):
        self.__x_center = x_center

    def get_x_center(self, state: 'GameState') -> float:
        return self.__x_center


class CarCamera(Camera):
    def __init__(self, simulation: Simulation, car_id):
        self.__car_id = car_id
        self.__simulation = simulation
        self.__time_step = -1
        self.__prev_x = simulation.get_car(car_id).x
        self.__next_x = self.__prev_x
        self.__last_x_center = 0

    def get_x_center(self, state: 'GameState') -> float:
        if not self.__simulation.get_car(self.__car_id):
            return self.__last_x_center
        if self.__time_step < state.time_step:
            self.__time_step = state.time_step
            self.__prev_x = self.__next_x
            self.__next_x = self.__simulation.get_car(self.__car_id).x
        alpha = state.time % state.delta_time / state.delta_time
        beta = 1 - alpha
        self.__last_x_center = alpha * self.__next_x + beta * self.__prev_x
        return self.__last_x_center