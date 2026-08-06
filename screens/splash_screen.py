from tkinter import Frame, Label
from PIL import ImageTk, Image

from services.path_service import asset_path


class SplashScreen:

    def __init__(self, root):
        self._root = root
        self._container = None

    def redrawn(self):
        self.destroy()
        self._container = Frame(self._root, bg="black")
        self._container.pack(side="left", fill="both", expand=True)
        self._container.grid_rowconfigure(0, weight=1)
        self._container.grid_columnconfigure(0, weight=1)

        img = Image.open(asset_path('cherry.jpg'))
        self._root.splash_logo = ImageTk.PhotoImage(img.resize((150, 350)))

        logo_label = Label(
            self._container,
            image=self._root.splash_logo,
            bd=0,
            bg="black",
            anchor="center",
        )
        logo_label.grid(row=0, column=0, sticky="nsew")

    def destroy(self):
        if self._container is not None:
            self._container.destroy()
            self._container = None
