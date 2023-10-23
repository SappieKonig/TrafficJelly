from dataclasses import dataclass


@dataclass(kw_only=True)
class VizCar:
    id: int
    x: float
    y: float


@dataclass(kw_only=True)
class GameRoadState:
    scale: float
    pan_offset_x: float
    pan_offset_y: float
    prev_cars: dict[int, VizCar]
    next_cars: dict[int, VizCar]
