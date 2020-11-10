from models.audio import Audio
from models.song import Song
from services.player_service import Player_Service


class Audio_Service:

    def __init__(self, song: Song, callback):
        self._song = song
        self._index = 0
        self._callback = callback
        self.__selected__()

    def forward(self):

        self._index = self._index+1

        self._index = 0 if self._index == len(
            self._song.audios) else self._index

        self.__selected__()

    def current(self):
        return self._song.audios[self._index]

    def play(self):
        self._player.play()

    def stop(self):
        self._player.stop()

    def autoforward(self):
        if self._song.autoforward:
            self.forward()

    def __selected__(self):
        current = self.current()
        self._player = Player_Service(
            current, self._callback, self.autoforward)
        for audio in self._song.audios:
            audio.set_selected(current.id)
