class Band:
    def __init__(self, id: str, name: str, logo: bool):
        self._id = id
        self._name = name
        self._logo = logo

    @property
    def id(self):
        return self._id

    @property
    def name(self):
        return self._name

    @property
    def logo(self):
        return self._logo
