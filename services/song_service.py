from models.song import Song
from services.audio_service import Audio_Service


class Song_Service:

    def __init__(self, callback):
        self._index = 0
        self._callback = callback
        self._songs = []
        self._audioService = None

    def set_songs(self, songs: []):
        self._songs = songs
        self._index = 0
        self._audio_init_()

    def _audio_init_(self):
        song = self.current()
        if not self._audioService == None:
            self._audioService.stop()
        
        self._audioService = Audio_Service(song, self._callback)

    def forward(self):
        self._index = self._index+1
        self._index = 0 if self._index == len(
            self._songs) else self._index
        self._audio_init_()

    def backward(self):
        self._index = self._index-1
        self._index = len(self._songs)-1 if self._index < 0 else self._index
        self._audio_init_()

    def current(self):
        if len(self._songs) == 0:
            return None
        return self._songs[self._index]

    def forwardAudio(self):
        self._audioService.forward()

    def currentAudio(self):
        return self._audioService.current()

    def play(self):
        return self._audioService.play()

    def stop(self):
        return self._audioService.stop()
