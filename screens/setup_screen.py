import os
import tkinter as tk
from PIL import ImageTk, Image, ImageTk
from tkinter import Label, Frame
from models.band import Band
from screens.base_screen import Base_Screen
import keyboard


class Setup_Screen(Base_Screen):

    def __init__(self, root, bands, band_selected):
        self._root = root
        self._bands = bands
        self._index_selected = 0

        self._band_selector_widget = []

        self._band_selected = band_selected

    def set_screen_to_destroy(self, screen_to_destroy: Base_Screen):
        self._screen_to_destroy = screen_to_destroy

    def redrawn(self):

        self._index_selected = 0

        self._screen_to_destroy.destroy()
        self.__container_drawn__()
        self.__band_drawn__()
        self.__band_selector_drawn__()
        self.__keyboards__()
        self.__cherry_drawn__()

    def destroy(self):
        self._container.destroy()
        keyboard.unhook_all()

    def __container_drawn__(self):
        self._container = Frame(self._root, bg="black")
        self._container.pack(side="left", fill="both", expand=True)
        self._container.grid_columnconfigure(0, weight=1, pad=0, minsize=200)
        self._container.grid_columnconfigure(1, weight=1, pad=0, minsize=400)
        self._container.grid_columnconfigure(2, weight=1, pad=0, minsize=200)

        for i in range(len(self._bands)):
            self._container.grid_rowconfigure(i, weight=1, pad=0)

    def __band_drawn__(self):

        self._root.logo = {}

        for index, band in enumerate(self._bands):

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
                logo_label.grid(row=index, column=1)
            else:
                btn = tk.Button(self._container, text=band.name,
                                command=lambda band=band:
                                self._band_selected(band))
                btn.grid(row=index+1, column=1)

    def __band_selector_drawn__(self):

        for widget in self._band_selector_widget:
            widget.destroy()

        self._band_selector_widget.clear()

        for index in range(len(self._bands)):

            if index == self._index_selected:
                img = Image.open("assets/selector.jpg")
                self._root.selector = ImageTk.PhotoImage(img.resize((50, 50)))
                selector_label = Label(self._container,
                                       image=self._root.selector, bd=0,
                                       bg="black",  anchor="center")
                selector_label.grid(row=index, column=0)
                self._band_selector_widget.append(selector_label)

    def __band_forward_click__(self, args):
        size = len(self._bands)-1
        self._index_selected = self._index_selected + \
            1 if self._index_selected < size else 0
        self.__band_selector_drawn__()

    def __selector_click__(self, args):
        band = self._bands[self._index_selected]
        self._band_selected(band)

    def __end__(self, args):
        exit()

    def __keyboards__(self):
        keyboard.on_press_key('right arrow', self.__band_forward_click__)
        keyboard.on_press_key('space', self.__selector_click__)

    def __cherry_drawn__(self):
        img = Image.open("assets/cherry.jpg")
        self._root.cherry = ImageTk.PhotoImage(img.resize((94, 220)))
        logo_label = Label(self._container, image=self._root.cherry, bd=0,
                           bg="black",  anchor="center")
        logo_label.bind("<Button-1>", self.__end__)
        logo_label.grid(row=1, rowspan=len(self._bands),
                        column=3, sticky="w", padx=0, pady=0)
