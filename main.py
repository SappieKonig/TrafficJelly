import os
os.system("./build.sh")

import numpy as np
from scipy.integrate import quad
from scipy.interpolate import interp1d
from scipy.optimize import curve_fit, fsolve
from scipy.stats import probplot, invgauss
from scipy.special import gamma

from traffic_model import TrafficModel
import time
import matplotlib.pyplot as plt

from graphviz.game import Game
from graphviz.simulation import Simulation
from graphviz.view.game_graph_view import GameGraphView
from graphviz.view.game_road_view import GameRoadView


DELTA_TIME = 0.5
SCALE = 1

# fromNode, toNode, timeTaken, startTime
ArrivalStats = tuple[int, int, float, float]


def plot_arrival_stats_hist(arrival_stats: list[ArrivalStats], from_node_id: int, to_node_id: int,
                            from_name: str, to_name: str):
    plt.rcParams.update({'font.size': 16, 'axes.labelsize': 'large',
                         'axes.titlesize':'large', 'xtick.labelsize':'large', 'ytick.labelsize':'large'})

    filtered_arrival_stats = [arrival_stat for arrival_stat in arrival_stats if arrival_stat[0] == from_node_id and arrival_stat[1] == to_node_id]
    data = [arrival_stat[2] for arrival_stat in filtered_arrival_stats]
    n, bins, patches = plt.hist(data, bins=50, density=True, alpha=0.6, color='g')

    # Define the Weibull PDF function
    def weibull_pdf(x, c, scale):
        return (c / scale) * (x / scale)**(c-1) * np.exp(-(x / scale)**c)

    mu = np.mean(data)
    lambda_initial = mu**3 / np.var(data)


    # Define the Inverse Gaussian PDF for fitting
    def invgauss_pdf(x, mu, lambda_param):
        return np.sqrt(lambda_param / (2 * np.pi * x**3)) * np.exp(-lambda_param * (x - mu)**2 / (2 * mu**2 * x))

    def invgauss_cdf(x, mu, lambda_param):
    # CDF is the integral of the PDF from 0 to x
        return quad(invgauss_pdf, 0, x, args=(mu, lambda_param))[0]

# Find the x value (quantile) for which the CDF equals the percentile
    def find_quantile(p, mu, lambda_param):
        # Starting guess for fsolve, can be refined depending on the expected range of your data
        x0 = mu
        # Solve for the quantile
        quantile = fsolve(lambda x: invgauss_cdf(x, mu, lambda_param) - p, x0)[0]
        return quantile

    c_initial = 1.5  # Initial guess for the shape parameter
    mean_data = np.mean(data)
    scale_initial = mean_data / gamma(1 + 1 / c_initial)

    params_init = [c_initial, scale_initial]

    # Fit the Weibull distribution to the histogram data
    # params, cov = curve_fit(weibull_pdf, (bins[:-1] + bins[1:]) / 2, n, p0=params_init)
    params, cov = curve_fit(invgauss.pdf, (bins[:-1] + bins[1:]) / 2, n, p0=[mu, lambda_initial])

    # Generate a range of x values
    x = np.linspace(min(data), max(data), 100)
    # plt.plot(x, weibull_pdf(x, params[0], params[1]), 'r-', lw=2)
    plt.plot(x, invgauss_pdf(x, params[0], params[1]), 'r-', lw=2)
    # plt.legend(['Weibull fit', 'Histogram'])
    # Format the parameters with two significant digits in scientific notation
    mu_formatted = f"{params[0]:.2e}".replace('e+0','e').replace('e-0','e-').replace('e+','e')
    lambda_formatted = f"{params[1]:.2e}".replace('e+0','e').replace('e-0','e-').replace('e+','e')

    # Update the legend with the custom formatted strings
    plt.legend([
        f'Inverse Gaussian fit',
        'Journey times histogram'
    ])
    plt.title(f"Travel time from {from_name} to {to_name}")
    plt.xlabel('Travel time (s)')
    plt.ylabel('Probability density')

    plt.savefig(f"arrival_stats_from_{from_name}_to_{to_name}_hist.svg")

    # empty the plot
    plt.clf()

    sorted_data = np.sort(data)
    percentiles = np.linspace(0.01, 0.99, len(sorted_data))
    theoretical_quantiles = [find_quantile(p, params[0], params[1]) for p in percentiles]

    plt.plot(theoretical_quantiles, sorted_data, 'o')
    plt.plot(theoretical_quantiles, theoretical_quantiles, 'r-', lw=2)
    plt.title('Q-Q Plot')
    plt.xlabel('Theoretical Quantiles')
    plt.ylabel('Sample Quantiles')

    plt.savefig(f"arrival_stats_from_{from_name}_to_{to_name}_qq.svg")


def main():
    cars_per_edge = []
    simulation = TrafficModel("graph.txt", DELTA_TIME, SCALE)
    start = time.time()
    steps_per_day = int(3600 * 24 / DELTA_TIME * SCALE)
    until = int(12 / 24 * steps_per_day)
    for i in range(until):
        if i % 100 == 0:
            cars_per_edge.append(simulation.get_n_cars_per_edge())
            time_taken = time.time() - start
            print(f"Time taken for 100 steps: {time_taken:.2f}")
            start = time.time()
            n_cars = simulation.get_n_cars_in_simulation()
            print(f"Number of cars in simulation: {n_cars}")
            print(i / steps_per_day * 24)
        simulation.step_forward()
    # simulation = create_simulation()
    arrival_stats = simulation.get_travel_stats()
    # save arrival stats to file
    with open("arrival_stats.txt", "w") as f:
        for arrival_stat in arrival_stats:
            f.write(f"{arrival_stat[0]} {arrival_stat[1]} {arrival_stat[2]} {arrival_stat[3]}\n")
    filtered_arrival_stats = [arrival_stat for arrival_stat in arrival_stats if arrival_stat[0] == 0 and arrival_stat[1] == 1]
    plot_arrival_stats_hist(arrival_stats, 0, 1, simulation.get_label_from_node_id(0),
                            simulation.get_label_from_node_id(1))
    # game = Game(simulation=simulation)
    # game.push_view(GameGraphView(game=game))
    # game.main()


def create_simulation():
    simulation = Simulation()
    edge_ids = simulation.get_edge_ids()
    assert edge_ids
    # noinspection PyProtectedMember
    simulation._add_cars_to_edge(edge_ids[0], 50)
    return simulation


if __name__ == '__main__':
    main()
