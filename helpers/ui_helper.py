import tkinter as tk
from PIL import ImageTk, Image
from models.song import Song


class UI_Helper:

    def __init__(self, root):

        self._root = root
        self._audiosLabel = []

        self._song_autoforward = tk.StringVar()
        self._songAutoForwardLabel = tk.Label(
            root, textvariable=self._song_autoforward)
        self._songAutoForwardLabel.grid(
            row=2, column=2, sticky="e", padx=0, pady=0, ipadx=0, ipady=0)
        self.__style__auto_forward()

    def redrawn(self, song: Song):
        self.__band__()
        self.__song__(song)
        self.__audios__(song)

    def __band__(self):
        self._bandName = tk.StringVar()
        self._bandName.set("SuperBojo")
        self._bandNameLabel = tk.Label(
            self._root, textvariable=self._bandName, anchor="nw")
        self._bandNameLabel.grid(
            row=0, column=0, columnspan=3, sticky="new", padx=0, pady=0, ipadx=0, ipady=0)
        self._bandNameLabel.configure(
            bg="gray", fg="black", anchor="c", padx=0, pady=0, font=self.__font__(15))

    def __song__(self, song: Song):
        self._songNameLabel = tk.Label(
            self._root, text=song.name, anchor="nw")
        self.label_format(self._songNameLabel, 53, False)
        self._songNameLabel.grid(
            row=1, column=0, columnspan=3, sticky="new", padx=0, pady=0, ipadx=0, ipady=0)

    def __audios__(self, song: Song):

        self._audiosFrame = tk.Frame(self._root, bg="red", width=100)
        self._audiosFrame.grid(row=2, column=0, columnspan=2, rowspan=2, padx=10, sticky="nw")

        for l in self._audiosLabel:
            l.destroy()

        self._audiosLabel.clear()

        for i, audio in enumerate(song.audios):
            label = tk.Label(self._audiosFrame, text=audio.name)
            self.label_format(label, 30, audio.selected)
            label.place(x=0, y=i+10)
            label.pack(fill="both")
            self._audiosLabel.append(label)

    def __font__(self, fontSize):
        return ("Helvetica", fontSize)

    def __style__auto_forward(self):
        self._songAutoForwardLabel.configure(
            bg="yellow", fg="black", width=20, anchor="c",  font=self.__font__(15))

    def label_format(self, label: tk.Label, fontSize: int, selected: bool):

        bgColor = "blue" if selected else "black"
        fgColor = "white" if selected else "yellow"

        label.configure(fg=fgColor, bg=bgColor,
                        font=self.__font__(fontSize), anchor='nw')

    def window_configure(self):

        self._root.title('Cherry')
        #self._root.overrideredirect(True)

        self._root.grid_columnconfigure(0, weight=1, pad=0)
        self._root.grid_columnconfigure(1, weight=1, pad=0)
        self._root.grid_columnconfigure(2, weight=1, pad=0)

        window_width = 800
        window_height = 400
        self._root.geometry("{}x{}".format(window_width, window_height))
       ## self._root.resizable(False, False)

        self._root.configure(bg='black', padx=0, pady=0)

        screen_width = self._root.winfo_screenwidth()
        screen_height = self._root.winfo_screenheight()
        position_top = int(screen_height/2 - window_height/2)
        position_right = int(screen_width / 2 - window_width/2)
        self._root.geometry(
            f'{window_width}x{window_height}+{position_right}+{position_top}')

    def band_button(self, event):
        pass

    def cherry_button(self, click):
        img = Image.open("assets/cherry.jpg")
        img.thumbnail((180, 180), Image.ANTIALIAS)
        self._root.logo = ImageTk.PhotoImage(img)
        labelLogo = tk.Label(image=self._root.logo, bd=0,
                             bg="black", anchor="e")
        labelLogo.bind("<Button-1>", click)
        labelLogo.grid(row=3, column=2, sticky="sew", padx=70, pady=40)

    def set_song_autoforward(self, song: Song):
        auto = 'Auto' if song.autoforward else 'Manual'
        self._song_autoforward.set(auto)
