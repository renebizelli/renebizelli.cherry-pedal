from models.song import Song
from services.audio_service import AudioService


class SongService:

    def __init__(self, callback):
        self._index = 0
        self._callback = callback
        self._songs = []
        self._audio_service = None

    def set_songs(self, songs: []):
        self._songs = songs
        self._index = 0
        if len(self._songs) > 0:
            self._init_audio()

    def _init_audio(self):
        song = self.current()
        if self._audio_service is not None:
            self._audio_service.stop()
        
        self._audio_service = AudioService(song, self._callback)

    def forward(self):
        if len(self._songs) == 0:
            return

        self._index = self._index+1
        self._index = 0 if self._index == len(
            self._songs) else self._index
        self._init_audio()

    def backward(self):
        if len(self._songs) == 0:
            return

        self._index = self._index-1
        self._index = len(self._songs)-1 if self._index < 0 else self._index
        self._init_audio()

    def current(self):
        if len(self._songs) == 0:
            return None
        return self._songs[self._index]

    def next_audio(self):
        if self._audio_service is None:
            return

        self._audio_service.forward()

    def current_audio(self):
        if self._audio_service is None:
            return None

        return self._audio_service.current()

    def play(self):
        if self._audio_service is None:
            return

        return self._audio_service.play()

    def stop(self):
        if self._audio_service is None:
            return

        return self._audio_service.stop()

    def forwardAudio(self):
        return self.next_audio()

    def currentAudio(self):
        return self.current_audio()


Song_Service = SongService
