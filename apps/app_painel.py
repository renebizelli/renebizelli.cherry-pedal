import tkinter as tk
from PIL import ImageTk, Image
from models.song import Song


class Painel:

    def __init__(self, root, container: tk.Frame, show_setup_event):
        self._root = root
        self._container = container
        self._show_setup_event = show_setup_event
        self._audios_widget = []

    def redrawn(self, song: Song, click_cherry):
        self._container.pack_forget()
        self.__band__()
        self.__song__(song)
        self.__audios__(song)
        self.__autoforward__(song)
        self.__cherry_button__(click_cherry)

    def __band__(self):
        band_name = tk.StringVar()
        band_name.set("SuperBojo")
        band_name_label = tk.Label(
            self._container, textvariable=band_name, anchor="nw")
        band_name_label.grid(
            row=0, column=0, columnspan=3, sticky="new",
            padx=0, pady=0, ipadx=0, ipady=0)
        band_name_label.configure(
            bg="gray", fg="black", anchor="c", padx=0,
            pady=0, font=self.__font__(15))

        band_name_label.bind("<Button-1>", self._show_setup_event)

    def __song__(self, song: Song):
        song_name_label = tk.Label(self._container, text=song.name)
        self.__label_format__(song_name_label, 53)
        song_name_label.grid(
            row=1, column=0, columnspan=3, sticky="new", padx=0, pady=0, ipadx=0, ipady=0)

    def __audios__(self, song: Song):

        for widget in self._audios_widget:
            widget.destroy()

        self._audios_widget.clear()

        audios_frame = tk.Frame(self._container, bg="black", width=100)
        audios_frame.grid(row=2, column=0, columnspan=2,
                          rowspan=2, padx=10, sticky="nw")

        self._audios_widget.append(audios_frame)

        for i, audio in enumerate(song.audios):
            label = tk.Label(audios_frame, text=audio.name)
            self.__audio_item_format__(label, audio.selected)
            label.place(x=0, y=i+10)
            label.pack(fill="both")
            self._audios_widget.append(label)

    def __autoforward__(self,  song: Song):
        mode = 'Auto' if song.autoforward else 'Manual'
        song_auto_forward_label = tk.Label(
            self._container, text=mode, bg="yellow", fg="black",
            width=20, anchor="center",  font=self.__font__(15))
        song_auto_forward_label.grid(
            row=2, column=2, sticky="e", padx=0, pady=0, ipadx=0, ipady=0)

    def __font__(self, fontSize):
        return ("Helvetica", fontSize)

    def __label_format__(self, label: tk.Label, fontSize: int,):
        label.configure(fg="yellow", bg="black",
                        font=self.__font__(fontSize), anchor='nw')

    def __audio_item_format__(self, label: tk.Label, selected: bool):
        size = 30
        self.__label_format__(label, size)

        if selected:
            label.configure(fg="white", bg="blue",
                            font=self.__font__(size))

    def __cherry_button__(self, click):
        img = Image.open("assets/cherry.jpg")
        img.thumbnail((180, 180), Image.ANTIALIAS)
        self._root.logo = ImageTk.PhotoImage(img)
        logo_label = tk.Label(self._container, image=self._root.logo, bd=0,
                              bg="black", anchor="e")
        logo_label.bind("<Button-1>", click)
        logo_label.grid(row=3, column=2, sticky="sew", padx=70, pady=40)
 