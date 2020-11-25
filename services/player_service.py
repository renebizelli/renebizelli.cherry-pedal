from models.audio import Audio
import pygame
import os
import time
import threading

class Player_Service:

    def __init__(self, audio: Audio, callback, autoforward):

        self._audio = audio
        self._callback = callback
        self._autoforward = autoforward
        self._end_by_play = 9000
        self._end_by_stop = 9001
        self._end = 0

        if not os.path.isfile(audio.file):
            self._callback('AUDIO_NOT_FOUND')
            raise Exception("Audio resource " + audio.file + " not found")

        pygame.init()
        pygame.mixer.init()
        pygame.mixer.music.load(self._audio.file)
               

    def __set_end__(self, code:int):
        self._end = code
        
    def __end_compare_by_play__(self) -> bool :
        return self.__end_compare_(self._end_by_play)
    
    def __end_compare_by_stop__(self) -> bool :
        return self.__end_compare_(self._end_by_stop)

    def __end_compare_(self, code:int) -> bool :
        return self._end == code


    def play(self):
        if pygame.mixer.music.get_busy():
            self.stop()
        
        self.__set_end__(self._end_by_play)
        pygame.mixer.music.play()
        t = threading.Thread(target=self.__check_end__, daemon=False)
        t.start()
        self._callback('AUDIO_STARTS')

    def stop(self):
        self.__set_end__(self._end_by_stop)
        pygame.mixer.music.stop()
        # pygame.mixer.music.unload()
        pygame.mixer.music.load(self._audio.file)

    def __check_end__(self):
        
        keepon = True
        
        while keepon:
            keepon = pygame.mixer.music.get_busy()

        if self.__end_compare_by_play__():
            self._autoforward()
            self._callback('AUDIO_ENDS')

        elif self.__end_compare_by_stop__():
            self._callback('AUDIO_ENDS')
 
