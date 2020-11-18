import os
import tkinter as tk
from PIL import ImageTk, Image, ImageTk
from tkinter import Label
from models.band import Band
from apps.app_base import AppBase
from apps.app_base import AppBase


class Setup(AppBase):

    def __init__(self, root, bands, band_selected):
        self._root = root
        self._bands = bands

        self._band_selected = band_selected

    def set_app_to_destroy(self, app_to_destroy: AppBase):
        self._app_to_destroy = app_to_destroy

    def redrawn(self):

        self._app_to_destroy.destroy()

        self._container = self.get_container()

        self._root.logo = {}

        for band in self._bands:

            path_logo = "assets/bands/{}.jpg".format(band.id)

            if band.logo and os.path.isfile(path_logo):
                image_name = 'logo-band-{}'.format(band.id)
                img = Image.open(path_logo)
                self._root.logo[image_name] = ImageTk.PhotoImage(img)
                logo_label = tk.Label(
                    self._container, image=self._root.logo[image_name], bd=0,
                    anchor="center")
                logo_label.bind("<Button-1>", lambda event,
                                band=band: self._band_selected(band))
                logo_label.pack(pady=15)
            else:
                btn = tk.Button(self._container, text=band.name,
                                command=lambda band=band:
                                self._band_selected(band))
                btn.pack()

    def destroy(self):
        self._container.destroy()
