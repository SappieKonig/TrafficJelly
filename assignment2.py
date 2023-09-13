import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm


class Car:
    def __init__(self):
        self.speed = np.random.normal(100, 4)
        self.age = 0
        self.distance = 0

    def step(self):
        self.age += 1


class Road:
    def __init__(self, length, spawn_prob=0.1):
        self.length = length
        self.vehicles: list[Car] = []
        self.time_taken = []
        self.time_step_size = 1/3600
        self.spawn_prob = spawn_prob

    def step(self):
        last_distance = self.length + 1
        i = 0
        while i < len(self.vehicles):
            car = self.vehicles[i]
            car.distance += car.speed * self.time_step_size
            if car.distance >= last_distance - 55e-3:
                car.distance = last_distance - 55e-3

            if car.distance >= self.length:
                self.time_taken.append(car.age * self.time_step_size)
                self.vehicles.pop(i)
                continue
            else:
                i += 1

            last_distance = car.distance
            car.step()

        if np.random.random() < self.spawn_prob:
            self.vehicles.append(Car())

    def get_average_time(self):
        return np.mean(self.time_taken)

    def create_histogram(self, n_bins = 30):
        minutes_taken = np.array(self.time_taken) * 60  # Convert to minutes
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
        plt.title(f"Average time taken for lane with spawn probability of {self.spawn_prob}")
        plt.show()


road = Road(5, 0.3)

for i in range(int(1e5)):
    road.step()

print(road.get_average_time() * 60)

road.create_histogram()
