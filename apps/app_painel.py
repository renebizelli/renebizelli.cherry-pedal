from tkinter import Frame, Label, Widget
from PIL import ImageTk, Image
from models.song import Song
from models.band import Band


class Painel:

    def __init__(self, root, container: Frame, show_setup_event):
        self._root = root
        self._container = container
        self._show_setup_event = show_setup_event
        self._audios_widget = []
        self._play_indicator_status = False

    def redrawn(self, band: Band, song: Song):

        for w in self._container.winfo_children():
            w.destroy()

        self.band_drawn(band)
        self.song_drawn(song)
        self.audios_drawn(song)
        self.autoforward_drawn(song)
        self.__play_indicator_drawn__()
        self.__cherry_button__()

    def band_drawn(self, band: Band):
        band_name_label = Label(
            self._container, text=band.name.upper(), anchor="nw")
        band_name_label.grid(
            row=0, column=0, columnspan=3, sticky="new",
            padx=0, pady=0, ipadx=0, ipady=0)
        band_name_label.configure(
            bg="gray", fg="black", anchor="center", font=self.__font__(15))

        band_name_label.bind("<Button-1>", self._show_setup_event)

    def song_drawn(self, song: Song):
        song_name_label = Label(self._container,
                                text=song.name.upper(),
                                fg="black", bg="gold",
                                font=self.__font__(40), anchor='nw')
        song_name_label.grid(
            row=1, column=0, columnspan=3, sticky="new", padx=0, pady=0)

    def audios_drawn(self, song: Song):

        for widget in self._audios_widget:
            widget.destroy()

        self._audios_widget.clear()

        audios_frame = Frame(self._container, bg="red", width=100)
        audios_frame.grid(row=2, column=0, columnspan=2,
                          rowspan=3, sticky="new")

        self._audios_widget.append(audios_frame)

        for i, audio in enumerate(song.audios):
            label = Label(audios_frame, text=audio.name.upper())
            self.__audio_item_format__(label, audio.selected)
            label.place(x=0, y=i+10)
            label.pack(fill="both")
            self._audios_widget.append(label)

    def autoforward_drawn(self,  song: Song):
        mode = 'AUTO' if song.autoforward else 'MANUAL'
        song_auto_forward_label = Label(
            self._container, text=mode, bg="darkgreen", fg="white",
            width=20, anchor="center",  font=self.__font__(15))
        song_auto_forward_label.grid(
            row=2, column=2, sticky="e", padx=0, pady=0, ipadx=0, ipady=0)

    def __play_indicator_drawn__(self):

        self._play_indicator_status = False

        self._play_indicator_label = Label(
            self._container, text="PLAYING", bg="black", fg="black",
            width=20, anchor="center",  font=self.__font__(15), pady=0)
        self._play_indicator_label.grid(
            row=3, column=2, sticky="e", padx=0, pady=10, ipadx=0, ipady=0)

    def play_indicator(self, status: bool):
        self._play_indicator_status = status
        self.__play_blinker__(status)

    def __play_blinker__(self, on: bool):

        bg = "darkred" if on else "black"
        fg = "white" if on else "black"
        on = not on

        if self._play_indicator_status:
            self._play_indicator_label.configure(bg=bg, fg=fg)
            self._play_indicator_label.after(
                500, lambda: self.__play_blinker__(on))
        else:
            self._play_indicator_label.configure(bg="black", fg="black")

    def __font__(self, fontSize):
        return ("Helvetica", fontSize)

    def __audio_item_format__(self, label: Label, selected: bool):

        label.configure(font=self.__font__(30), anchor='nw')

        if selected:
            label.configure(fg="white", bg="blue")
        else:
            label.configure(fg="yellow", bg="black")

    def __end__(self, args):
        exit()

    def __cherry_button__(self):
        img = Image.open("assets/cherry.jpg")
        self._root.logo = ImageTk.PhotoImage(img.resize((77, 180)))
        logo_label = Label(self._container, image=self._root.logo, bd=0,
                           bg="black",   anchor="center")
        logo_label.bind("<Button-1>", self.__end__)
        logo_label.grid(row=4, column=2, sticky="e", padx=70, pady=40)
