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

            for song in self._data['songs']:

                audios = []

                for audio in song['audios']:
                    audios.append(
                        Audio(audio['file'], audio['name'], audio['file']))

                self._songs.append(Song(
                    song['id'], song['name'], song['autoforward'], audios))

    def get(self):
        return self._songs
