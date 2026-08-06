from services.audio_player import AudioPlayer


class PygameAudioPlayer(AudioPlayer):
    _mixer_initialized = False
    _channel = None

    def __init__(self):
        import pygame
        self._pygame = pygame
        self._sound = None
        self._initialize_mixer()

    def _initialize_mixer(self):
        if PygameAudioPlayer._mixer_initialized:
            return

        self._pygame.mixer.pre_init(
            frequency=44100,
            size=-16,
            channels=2,
            buffer=256,
        )
        self._pygame.init()
        self._pygame.mixer.init()
        PygameAudioPlayer._channel = self._pygame.mixer.Channel(0)
        PygameAudioPlayer._mixer_initialized = True

    def load(self, file):
        self._sound = self._pygame.mixer.Sound(file)
        self._sound.set_volume(1.0)

    def play(self):
        PygameAudioPlayer._channel.stop()
        PygameAudioPlayer._channel.play(self._sound)

    def stop(self):
        PygameAudioPlayer._channel.stop()

    def is_playing(self):
        return PygameAudioPlayer._channel.get_busy()


Pygame_Audio_Player = PygameAudioPlayer
