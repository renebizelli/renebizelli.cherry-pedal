import json
from pathlib import Path

from models.song import Song
from models.audio import Audio
from models.band import Band


class MissingAudioFilesError(FileNotFoundError):

    def __init__(self, missing_files):
        self.missing_files = missing_files
        message = "Audio files not found:\n{}".format(
            "\n".join(str(file) for file in missing_files)
        )
        super().__init__(message)


class SourceService:

    def __init__(self, config_file='source.json'):

        self._data = None
        self._config_file = Path(config_file)
        self._project_root = self._config_file.resolve().parent
        self._load()

    def bands(self):

        bands = []

        for band in self._data['bands']:
            bands.append(Band(band['id'], band['name'], band['logo']))

        return bands

    def songs(self, band: Band):

        songs = []

        path = self._resolve_path(self._data.get('path', 'bands'))

        for song in self._data[band.id]:

            audios = []

            for audio in song['audios']:

                file = self._path_audio_file(path, band, song['id'], audio['file'])

                audios.append(
                    Audio(audio['file'], audio['name'], str(file)))

            songs.append(Song(
                song['id'], song['name'], song['autoforward'], audios))
        
        
        return songs

    def validate_audio_files(self):
        missing_files = []

        for band in self.bands():
            for song in self.songs(band):
                for audio in song.audios:
                    audio_file = Path(audio.file)

                    if not audio_file.is_file():
                        missing_files.append(audio_file)

        if len(missing_files) > 0:
            raise MissingAudioFilesError(missing_files)


    def _load(self):
        with self._config_file.open('r', encoding='utf-8-sig') as json_file:
            self._data = json.load(json_file)

    def _resolve_path(self, path: str):
        source_path = Path(path)

        if source_path.is_absolute():
            return source_path

        return self._project_root / source_path

    def _path_audio_file(self, path: Path, band: Band, song_id: str, file: str):
        return path / band.id / song_id / file


Source_Service = SourceService
