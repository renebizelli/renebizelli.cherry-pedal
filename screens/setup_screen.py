import os
import tkinter as tk
from PIL import ImageTk, Image
from tkinter import Label, Frame
from models.band import Band
from screens.base_screen import BaseScreen
from services.input_service import InputService
from services.path_service import asset_path


class SetupScreen(BaseScreen):

    def __init__(self, root, bands, band_selected):
        
        super().__init__(root)        
        
        self._bands = bands
        self._index_selected = 0

        self._band_selector_widget = []

        self._band_selected = band_selected

    def set_screen_to_destroy(self, screen_to_destroy: BaseScreen):
        self._screen_to_destroy = screen_to_destroy

    def redrawn(self):

        self._index_selected = 0

        self._screen_to_destroy.destroy()
        self._draw_container()
        self._draw_bands()
        self._draw_band_selector()
        self._bind_inputs()
        self._draw_exit_button()

    def destroy(self):
        self.stop_inputs()
        self._container.destroy()

    def _draw_container(self):
        self._container = Frame(self._root, bg="black")
        self._container.pack(side="left", fill="both", expand=True)
        self._container.grid_columnconfigure(0, weight=1, pad=0, minsize=200)
        self._container.grid_columnconfigure(1, weight=1, pad=0, minsize=400)
        self._container.grid_columnconfigure(2, weight=1, pad=0, minsize=200)

        for i in range(len(self._bands)):
            self._container.grid_rowconfigure(i, weight=1, pad=0)

    def _draw_bands(self):

        self._root.logo = {}

        for index, band in enumerate(self._bands):

            path_logo = asset_path('bands', '{}.jpg'.format(band.id))

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

    def _draw_band_selector(self):

        for widget in self._band_selector_widget:
            widget.destroy()

        self._band_selector_widget.clear()

        for index in range(len(self._bands)):

            if index == self._index_selected:
                img = Image.open(asset_path('selector.jpg'))
                self._root.selector = ImageTk.PhotoImage(img.resize((50, 50)))
                selector_label = Label(self._container,
                                       image=self._root.selector, bd=0,
                                       bg="black",  anchor="center")
                selector_label.grid(row=index, column=0)
                self._band_selector_widget.append(selector_label)

    def _handle_band_forward(self, args):
        size = len(self._bands)-1
        self._index_selected = self._index_selected + \
            1 if self._index_selected < size else 0
        self._draw_band_selector()

    def _handle_selector(self, args):
        band = self._bands[self._index_selected]
        self._band_selected(band)

    def _end(self, args):
        self.stop_inputs()
        self._root.destroy()

    def _bind_inputs(self):
        self.stop_inputs()
        self._input_service = InputService(
            keyboard_bindings={
                'right arrow': self._handle_band_forward,
                'space': self._handle_selector,
            },
            gpio_bindings={
                17: self._handle_band_forward, # roxo
                22: self._handle_selector, # azul
            }
        )
        self._input_service.start()

    def _draw_exit_button(self):
        exit_button = tk.Button(
            self._container,
            text="SAIR",
            command=lambda: self._end(None),
            bg="darkred",
            fg="white",
            activebackground="red",
            activeforeground="white",
            font=("Helvetica", 18, "bold"),
            bd=0,
        )
        exit_button.grid(row=len(self._bands), column=1, sticky="ew", padx=40, pady=12)


Setup_Screen = SetupScreen
