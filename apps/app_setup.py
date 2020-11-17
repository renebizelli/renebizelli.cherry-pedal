from models.band import Band
import tkinter as tk


class Setup:

    def __init__(self, container, bands, band_selected):
        self._container = container
        self._bands = bands
        self._band_selected = band_selected

    def redrawn(self):

        for w in self._container.winfo_children():
            w.destroy()

        for band in self._bands:
            btn = tk.Button(self._container, text=band.name,
                            command=lambda band=band: self._band_selected(band))
            btn.pack()
