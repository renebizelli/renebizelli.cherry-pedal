from models.band import Band
import tkinter as tk


class Setup:

    def __init__(self, root, bands, band_selected):
        self._root = root
        self._bands = bands
        self._band_selected = band_selected
        self._container = tk.Frame(
            self._root, bg="gold")

    def redrawn(self):
        self._container.pack(side="left", fill=tk.BOTH, expand=1)

        for band in self._bands:
            btn = tk.Button(self._container, text=band.name,
                            command=self._band_selected)
            btn.pack()
