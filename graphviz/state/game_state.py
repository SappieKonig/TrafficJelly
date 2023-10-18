from dataclasses import dataclass


@dataclass(kw_only=True)
class GameState:
    """
    Attributes
    - `alpha` is the interpolation factor between simulation steps. It is equal to 0 when it is at the start of a step.
      It is equal to 1 at the end of a step in the simulation. When alpha is greater than or equal to 1, the step
      function of the simulation is called.
    """
    # Simulation state
    alpha: float
    delta_time: float
    time_scale: float
