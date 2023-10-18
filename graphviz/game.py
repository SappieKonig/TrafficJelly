import pygame

from graphviz.simulation import Simulation
from graphviz.state.game_state import GameState
from graphviz.view.game_road_view import GameRoadView
from graphviz.view.game_view import GameView


class Game:
    FPS = 60

    state: GameState

    def __init__(self, *, simulation: Simulation):
        pygame.init()

        self.font = pygame.font.Font(None, 36)

        self.screen = pygame.display.set_mode((1280, 720))
        self.clock = pygame.time.Clock()
        self.display = pygame.display
        self.simulation = simulation
        self.views = []

        self.time_scale_rate = 2

        self.dt = 0
        self.state = GameState(
            alpha=1,
            delta_time=simulation.get_delta_time(),
            time_scale=1.0,
        )

    def push_view(self, view: GameView):
        self.views.append(view)

    def main(self):
        self.loop()
        pygame.quit()

    def loop(self):
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                self.on_event(event)
            self.update_viewport()
            self.update_state()
            self.render()

            self.display.flip()
            self.dt = self.clock.tick(self.FPS) / 1000
            self.state.alpha += self.dt * self.state.time_scale / self.state.delta_time

    def on_event(self, event: pygame.event.Event):
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_PERIOD:
                self.state.time_scale *= self.time_scale_rate
            if event.key == pygame.K_COMMA:
                self.state.time_scale /= self.time_scale_rate
        self.view.on_event(event)

    def update_viewport(self):
        self.view.update_viewport()

    def update_state(self):
        self.view.update_state()

    def render(self):
        self.view.render()

    @property
    def view(self) -> GameView:
        return self.views[-1]
