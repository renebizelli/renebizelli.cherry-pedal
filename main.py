from tkinter import *
from models.song import Song
from models.audio import Audio
from models.band import Band
from services.song_service import Song_Service
from services.source_service import Source_Service
from apps.app_painel import Painel
from apps.app_setup import Setup
import keyboard


def song_forward_click(args):
    song_service.forward()
    song_changed()


def song_backward_click(args):
    song_service.backward()
    song_changed()


def song_changed():
    painel.play_indicator(False)
    song_current = song_service.current()
    if song_current is not None:
        painel.song_drawn(song_current)
        painel.autoforward_drawn(song_current)
        painel.audios_drawn(song_current)


def audio_forward_click(args):
    song_service.forwardAudio()
    painel.play_indicator(False)
    audio_changed()


def audio_changed():
    song_current = song_service.current()
    if song_current is not None:
        painel.audios_drawn(song_current)


def play_click(args):
    song_service.play()


def stop_click(args):
    song_service.stop()


def callback(command):
    if command == 'AUDIO_STARTS':
        painel.play_indicator(True)
        print("Start")
    elif command == 'AUDIO_ENDS':
        painel.play_indicator(False)
        print("End")

    audio_changed()


def band_selected(band: Band):
    print(">>>", band)
    songs = source.songs(band)
    song_service.set_songs(songs)
    song_current = song_service.current()
    if song_current is not None:
        painel.redrawn(band, song_current)


def band_click(args):
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

container = Frame(root, bg="black")
container.pack(side="left", fill="both", expand=True)
container.grid_columnconfigure(0, weight=1, pad=0, minsize=300)
container.grid_columnconfigure(1, weight=1, pad=0, minsize=300)
container.grid_columnconfigure(2, weight=1, pad=0, minsize=200)

song_service = Song_Service(callback)

source = Source_Service()

bands = source.bands()
songs = []

painel = Painel(root, container, band_click)
setup = Setup(root, container, bands, band_selected)
setup.redrawn()

keyboard.on_press_key('down arrow', audio_forward_click)
keyboard.on_press_key('left arrow', song_backward_click)
keyboard.on_press_key('right arrow', song_forward_click)
keyboard.on_press_key('space', play_click)
keyboard.on_press_key('esc', stop_click)

root.mainloop()
