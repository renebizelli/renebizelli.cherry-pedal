from tkinter import *
from models.song import Song
from models.audio import Audio
from models.band import Band
from services.song_service import Song_Service
from services.source_service import Source_Service
from apps.app_painel import Painel
from apps.app_setup import Setup


def songForwardButton_click():
    songService.forward()
    screenBuild()


def songBackwardButton_click():
    songService.backward()
    screenBuild()


def audioForwardButton_click():
    songService.forwardAudio()
    screenBuild()


def playButton_click():
    songService.play()


def stopButton_click():
    songService.stop()


def screenBuild():
    song = songService.current()
    painel.redrawn(song, end_app)


def callback(command):
    if command == 'AUDIO_STARTS':
        print("Start")
    elif command == 'AUDIO_ENDS':
        screenBuild()


def end_app(a):
    exit()


def band_selected(band: Band):
    songs = source.songs(band)
    songService.set_songs(songs)
    print("select band", band)


root = Tk()

songService = Song_Service(callback)

painel = Painel(root)

painel.window_configure()
source = Source_Service()

bands = source.bands()
songs = []

setup = Setup(root, bands, band_selected)
setup.redrawn()


btnFrame = Frame(root)
btnFrame.pack()

songForwardButton = Button(btnFrame, text="Song >>",
                           command=songForwardButton_click)
songForwardButton.pack()

songBackwardButton = Button(btnFrame, text="Song <<",
                            command=songBackwardButton_click)
songBackwardButton.pack()

audioForwardButton = Button(btnFrame, text="Audio >>",
                            command=audioForwardButton_click)
audioForwardButton.pack()

playButton = Button(btnFrame, text="Play", command=playButton_click)
playButton.pack()

stopButton = Button(btnFrame, text="Stop", command=stopButton_click)
stopButton.pack()

screenBuild()

root.mainloop()
