from models.audio import Audio
import os
import threading
import time
from services.audio_events import AUDIO_ENDS, AUDIO_NOT_FOUND, AUDIO_STARTS
from services.pygame_audio_player import PygameAudioPlayer

END_BY_PLAY = 9000
END_BY_STOP = 9001


class AudioNotFoundError(FileNotFoundError):
    pass


class PlayerService:
    _current_play_id = None
    _play_sequence = 0
    _lock = threading.Lock()

    def __init__(self, audio: Audio, callback, autoforward, player=None):

        self._audio = audio
        self._callback = callback
        self._autoforward = autoforward
        self._player = player or PygameAudioPlayer()
        self._end = 0
        self._play_id = None

        if not os.path.isfile(audio.file):
            self._callback(AUDIO_NOT_FOUND)
            raise AudioNotFoundError("Audio resource " + audio.file + " not found")

        self._player.load(self._audio.file)

    def _set_end(self, code: int):
        self._end = code
        
    def _ended_by_play(self) -> bool:
        return self._ended_by(END_BY_PLAY)
    
    def _ended_by_stop(self) -> bool:
        return self._ended_by(END_BY_STOP)

    def _ended_by(self, code: int) -> bool:
        return self._end == code

    def play(self):
        if self._player.is_playing():
            self.stop()

        with PlayerService._lock:
            PlayerService._play_sequence += 1
            self._play_id = PlayerService._play_sequence
            PlayerService._current_play_id = self._play_id

        self._set_end(END_BY_PLAY)
        self._player.play()
        t = threading.Thread(target=self._check_end, args=[self._play_id], daemon=True)
        t.start()
        self._callback(AUDIO_STARTS)

    def stop(self):
        owns_current_play = self._owns_current_play()
        self._set_end(END_BY_STOP)
        self._player.stop()

        if owns_current_play:
            with PlayerService._lock:
                PlayerService._current_play_id = None

            self._callback(AUDIO_ENDS)

    def _owns_current_play(self):
        with PlayerService._lock:
            return self._play_id is not None and self._play_id == PlayerService._current_play_id

    def _check_end(self, play_id):
        
        keepon = True
        
        while keepon:
            keepon = self._player.is_playing()
            time.sleep(0.01)

        with PlayerService._lock:
            if play_id != PlayerService._current_play_id:
                return

            PlayerService._current_play_id = None

        if self._ended_by_play():
            self._autoforward()
            self._callback(AUDIO_ENDS)

        elif self._ended_by_stop():
            self._callback(AUDIO_ENDS)


Player_Service = PlayerService
