from tkinter import *
from models.song import Song
from models.audio import Audio
from services.song_service import Song_Service
from services.source_service import Source_Service
from helpers.ui_helper import label_format

root = Tk()
root.title('Cherry')
root.geometry("600x400")
root.resizable(False, False)

source = Source_Service()

songs = source.get()

for s in songs:
    print(">>>", s.name)

labels = []

songName = StringVar()
songNameLabel = Label(root, textvariable=songName)
songNameLabel.grid(row=0, column=0)

label_format(songNameLabel, 20, False)


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
    songName.set(song.name)

    for l in labels:
        l.destroy()

    labels.clear()

    for i, audio in enumerate(song.audios):
        label = Label(root, text=audio.name)

        label_format(label, 16, audio.selected)

        label.grid(row=i + 3)
        labels.append(label)


def callback(command):
    if command == 'AUDIO_STARTS':
        print("Start")
    elif command == 'AUDIO_ENDS':
        screenBuild()


songService = Song_Service(songs, callback)

songForwardButton = Button(root, text="Song >>",
                           command=songForwardButton_click)
songForwardButton.grid(row=1, column=0)

songBackwardButton = Button(root, text="Song <<",
                            command=songBackwardButton_click)
songBackwardButton.grid(row=2, column=0)

audioForwardButton = Button(root, text="Audio >>",
                            command=audioForwardButton_click)
audioForwardButton.grid(row=1, column=1)

playButton = Button(root, text="Play", command=playButton_click)
playButton.grid(row=1, column=2)

stopButton = Button(root, text="Stop", command=stopButton_click)
stopButton.grid(row=1, column=3)

# ************************************************************

screenBuild()

# ************************************************************

root.mainloop()
