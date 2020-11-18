from tkinter import *
from models.band import Band
from services.source_service import Source_Service
from screens.painel_screen import Painel_Screen
from screens.setup_screen import Setup_Screen

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


def band_selected(band: Band):
    songs = source.songs(band)
    painel.redrawn(band, songs)


def band_click(args):
    setup.redrawn()


source = Source_Service()
bands = source.bands()

painel = Painel_Screen(root, band_click)
setup = Setup_Screen(root, bands, band_selected)

painel.set_screen_to_destroy(setup)
setup.set_screen_to_destroy(painel)

setup.redrawn()

root.mainloop()
