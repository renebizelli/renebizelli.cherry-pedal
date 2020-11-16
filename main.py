from tkinter import *
from models.song import Song
from models.audio import Audio
from services.song_service import Song_Service
from services.source_service import Source_Service
from helpers.ui_helper import UI_Helper 

# root.grid_columnconfigure(0, weight=1, pad=0)
# root.grid_columnconfigure(1, weight=1, pad=0)
# root.grid_columnconfigure(2, weight=1, pad=0)
# l1 = Label(root, text="Label 1", bg="blue")
# l1.grid(row=0, column=0)
# l2 = Label(root, text="Label 2", bg="green")
# l2.grid(row=1, column=0)
# root.mainloop()

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

    ui.redrawn(song)

    # ui.set_song_name(song)
    ui.set_song_autoforward(song)

def callback(command):
    if command == 'AUDIO_STARTS':
        print("Start")
    elif command == 'AUDIO_ENDS':
        screenBuild()


def end_app(a):
    exit()


def select_band(a):
    print("double")

root = Tk()

ui = UI_Helper(root)

ui.window_configure()

source = Source_Service()
songs = source.get()

btnFrame = Frame(root)
btnFrame.grid(row=7, column=0)

songService = Song_Service(songs, callback)

songForwardButton = Button(btnFrame, text="Song >>",
                           command=songForwardButton_click)
songForwardButton.grid(row=1, column=0)

songBackwardButton = Button(btnFrame, text="Song <<",
                            command=songBackwardButton_click)
songBackwardButton.grid(row=1, column=1)

audioForwardButton = Button(btnFrame, text="Audio >>",
                            command=audioForwardButton_click)
audioForwardButton.grid(row=1, column=2)

playButton = Button(btnFrame, text="Play", command=playButton_click)
playButton.grid(row=1, column=3)

stopButton = Button(btnFrame, text="Stop", command=stopButton_click)
stopButton.grid(row=1, column=4)

ui.cherry_button(end_app)

screenBuild()

root.mainloop()
