import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import norm
import random

from .simulation import Simulation
from traffic_jelly.data_types import GraphicsCar


class BrakeAction:
    def __init__(self, time):
        self.time = time

    def __call__(self, car, time_step_size):
        car.brake(time_step_size)
        self.time -= time_step_size


class AccelerateAction:
    def __init__(self, time):
        self.time = time

    def __call__(self, car, time_step_size):
        car.accelerate(time_step_size)
        self.time -= time_step_size


class CruiseAction:
    def __init__(self, time):
        self.time = time

    def __call__(self, car, time_step_size):
        car.renew_speed(time_step_size)
        self.time -= time_step_size


class Car:
    def __init__(self):
        # every car has a personal target speed
        self.target = np.random.normal(30, 3)  # m/s
        self.target = min(max(self.target, 25), 35)
        self.speed = 0  # m/s
        self.age = 0  # seconds
        self.distance = 0  # km
        self.lane = 0
        self.action = None

    def step(self, time_step_size):
        self.age += time_step_size
        self.distance += self.speed * time_step_size
        if self.action is not None:
            self.action(self, time_step_size)
            if self.action.time <= 0:
                self.action = None

    def brake(self, time_step_size):
        self.speed -= 10 * time_step_size
        if self.speed < 0:
            self.speed = 0

    def accelerate(self, time_step_size):
        self.speed += 2 * time_step_size

    def renew_speed(self, time_step_size):
        if np.random.normal(self.get_target(), 3) > self.speed:
            self.accelerate(time_step_size)
        else:
            self.brake(time_step_size)

    def get_target(self):
        return self.target + 5 * self.lane

    def as_graphics_car(self):
        return GraphicsCar(
            id=id(self),
            x=self.distance,
            y=self.lane,
        )

    def get_margin(self):
        return 20 + 35 * self.speed / 100


class ComplexSimulation(Simulation):
    def __init__(self, length, spawn_prob_per_sec=0.1):
        self.length = length  # meters
        self.vehicles: list[Car] = []
        self.time_taken = []
        self.time_step_size = .2  # seconds
        self.spawn_prob = spawn_prob_per_sec * self.time_step_size
        self.lane_count = 2

    def step_forward(self):
        switch_lane_action = [False for _ in range(len(self.vehicles))]
        for i, car in enumerate(self.vehicles):
            if car.action is not None:
                continue
            backward_margin = 100
            cars_in_front = []
            j = i - 1  # vehicles are ordered largest distance to smallest
            while j >= 0 and self.vehicles[j].distance - car.distance < car.get_margin():
                cars_in_front.append(self.vehicles[j])
                j -= 1
            front_occupied = set(other_car.lane for other_car in cars_in_front)

            cars_in_front_far = []
            j = i - 1
            while j >= 0 and self.vehicles[j].distance - car.distance < car.get_margin() * 3:
                cars_in_front_far.append(self.vehicles[j])
                j -= 1
            front_snails = set(other_car.lane for other_car in cars_in_front_far if
                               (car.speed - other_car.speed > 10 or other_car.speed == 0))

            cars_in_back = []
            j = i + 1
            while j < len(self.vehicles) and car.distance - self.vehicles[j].distance < backward_margin:
                pseudo_margin = self.vehicles[j].get_margin()
                if car.distance - self.vehicles[j].distance < pseudo_margin:
                    cars_in_back.append(self.vehicles[j])
                j += 1
            back_occupied = set(other_car.lane for other_car in cars_in_back)

            if car.lane == 0:
                if car.lane not in front_occupied and car.lane not in front_snails:
                    car.action = CruiseAction(.2)
                elif 1 not in front_occupied and 1 not in back_occupied and 1 not in front_snails:
                    switch_lane_action[i] = True
                    car.action = AccelerateAction(.2)
                else:
                    car.action = BrakeAction(.5)

            elif car.lane == 1:
                if 0 not in front_occupied and 0 not in back_occupied and 0 not in front_snails:
                    switch_lane_action[i] = True
                    car.action = BrakeAction(.5)
                elif 1 not in front_occupied and 1 not in front_snails:
                    car.action = CruiseAction(.2)
                else:
                    car.action = BrakeAction(.5)

        for i in range(len(self.vehicles)):
            if switch_lane_action[i]:
                self.vehicles[i].lane = 1 - self.vehicles[i].lane

            self.vehicles[i].step(self.time_step_size)

            if self.vehicles[i].distance >= self.length:
                self.time_taken.append(self.vehicles[i].age)

        if random.random() < self.spawn_prob and (len(self.vehicles) == 0 or self.vehicles[-1].distance > 20):
            new_car = Car()
            if len(self.vehicles) != 0 and self.vehicles[-1].distance < 100:
                new_car.speed = min(self.vehicles[-1].speed, new_car.speed)
            self.vehicles.append(new_car)

        self.vehicles = [car for car in self.vehicles if car.distance < self.length]

        changed = True
        while changed:
            changed = False
            for i in range(len(self.vehicles) - 1):
                if self.vehicles[i].distance < self.vehicles[i + 1].distance:
                    self.vehicles[i], self.vehicles[i + 1] = self.vehicles[i + 1], self.vehicles[i]
                    changed = True
                    if self.vehicles[i].lane == self.vehicles[i + 1].lane:
                        print("ERROR, CRASH")
                        print(f"At {self.vehicles[i].distance} km")
                        print(f"Car 1: {self.vehicles[i].lane}")
                        print(f"Car 2: {self.vehicles[i + 1].lane}")
                        print(f"Car 1 speed: {self.vehicles[i].speed}")
                        print(f"Car 2 speed: {self.vehicles[i + 1].speed}")

    def get_average_time(self):
        return np.mean(self.time_taken)

    def sort_cars(self):
        self.vehicles.sort(key=lambda x: x.distance)

    def create_histogram(self, n_bins=30):
        minutes_taken = np.array(self.time_taken) / 60  # Convert to minutes
        bin_width = (minutes_taken.max() - minutes_taken.min()) / n_bins
        hist_surface = bin_width * len(minutes_taken)
        mean = minutes_taken.mean()
        std = minutes_taken.std()
        x = np.linspace(minutes_taken.min(), minutes_taken.max(), 1000)
        norm_dist = norm.pdf(x, mean, std) * hist_surface
        plt.plot(x, norm_dist, color="red")
        plt.hist(minutes_taken, bins=n_bins)
        plt.xlabel("Time taken (minutes)")
        plt.ylabel("Frequency")
        plt.show()

    def get_random_car(self):
        car_idx = np.random.randint(len(self.vehicles))
        return self.vehicles[car_idx]

    def get_graphics_cars_in_interval(self, interval: tuple[float, float]):
        cars = [car for car in self.vehicles if interval[0] <= car.distance <= interval[1]]
        graphics_cars = {id(car): car.as_graphics_car() for car in cars}
        return graphics_cars

    def get_lane_count(self):
        return self.lane_count

    def get_delta_time(self):
        return self.time_step_size

    def get_road_length(self):
        return self.length

    def get_car(self, car_id: int):
        for car in self.vehicles:
            if id(car) == car_id:
                return car

    def cause_breakdown(self, distance: float):
        for car in reversed(self.vehicles):
            if car.distance > distance:
                car.action = BrakeAction(100000000)
                break
