from models.music import Music
from controllers.audio_controller import Audio_Controller


class Music_Controller:

    def __init__(self, musics: [Music], callback):
        self._musics = musics
        self._index = 0
        self._callback = callback
        self._audio_init_()
 
    def _audio_init_(self):
        music = self.current()
        self._audioController = Audio_Controller(music, self._callback)

    def forward(self):
        self._index = self._index+1
        self._index = 0 if self._index == len(
            self._musics) else self._index
        self._audio_init_()

    def current(self):
        return self._musics[self._index]

    def forwardAudio(self):
        self._audioController.forward()

    def currentAudio(self):
        return self._audioController.current()

    def play(self):
        return self._audioController.play()
 
    def stop(self):
        return self._audioController.stop()