from abc import abstractmethod, ABCMeta

import pygame


class GameView(metaclass=ABCMeta):
    @abstractmethod
    def on_event(self, event: pygame.event.Event): pass

    @abstractmethod
    def update_viewport(self): pass

    @abstractmethod
    def update_state(self): pass

    @abstractmethod
    def render(self): pass