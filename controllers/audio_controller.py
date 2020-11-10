from models.audio import Audio
from models.music import Music
from controllers.player_controller import Player_Controller


class Audio_Controller:

    def __init__(self, music: Music, callback):
        self._music = music
        self._index = 0
        self._callback = callback
        self.__selected__()

    def forward(self):

        self._index = self._index+1

        self._index = 0 if self._index == len(
            self._music.audios) else self._index

        self.__selected__()

    def current(self):
        return self._music.audios[self._index]

    def play(self):
        self._player.play()

    def stop(self):
        self._player.stop()

    def autoforward(self):
        if self._music.autoforward:
            self.forward()

    def __selected__(self):
        current = self.current()
        self._player = Player_Controller(
            current, self._callback, self.autoforward)
        for audio in self._music.audios:
            audio.set_selected(current.id)
