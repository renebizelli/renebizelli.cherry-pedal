from services.song_service import SongService


class PedalController:

    def __init__(self, songs, audio_event_callback):
        self._song_service = SongService(audio_event_callback)
        self._song_service.set_songs(songs)

    def current_song(self):
        return self._song_service.current()

    def current_audio(self):
        return self._song_service.current_audio()

    def next_song(self):
        self._song_service.forward()
        return self.current_song()

    def previous_song(self):
        self._song_service.backward()
        return self.current_song()

    def next_audio(self):
        self._song_service.next_audio()
        return self.current_song()

    def play(self):
        self._song_service.play()

    def stop(self):
        self._song_service.stop()


Pedal_Controller = PedalController
