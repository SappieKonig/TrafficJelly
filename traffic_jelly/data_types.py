from dataclasses import dataclass


@dataclass(kw_only=True)
class GraphicsCar:
    id: int
    x: float
    y: float
