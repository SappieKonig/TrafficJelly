import numpy as np
import matplotlib.pyplot as plt
import random

class Car:

    def __init__(self, x, v):
        """
        A car is modelled as a point with position x (in km)
        along the road moving with a velocity v (in km/h).
        """
        self.x = x
        self.v = v

class Model:

    def __init__(self, cars, length, dt):
        """
        The model is a road of a length (in km) that can be
        stepped through in time steps dt (in seconds).
        """
        self.length = length
        self.dt = dt
        self.t = 0

        self.cars = cars

    def step(self):
        """
        Every time step this method should be called to update the state of the model.
        All mechanics happen here.
        """
        for car in self.cars:
            new_car_x = car.x+(car.v/3600)*self.dt
            car.x = max(0, min(new_car_x, self.length))

        self.t += self.dt

    def get_car_xs(self):
        """
        Returns the cars velocities.
        """
        return [car.x for car in self.cars]

# Amount of simulation runs
N = 50000

# Velocity distribution
v_mean = 100
v_std = 4

# Road characteristics
road_length = 5
road_dt = 1

# List to collect duration statistics
durations = []

# We step through the model with the same parameters and a different random velocity N times and collect their durations
for i in range(N):

    # We setup the road with a single car of random velocity
    v = max(0, random.gauss(v_mean, v_std)) # We make sure that non-positive velocities are not allowed
    car = Car(0, v)
    model = Model([car], road_length, road_dt)
    
    while True:
        model.step()
        x = model.get_car_xs()[0] # There is only one car on the road
        if x == model.length:
            break

    durations += [model.t]

# We plot the durations in a histogram
durations = np.array(durations)
n_bins = 20
plt.hist(durations, density=True, bins=n_bins)

# We plot the expected normal distribution based on the found mean and standard deviation.
def norm(x, mean, std):
    return np.exp(-((x-mean)/std)**2/2)/(np.sqrt(2*np.pi)*std)

n_points = 100
t = np.linspace(min(durations), max(durations), n_points)
plt.plot(t, norm(t, np.mean(durations), np.std(durations)))
plt.show()
