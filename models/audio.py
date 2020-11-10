class Audio:

    def __init__(self, id: str, name: str, file: str):
        self._id = id
        self._name = name
        self._file = file

        self._selected = False

    @property
    def id(self):
        return self._id

    @property
    def name(self):
        return self._name

    @property
    def file(self):
        return self._file

    @property
    def selected(self):
        return self._selected

    def set_selected(self, id: str):
        self._selected = self._id == id
