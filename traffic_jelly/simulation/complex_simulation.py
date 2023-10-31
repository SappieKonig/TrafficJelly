import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm

from .simulation import Simulation
from traffic_jelly.data_types import GraphicsCar


class Car:
    def __init__(self):
        self.speed = np.random.normal(100, 4) #km/h
        self.age = 0  # seconds
        self.distance = 0  # m
        self.lane = 0
        self.action = None

        self.mode = "independent"

    def step(self, time_step_size):
        self.age += time_step_size
        self.distance += self.speed / 3.6 * time_step_size  # s to h

    def brake_soft(self, time_step_size):
        self.speed -= 5 * time_step_size
        if self.speed < 0:
            self.speed = 0

    def brake_hard(self, time_step_size):
        self.speed -= 20 * time_step_size
        if self.speed < 0:
            self.speed = 0


    def renew_speed(self):
        if self.mode == "independent":
            self.speed = np.random.normal(100, 4)
        elif self.mode == "dependent":
            self.speed = np.random.normal(self.speed, 4)
        elif self.mode == "randomwalk":
            self.speed += 4 * (1 - 2 * np.random.binomial(n=1, p=0.5))
            # times 4 taken for similar variance to the
            # speed dependent case
        if self.speed < 0:
            self.speed = 0


    def as_graphics_car(self):
        return GraphicsCar(
            id=id(self),
            x=self.distance,
            y=self.lane,
        )



class ComplexSimulation(Simulation):
    def __init__(self, length, spawn_prob_per_sec=0.1):
        self.length = length  # m
        self.vehicles: list[Car] = []
        self.time_taken = []
        self.time_step_size = 1  # seconds
        self.spawn_prob = spawn_prob_per_sec * self.time_step_size
        self.lane_count = 2

        self.mode = "double"


    def switch_lane(self):
        switch_lane_action = [False for _ in range(len(self.vehicles))]
        margin = 55
        for i, car in enumerate(self.vehicles):
            cars_in_front = []
            j = i - 1  # vehicles are ordered largest distance to smallest
            while j >= 0 and self.vehicles[j].distance - car.distance < margin:
                cars_in_front.append(self.vehicles[j])
                j -= 1

            cars_in_back = []
            j = i + 1
            while j < len(self.vehicles) and car.distance - self.vehicles[j].distance < margin:
                cars_in_back.append(self.vehicles[j])
                j += 1

            if car.lane == 0:
                if (not any([other_car.lane == 1 for other_car in cars_in_front])
                    and not any([other_car.lane == 1 for other_car in cars_in_back])) \
                        and any(other_car.lane == 0 for other_car in cars_in_front):
                    switch_lane_action[i] = True

            elif car.lane == 1:
                if (not any([other_car.lane == 0 for other_car in cars_in_front])
                        and not any([other_car.lane == 0 for other_car in cars_in_back])):
                    switch_lane_action[i] = True

        # switching them cars
        for i, car in enumerate(self.vehicles):
            if switch_lane_action[i]:
                self.vehicles[i].lane = 1 - self.vehicles[i].lane

    def spawn(self):
        if self.mode == "one":
            if len(self.vehicles) == 0:
                self.vehicles.append(Car())

        elif self.mode == "single" or self.mode == "double":
            if np.random.random() < self.spawn_prob and (
                    len(self.vehicles) == 0 or self.vehicles[-1].distance > 55):
                self.vehicles.append(Car())

    def renew_speeds(self):
        brake_action = [False for _ in range(len(self.vehicles))]

        for i, car in enumerate(self.vehicles):
            margin = 55
            cars_in_front = []
            j = i - 1  # vehicles are ordered largest distance to smallest
            while j >= 0 and self.vehicles[j].distance - car.distance < margin:
                cars_in_front.append(self.vehicles[j])
                j -= 1

            fronters = [other_car for other_car in cars_in_front if other_car.lane == car.lane]
            if len(fronters) > 0:
                car.speed = fronters[0].speed
                brake_action[i] = True

        for i, car in enumerate(self.vehicles):
            if not brake_action[i]:
                self.vehicles[i].renew_speed()





    def step_forward(self):
        self.spawn()

        if self.mode == "double":
            self.switch_lane()

        self.renew_speeds()

        for i, car in enumerate(self.vehicles):
            self.vehicles[i].step(self.time_step_size)

            if self.vehicles[i].distance >= self.length:
                self.time_taken.append(self.vehicles[i].age)

        self.vehicles = [car for car in self.vehicles if car.distance < self.length]
        self.sort_cars()

    def sort_cars(self):
        L = len(self.vehicles)
        check = True
        while check and L > 0:
            check = False
            for i in range(L - 1):
                if self.vehicles[i + 1].distance > self.vehicles[i].distance:
                    self.vehicles[i], self.vehicles[i+1] = self.vehicles[i+1], self.vehicles[i]
                    check = True

        """
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
                        print(f"Car 1 target: {self.vehicles[i].target}")
                        print(f"Car 2 target: {self.vehicles[i + 1].target}")
                        print(f"Car 1 action: {self.vehicles[i].action}")
                        print(f"Car 2 action: {self.vehicles[i + 1].action}")
        """

    def get_average_time(self):
        return np.mean(self.time_taken)

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
        plt.title("Histogram arrival times")
        plt.show()

    def create_histogram_close(self, n_bins=30):
        minutes_taken = np.sort(np.array(self.time_taken) / 60)  # Convert to minutes
        L = len(minutes_taken)
        minutes_taken = minutes_taken[0: int(9 * L // 10)]
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
        plt.title("Histogram arrival times 90 quantile")
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