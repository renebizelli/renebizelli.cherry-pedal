import abc


class AudioPlayer(metaclass=abc.ABCMeta):

    @abc.abstractmethod
    def load(self, file):
        pass

    @abc.abstractmethod
    def play(self):
        pass

    @abc.abstractmethod
    def stop(self):
        pass

    @abc.abstractmethod
    def is_playing(self):
        pass


Audio_Player = AudioPlayer
