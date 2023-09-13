import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm


class Car:
    def __init__(self):
        # every car has a personal target speed
        self.target = np.random.normal(100, 4)  # km/h
        self.speed = 60
        self.age = 0  # seconds
        self.distance = 0  # km
        self.lane = 0

    def step(self, time_step_size):
        self.age += time_step_size
        self.distance += self.speed * time_step_size / 3600

    def brake(self, time_step_size):
        self.speed -= 20 * time_step_size
        if self.speed < 0:
            self.speed = 0

    def accelerate(self, time_step_size):
        self.speed += 5 * time_step_size

    def renew_speed(self, time_step_size):
        if np.random.normal(self.get_target(), 4) > self.speed:
            self.accelerate(time_step_size)
        else:
            self.brake(time_step_size)

    def get_target(self):
        return self.target + 20 * self.lane


class Road:
    def __init__(self, length, spawn_prob_per_sec=0.1):
        self.length = length  # km
        self.vehicles: list[Car] = []
        self.time_taken = []
        self.time_step_size = 1  # seconds
        self.spawn_prob = spawn_prob_per_sec * self.time_step_size

    def step(self):
        switch_lane_action = [False for _ in range(len(self.vehicles))]
        for i, car in enumerate(self.vehicles):
            cars_in_front = []
            j = i - 1  # vehicles are ordered largest distance to smallest
            while j >= 0 and self.vehicles[j].distance - car.distance < 55e-3:
                cars_in_front.append(self.vehicles[j])
                j -= 1

            cars_in_back = []
            j = i + 1
            while j < len(self.vehicles) and car.distance - self.vehicles[j].distance < 55e-3:
                cars_in_back.append(self.vehicles[j])
                j += 1

            if car.lane == 0:
                if not any([other_car.lane == 0 for other_car in cars_in_front]):
                    car.renew_speed(self.time_step_size)
                elif (not any([other_car.lane == 1 for other_car in cars_in_front])
                          and not any([other_car.lane == 1 for other_car in cars_in_back])):
                    switch_lane_action[i] = True
                    car.accelerate(self.time_step_size)
                else:
                    car.brake(self.time_step_size)

            elif car.lane == 1:
                if (not any([other_car.lane == 0 for other_car in cars_in_front])
                        and not any([other_car.lane == 0 for other_car in cars_in_back])):
                    switch_lane_action[i] = True
                    car.brake(self.time_step_size)
                elif not any([other_car.lane == 1 for other_car in cars_in_front]):
                    car.renew_speed(self.time_step_size)
                else:
                    car.brake(self.time_step_size)

        for i in range(len(self.vehicles)):
            if switch_lane_action[i]:
                self.vehicles[i].lane = 1 - self.vehicles[i].lane

            self.vehicles[i].step(self.time_step_size)

            if self.vehicles[i].distance >= self.length:
                self.time_taken.append(self.vehicles[i].age)

        if np.random.random() < self.spawn_prob and (len(self.vehicles) == 0 or self.vehicles[-1].distance > 0.02):
            self.vehicles.append(Car())
            if len(self.vehicles) > 1 and self.vehicles[-2].distance < 0.1:
                self.vehicles[-1].speed = min(self.vehicles[-2].speed, self.vehicles[-1].speed)


        self.vehicles = [car for car in self.vehicles if car.distance < self.length]
        self.vehicles = list(sorted(self.vehicles, key=lambda x: x.distance, reverse=True))

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


road = Road(5, 0.5)

for i in range(10000):
    road.step()

print(road.get_average_time() * 60)

road.create_histogram()