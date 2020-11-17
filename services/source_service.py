import io
import json

from models.song import Song
from models.audio import Audio
from models.band import Band


class Source_Service:

    def __init__(self):

        self._data = None
        self.__load__()

    def bands(self):

        bands = []

        for band in self._data['bands']:
            bands.append(Band(band['id'], band['name']))

        return bands

    def songs(self, band: Band):

        songs = []

        path = self._data['path']

        for song in self._data[band.id]:

            audios = []

            for audio in song['audios']:

                file = self.__path_audio_file__(
                    path, band, song['id'], audio['file'])

                audios.append(
                    Audio(audio['file'], audio['name'], file))

            songs.append(Song(
                song['id'], song['name'], song['autoforward'], audios))
        
        
        return songs


    def __load__(self):
        with io.open('source.json', 'r', encoding='utf-8-sig') as json_file:
            self._data = json.load(json_file)

    def __path_audio_file__(self, path: str, band: Band, song_id: str, file: str):
        return "{}/{}/{}/{}".format(path, band.id, song_id, file)
