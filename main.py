from tkinter import *
from models.music import Music
from models.audio import Audio
from controllers.music_controller import Music_Controller
from helpers.ui_helper import label_format

root = Tk()
root.title('Cherry')
root.geometry("600x400")
root.resizable(False, False)

musics = [
    Music(1, 'Kaus Trem', False, [Audio('1', 'intro', 'audios/kaus-trem/kaus_1.wav'),
                           Audio('2', 'apito', 'audios/kaus-trem/kaus_2.wav')]),
    Music(1, 'Os tiras estão chegando', False, [
          Audio('1', 'tiras intro', '1.wav'), Audio('2', 'tiras 2', '2.wav')]),
    Music(1, 'Linhas de Nasca', False, [
          Audio('1', 'Linhas intro', '1.wav'), Audio('2', 'Linhas meio', '2.wav')])
]

labels = []

musicName = StringVar()
musicNameLabel = Label(root, textvariable=musicName)
musicNameLabel.grid(row=0, column=0)

label_format(musicNameLabel, 20, False)


def musicForwardButton_click():
    musicController.forward()
    screenBuild()


def audioForwardButton_click():
    musicController.forwardAudio()
    screenBuild()


def playButton_click():
    musicController.play()


def stopButton_click():
    musicController.stop()


def screenBuild():
    music = musicController.current()
    musicName.set(music.name)

    for l in labels:
        l.destroy()

    labels.clear()

    for i, audio in enumerate(music.audios):
        label = Label(root, text=audio.name)

        label_format(label, 16, audio.selected)

        label.grid(row=i + 2)
        labels.append(label)


def callback(command):
    if command == 'AUDIO_STARTS':
        print("Start")
    elif command == 'AUDIO_ENDS':
        screenBuild()

musicController = Music_Controller(musics, callback)

musicForwardButton = Button(root, text="Music >>",
                            command=musicForwardButton_click)
musicForwardButton.grid(row=1, column=0)

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
