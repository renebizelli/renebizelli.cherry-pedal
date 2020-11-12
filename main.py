from tkinter import *
from models.song import Song
from models.audio import Audio
from services.song_service import Song_Service
from services.source_service import Source_Service
from helpers.ui_helper import label_format, window_configure
from PIL import ImageTk, Image

root = Tk()


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

    for l in audiosLabel:
        l.destroy()

    audiosFrame = Frame(root, bg="white")
    audiosFrame.grid(row=1, column=0, padx=10)

    audiosLabel.clear()

    for i, audio in enumerate(song.audios):
        label = Label(audiosFrame, text=audio.name, width=50)
        label_format(label, 16, audio.selected)

        #label.grid(row=i, column=0)
        label.pack()
        audiosLabel.append(label)


def callback(command):
    if command == 'AUDIO_STARTS':
        print("Start")
    elif command == 'AUDIO_ENDS':
        screenBuild()


def end_app(a):
    exit()


window_configure(root)

source = Source_Service()
songs = source.get()

audiosLabel = []

songName = StringVar()
songNameLabel = Label(root, textvariable=songName, anchor="w", bd=2)
label_format(songNameLabel, 40, False)
songNameLabel.grid(row=0, column=0, sticky="w")

btnFrame = Frame(root)
btnFrame.grid(row=3, column=0)

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


logo = ImageTk.PhotoImage(Image.open("assets/cherry.jpg"))
labelLogo = Label(image=logo, bd=0)
labelLogo.bind("<Button-1>", end_app)
labelLogo.place(relx=0.9, rely=0.4, anchor="center")
#labelLogo.grid(sticky=E, padx=5, pady=5)

screenBuild()

root.mainloop()
