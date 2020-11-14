import io
import json

from models.song import Song
from models.audio import Audio


class Source_Service:

    def __init__(self):

        self._data = None
        self._songs = []
        self.__load__()

    def __load__(self):

        with io.open('source.json', 'r', encoding='utf-8-sig') as json_file:
            self._data = json.load(json_file)

            self._songs = []

            path = self._data['path']

            for song in self._data['songs']:

                audios = []

                for audio in song['audios']:

                    file = self.__path_file_prepare__(
                        path, song['id'], audio['file'])

                    audios.append(
                        Audio(audio['file'], audio['name'], file))

                self._songs.append(Song(
                    song['id'], song['name'], song['autoforward'], audios))

    def __path_file_prepare__(self, path: str, song_id: str, file:str):
        return "{}/{}/{}".format(path, song_id, file)

    def get(self):
        return self._songs
