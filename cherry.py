#!/usr/bin/env python3

import sys
from tkinter import *
from tkinter import messagebox
from models.band import Band
from services.source_service import SourceService
from screens.painel_screen import PainelScreen
from screens.setup_screen import SetupScreen
from screens.splash_screen import SplashScreen

root = Tk()
root.title('Cherry')
# self._root.overrideredirect(True)
window_width = 800
window_height = 400

root.geometry("{}x{}".format(window_width, window_height))

root.attributes('-fullscreen', True)  
root.fullScreenState = False


# self._root.resizable(False, False)
root.configure(bg='black', padx=0, pady=0)
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()
position_top = int(screen_height/2 - window_height/2)
position_right = int(screen_width / 2 - window_width/2)
root.geometry(
    f'{window_width}x{window_height}+{position_right}+{position_top}')

def band_selected(band: Band):
    songs = source.songs(band)
    painel.redrawn(band, songs)


def band_click(args):
    setup.redrawn()


def show_startup_error(error):
    message = "Erro ao iniciar o Cherry:\n\n{}: {}".format(
        type(error).__name__,
        error
    )
    print(message)
    messagebox.showerror('Cherry', message)
    root.destroy()
    sys.exit(1)


try:
    source = SourceService()
    source.validate_audio_files()
    bands = source.bands()
except Exception as error:
    show_startup_error(error)

painel = PainelScreen(root, band_click)
setup = SetupScreen(root, bands, band_selected)
splash = SplashScreen(root)

painel.set_screen_to_destroy(setup)
setup.set_screen_to_destroy(painel)

splash.redrawn()
root.after(1500, lambda: (splash.destroy(), setup.redrawn()))

root.mainloop()
