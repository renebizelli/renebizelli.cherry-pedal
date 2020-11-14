from models.audio import Audio
import pygame
import os
import time
import threading
import time


class Player_Service:

    def __init__(self, audio: Audio, callback, autoforward):

        self._audio = audio
        self._callback = callback
        self._autoforward = autoforward
        self._ends_by_play = 9000
        self._ends_by_stop = 9001

        if not os.path.isfile(audio.file):
            self._callback('AUDIO_NOT_FOUND')
            raise Exception("Audio resource " + audio.file + " not found")

        pygame.init()
        pygame.mixer.init()
        pygame.mixer.music.load(self._audio.file)


    def play(self):

        if pygame.mixer.music.get_busy():
            self.stop()

        pygame.mixer.music.set_endevent(self._ends_by_play)
        pygame.mixer.music.play()
        t = threading.Thread(target=self.__check_ends__, daemon=False)
        t.start()
        self._callback('AUDIO_STARTS')

    def stop(self):
        pygame.mixer.music.set_endevent(self._ends_by_stop)
        pygame.mixer.music.stop()
        pygame.mixer.music.unload()
        pygame.mixer.music.load(self._audio.file)

    def __check_ends__(self):
        keepon = True

        for event in pygame.event.get():
            print('event', event.type)

            if event.type == self._ends_by_play:
                keepon = False
                self._autoforward()
                self._callback('AUDIO_ENDS')
                print('song end event play')

            elif event.type == self._ends_by_stop:
                keepon = False
                self._callback('AUDIO_ENDS')
                print('song end event by stop')

        if keepon:
            time.sleep(0.2)
            self.__check_ends__()
