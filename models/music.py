from models.audio import Audio


class Music:
    def __init__(self, id: str, name: str, autoforward: bool, audios: [Audio]):
        self._id = id
        self._name = name
        self._autoforward = autoforward
        self._audios = audios

    @property
    def id(self):
        return self._id

    @property
    def name(self):
        return self._name

    @property
    def autoforward(self):
        return self._autoforward

    @property
    def audios(self):
        return self._audios
