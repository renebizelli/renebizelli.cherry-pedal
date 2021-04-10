from tkinter import Frame, Label, Widget
from PIL import ImageTk, Image
from models.song import Song
from models.band import Band
from services.song_service import Song_Service
import keyboard
from screens.base_screen import Base_Screen
from threading import Timer
import RPi.GPIO as gpio
import time
import threading

class Painel_Screen(Base_Screen):

    def __init__(self, root, setup_drawn_event):
                
        super().__init__(root)

        self.__setup_drawn_click__ = setup_drawn_event

        self._audios_widget = []
        self._play_indicator_status = False
        self._song_service = Song_Service(self.__callback__)

        self._container = Frame(self._root)
        
        self._song_changed_bounce_control = True
        
    def set_screen_to_destroy(self, screen_to_destroy: Base_Screen):
        self._screen_to_destroy = screen_to_destroy

    def redrawn(self, band: Band, songs: []):

        self._screen_to_destroy.destroy()
        self._song_service.set_songs(songs)

        current_song = self._song_service.current()
        
        if current_song is not None:
            self.__container_drawn__()
            self.__band_drawn__(band)
            self.__song_drawn__(current_song)
            self.__audios_drawn__(current_song)
            self.__autoforward_drawn__(current_song)
            self.__play_indicator_drawn__()
            self.__cherry_drawn__()
            self.__keyboards__()
            
            self._song_changed_bounce_control = True
            
            self._gpio_thread = threading.Thread(target=self.__gpio__, daemon=True)            
            self._gpio_thread.start() 

    def __gpio__(self):
        
        self.gpio_init()        
        
        button_song_backward = 17 # roxo
        button_play = 22 #azul
        button_stop = 23 #amarelo
        button_song_forward = 24 #orange
        button_setup = 4 # white
        button_audio_forward = 27 #vermelho (confirmar)
        
        self.gpio_add_buttom(button_song_backward)
        self.gpio_add_buttom(button_play)
        self.gpio_add_buttom(button_stop)
        self.gpio_add_buttom(button_song_forward)
        self.gpio_add_buttom(button_setup)
        self.gpio_add_buttom(button_audio_forward)
        self.gpio_set_event()        
        
        while self._app_on:
            
            if gpio.event_detected(button_play):
                self.__play_click__(None)
                
            elif gpio.event_detected(button_stop):
                self.__stop_click__(None)                 
                
            elif gpio.event_detected(button_audio_forward):
                self.__audio_forward_click__(None)
                
            elif gpio.event_detected(button_song_forward):
                self.__song_forward_click__(None)
                
            elif gpio.event_detected(button_song_backward):
                self.__song_backward_click__(None)                  

            elif gpio.event_detected(button_setup):
                self.gpio_destroy()
                self.__setup_drawn_click__(None)            
        
        self.gpio_destroy()

    def destroy(self):
        self._app_on = False
        self._container.destroy()
        keyboard.unhook_all()

    def __container_drawn__(self):
        self._container = Frame(self._root, bg="black")
        self._container.pack(side="left", fill="both", expand=True)
        self._container.grid_columnconfigure(0, weight=1, pad=0, minsize=300)
        self._container.grid_columnconfigure(1, weight=1, pad=0, minsize=300)
        self._container.grid_columnconfigure(2, weight=1, pad=0, minsize=200)

    def __keyboards__(self):
        keyboard.on_press_key('down arrow', self.__audio_forward_click__)
        keyboard.on_press_key('left arrow', self.__song_backward_click__)
        keyboard.on_press_key('right arrow', self.__song_forward_click__)
        keyboard.on_press_key('space', self.__play_click__)
        keyboard.on_press_key('esc', self.__stop_click__)
        keyboard.on_press_key('f1', self.__setup_drawn_click__)

    def __band_drawn__(self, band: Band):
        band_name_label = Label(
            self._container, text=band.name.upper(), anchor="nw")
        band_name_label.grid(
            row=0, column=0, columnspan=3, sticky="new",
            padx=0, pady=0, ipadx=0, ipady=0)
        band_name_label.configure(
            bg="gray", fg="black", anchor="center", font=self.__font__(15))

    def __song_drawn__(self, song: Song):
        song_name_label = Label(self._container,
                                text=song.name.upper(),
                                fg="black", bg="gold",
                                font=self.__font__(40), anchor='nw')
        song_name_label.grid(
            row=1, column=0, columnspan=3, sticky="new", padx=0, pady=0)

    def __audios_drawn__(self, song: Song):
        
        try:
        
            for widget in self._audios_widget:
                widget.destroy()        

            self._audios_widget.clear()

            audios_frame = Frame(self._container, bg="black")
            audios_frame.grid(row=2, column=0, columnspan=2,
                              rowspan=3, sticky="new")

            self._audios_widget.append(audios_frame)
                    
            for i, audio in enumerate(song.audios):
                label = Label(audios_frame, text=audio.name.upper())
                self.__audio_item_format__(label, audio.selected)
                label.place(x=0, y=i+10)
                label.pack(fill="both", expand=True)
                self._audios_widget.append(label)
                
        except:
            print("Erro ao gerar lista de audios")
            
    def __autoforward_drawn__(self,  song: Song):
        mode = 'AUTO' if song.autoforward else 'MANUAL'
        song_auto_forward_label = Label(
            self._container, text=mode, bg="darkgreen", fg="white",
            anchor="center",  font=self.__font__(15))
        song_auto_forward_label.grid(
            row=2, column=2, sticky="ew", padx=5, pady=0, ipadx=0, ipady=0)

    def __play_indicator_drawn__(self):

        self._play_indicator_status = False

        self._play_indicator_label = Label(
            self._container, text="PLAYING", bg="black", fg="black",
            anchor="center",  font=self.__font__(15), pady=0)
        self._play_indicator_label.grid(
            row=3, column=2, sticky="ew", padx=5, pady=10, ipadx=0, ipady=0)

    def __play_indicator__(self, status: bool):
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

    def __cherry_drawn__(self):
        img = Image.open("assets/cherry.jpg")
        self._root.cherry = ImageTk.PhotoImage(img.resize((94, 220)))
        logo_label = Label(self._container, image=self._root.cherry, bd=0,
                           bg="black",  anchor="center")
        logo_label.bind("<Button-1>", self.__end__)
        logo_label.grid(row=4, column=2, sticky="ew", padx=0, pady=0)

    def __song_forward_click__(self, args):
        self._song_service.forward()
        self.__song_changed__()

    def __song_backward_click__(self, args):
        self._song_service.backward()
        self.__song_changed__()

            
    def __song_changed_after__(self):
        self.__play_indicator__(False)
        current_song = self._song_service.current()
        if current_song is not None:
            self._song_changed_bounce_control = True
            self.__song_drawn__(current_song)
            self.__autoforward_drawn__(current_song)
            self.__audios_drawn__(current_song)
            
    def __song_changed__(self):
        
        if self._song_changed_bounce_control:
            self._song_changed_bounce_control = False
            r = Timer(0.5, self.__song_changed_after__)
            r.start()

    def __audio_forward_click__(self, args):
        self._song_service.forwardAudio()
        self.__play_indicator__(False)
        self.__audio_changed__()

    def __audio_changed__(self):
        current_song = self._song_service.current()
        if current_song is not None:
            self.__audios_drawn__(current_song)

    def __play_click__(self, args):
        self._song_service.play()

    def __stop_click__(self, args):
        self._song_service.stop()

    def __callback__(self, command):
        
        if command == 'AUDIO_STARTS':
            self.__play_indicator__(True)
        elif command == 'AUDIO_ENDS':
            self.__play_indicator__(False)
            self.__audio_changed__()

    def __end__(self, args):
        self._app_on = False
        exit()
