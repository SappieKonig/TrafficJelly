import random
from collections import defaultdict
from dataclasses import dataclass

import numpy as np

DEFAULT_SLEEP_SIGMA = 5 * 60
DEFAULT_SLEEP_MEAN = 15 * 60

DEFAULT_V_SIGMA = 3
DEFAULT_V_MEAN = 30


@dataclass(kw_only=True)
class SimCar:
    x: float
    lane: int
    sleep: float

    v_mean: float
    v_sigma: float
    sleep_mean: float
    sleep_sigma: float


@dataclass(kw_only=True)
class SimNode:
    x: float
    y: float
    car_ids: set[int]


@dataclass(kw_only=True)
class SimEdge:
    start_node_id: int
    end_node_id: int

    lane_count: int
    length: float
    car_ids: set[int]


class Simulation:
    __dt: float
    __cars: dict[int, SimCar]
    __nodes: dict[int, SimNode]
    __edges: dict[int, SimEdge]

    def __init__(self):
        self.__dt = 0.2
        self.__cars = {}
        self.__nodes = {
            0: SimNode(x=-3000, y=500, car_ids=set()),
            1: SimNode(x=1500, y=2000, car_ids=set()),
            2: SimNode(x=3000, y=-1000, car_ids=set()),
        }
        dist01 = ((1500 - -3000) ** 2 + (2000 - 500) ** 2) ** .5
        dist02 = ((3000 - -3000) ** 2 + (-1000 - 500) ** 2) ** .5
        dist12 = ((3000 - 1500) ** 2 + (-1000 - 2000) ** 2) ** .5
        self.__edges = {
            0: SimEdge(start_node_id=0, end_node_id=1, length=dist01, lane_count=1, car_ids=set()),
            1: SimEdge(start_node_id=1, end_node_id=0, length=dist01, lane_count=1, car_ids=set()),
            # 4: SimEdge(start_node_id=1, end_node_id=0, length=dist01, lane_count=1, cars=set()),
            # 5: SimEdge(start_node_id=1, end_node_id=0, length=dist01, lane_count=1, cars=set()),
            # 6: SimEdge(start_node_id=1, end_node_id=0, length=dist01, lane_count=1, cars=set()),
            2: SimEdge(start_node_id=0, end_node_id=2, length=dist02, lane_count=1, car_ids=set()),
            3: SimEdge(start_node_id=2, end_node_id=1, length=dist12, lane_count=1, car_ids=set()),
        }
        self.__depart_edges_ids_per_node_id = {
            node_id:
                [edge_id for edge_id, edge in self.__edges.items() if edge.start_node_id == node_id]
            for node_id in self.__nodes.keys()
        }
        self.__next_car_id = 0

    def get_delta_time(self) -> float:
        return self.__dt

    def step_forward(self):
        arrived_car_ids_per_edge: dict[int, list[int]] = defaultdict(list)
        departed_car_ids_per_node: dict[int, list[int]] = defaultdict(list)

        for edge_id, edge in self.__edges.items():
            for car_id in edge.car_ids:
                car = self.__cars[car_id]
                v = np.random.normal(car.v_mean, car.v_sigma)
                car.x += v * self.__dt

                # Cars that have reached the end of the road
                if car.x > edge.length:
                    arrived_car_ids_per_edge[edge_id].append(car_id)

        for node_id, node in self.__nodes.items():
            for car_id in node.car_ids:
                car = self.__cars[car_id]
                car.sleep -= self.__dt

                # Cars that have reached the end of their "sleep"
                if car.sleep <= 0:
                    departed_car_ids_per_node[node_id].append(car_id)

        for edge_id, car_ids in arrived_car_ids_per_edge.items():
            edge = self.__edges[edge_id]
            arrival_node_id = edge.end_node_id
            arrival_node = self.__nodes[arrival_node_id]
            for car_id in car_ids:
                edge.car_ids.remove(car_id)
                car = self.__cars[car_id]
                car.x = 0
                car.lane = 0
                car.sleep = np.random.normal(car.sleep_mean, car.sleep_sigma)
                arrival_node.car_ids.add(car_id)

        for node_id, car_ids in departed_car_ids_per_node.items():
            node = self.__nodes[node_id]
            departure_edge_ids = self.__depart_edges_ids_per_node_id[node_id]
            for car_id in car_ids:
                car = self.__cars[car_id]
                car.x = 0
                car.lane = 0
                car.sleep = 0
                node.car_ids.remove(car_id)
                departure_edge_id = random.choice(departure_edge_ids)
                departure_edge = self.__edges[departure_edge_id]
                departure_edge.car_ids.add(car_id)

    def get_car_x(self, car_id: int) -> float:
        return self.__cars[car_id].x

    def get_car_lane(self, car_id: int) -> float:
        return self.__cars[car_id].lane

    def get_node_ids(self) -> list[int]:
        return list(self.__nodes.keys())

    def get_edge_ids(self) -> list[int]:
        return list(self.__edges.keys())

    def get_car_ids_in_edge_on_interval(self, edge_id: int, interval: tuple[int, int]):
        x_start, x_end = interval
        return [car_id for car_id in self.__edges[edge_id].car_ids if x_start <= self.__cars[car_id].x <= x_end]

    def get_car_count_histogram_in_edge(self, edge_id: int, bin_distance: float):
        edge = self.__edges[edge_id]
        distances = [self.__cars[car_id].x for car_id in edge.car_ids]
        bins = np.arange(0, edge.length, bin_distance)
        hist, edges = np.histogram(distances, bins)
        # hist[i] contains the count of distances between (edges[i] and edges[i+1])
        # edges should be about equal to bins
        return hist, edges

    def get_node_pos(self, node_id: int) -> tuple[float, float]:
        node = self.__nodes[node_id]
        return node.x, node.y

    def get_edge_start_node_id(self, edge_id: int) -> int:
        return self.__edges[edge_id].start_node_id

    def get_edge_end_node_id(self, edge_id: int) -> int:
        return self.__edges[edge_id].end_node_id

    def get_edge_lane_count(self, edge_id: int) -> int:
        return self.__edges[edge_id].lane_count

    def get_edge_road_length(self, edge_id: int) -> float:
        return self.__edges[edge_id].length

    def _add_cars_to_edge(self, edge_id: int, nr_cars: int, x_start=0, x_delta=25):
        cars = self.__cars
        edge = self.__edges[edge_id]
        for i in range(nr_cars):
            car_id = self.__get_new_car_id()
            x = x_start + i * x_delta
            cars[car_id] = SimCar(x=x, lane=0, sleep=0,
                                  v_mean=DEFAULT_V_MEAN,
                                  v_sigma=DEFAULT_V_SIGMA,
                                  sleep_mean=DEFAULT_SLEEP_MEAN,
                                  sleep_sigma=DEFAULT_SLEEP_SIGMA)
            edge.car_ids.add(car_id)

    def __get_new_car_id(self):
        car_id = self.__next_car_id
        self.__next_car_id += 1
        return car_id
