from models.audio import Audio
from models.song import Song
from services.player_service import PlayerService


class AudioService:

    def __init__(self, song: Song, callback):
        self._song = song
        self._index = 0
        self._callback = callback
        self._players = {}
        self._player = None
        self._preload_players()
        self._select_current_audio()

    def forward(self):
        
        self._index = self._index+1

        self._index = 0 if self._index == len(
            self._song.audios) else self._index

        self._select_current_audio()

    def current(self) -> Audio :
        return self._song.audios[self._index]

    def play(self):
        self._player.play()

    def stop(self):
        self._player.stop()

    def autoforward(self):
        if self._song.autoforward:
            self.forward()

    def _select_current_audio(self):
        current = self.current()

        if self._player is not None:
            self._player.stop()

        self._player = self._players[current.id]

    def _preload_players(self):
        for audio in self._song.audios:
            self._players[audio.id] = PlayerService(
                audio,
                self._callback,
                self.autoforward
            )


Audio_Service = AudioService
