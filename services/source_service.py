import json
from models.song import Song


class Source_Service:

    def __init__(self):

        self._data = None
        self._songs = [Song]

        self.__load__()

    def __load__(self):

        with open('source.json') as f:
            data = json.load(f)

        print('>', self._data)

        # for song in self._data['songs']:
        #     print('>', song.name)
