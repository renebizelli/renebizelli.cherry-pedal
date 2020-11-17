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
    if song is not None:
        painel.redrawn(song, end_app)


def callback(command):
    if command == 'AUDIO_STARTS':
        print("Start")
    elif command == 'AUDIO_ENDS':
        screenBuild()

def end_app(args):
    exit()

def band_selected(band: Band):
    songs = source.songs(band)
    songService.set_songs(songs)
    screenBuild()

def show_setup(args):
    setup.redrawn()

root = Tk()
root.title('Cherry')
# self._root.overrideredirect(True)
window_width = 800
window_height = 400
root.geometry("{}x{}".format(window_width, window_height))
# self._root.resizable(False, False)
root.configure(bg='black', padx=0, pady=0)
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()
position_top = int(screen_height/2 - window_height/2)
position_right = int(screen_width / 2 - window_width/2)
root.geometry(
f'{window_width}x{window_height}+{position_right}+{position_top}')

container = Frame(root, bg="green")
container.pack(side="left", fill="both", expand=1)
container.grid_columnconfigure(0, weight=1, pad=0)
container.grid_columnconfigure(1, weight=1, pad=0)
container.grid_columnconfigure(2, weight=1, pad=0)
container.pack_forget()

l = Label(container, text="sdfsdfsdfsdfsdf")
l.pack()

songService = Song_Service(callback)

source = Source_Service()

bands = source.bands()
songs = []

painel = Painel(root, container, show_setup)
setup = Setup(container, bands, band_selected)
#setup.redrawn()


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
