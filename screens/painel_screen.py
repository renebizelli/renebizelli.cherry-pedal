from tkinter import Canvas, Frame, Label
from models.song import Song
from models.band import Band
from screens.base_screen import BaseScreen
from threading import Timer
from services.input_service import InputService
from services.audio_events import AUDIO_ENDS, AUDIO_STARTS
from services.pedal_controller import PedalController
from screens.splash_screen import SplashScreen


class PainelScreen(BaseScreen):

    def __init__(self, root, setup_drawn_event):
                
        super().__init__(root)

        self._setup_drawn_event = setup_drawn_event

        self._audios_widget = []
        self._play_indicator_status = False
        self._pedal_controller = None
        self._band_label = None
        self._mode_label = None
        self._song_label = None
        self._play_indicator_canvas = None
        self._hide_play_indicator_after_id = None
        self._blink_play_indicator_after_id = None
        self._play_indicator_blink_on = True
        self._loading_screen = None

        self._container = Frame(self._root)
        
        self._song_changed_bounce_control = True
        
    def set_screen_to_destroy(self, screen_to_destroy: BaseScreen):
        self._screen_to_destroy = screen_to_destroy
        
    def _redraw(self, band: Band, current_song: Song):
        self._destroy_loading_screen()
                
        self._draw_container()
        self._draw_band(band)
        self._draw_song(current_song)
        self._draw_audios(current_song, self._pedal_controller.current_audio())
        self._draw_autoforward(current_song)
        self._draw_play_indicator()
        self._bind_inputs()

        self._song_changed_bounce_control = True

    def redrawn(self, band: Band, songs: []):
        
        self._screen_to_destroy.destroy()
        self._draw_loading_screen()
        self._stop_pedal_controller()
        self._pedal_controller = PedalController(songs, self._handle_audio_event)
        current_song = self._pedal_controller.current_song()
        
        if current_song is not None:
            self._root.after(500, lambda: self._redraw(band, current_song))

    def destroy(self):
        self.stop_inputs()
        self._stop_pedal_controller()
        self._destroy_loading_screen()
        self._cancel_hide_play_indicator()
        self._cancel_blink_play_indicator()
        self._container.destroy()

    def _draw_loading_screen(self):
        self._destroy_loading_screen()
        self._loading_screen = SplashScreen(self._root)
        self._loading_screen.redrawn()
        self._root.update_idletasks()

    def _destroy_loading_screen(self):
        if self._loading_screen is not None:
            self._loading_screen.destroy()
            self._loading_screen = None

    def _stop_pedal_controller(self):
        if self._pedal_controller is not None:
            self._pedal_controller.stop()

    def _draw_container(self):
        self._cancel_hide_play_indicator()
        self._cancel_blink_play_indicator()
        self._container = Frame(self._root, bg="black")
        self._container.pack(side="left", fill="both", expand=True)
        self._audios_widget = []
        self._band_label = None
        self._mode_label = None
        self._song_label = None
        self._play_indicator_canvas = None
        self._hide_play_indicator_after_id = None
        self._blink_play_indicator_after_id = None
        self._play_indicator_blink_on = True
        self._container.grid_columnconfigure(0, weight=7, pad=0, minsize=520)
        self._container.grid_columnconfigure(1, weight=3, pad=0, minsize=220)
        self._container.grid_rowconfigure(0, weight=0, minsize=54)
        self._container.grid_rowconfigure(1, weight=0, minsize=115)
        self._container.grid_rowconfigure(2, weight=1, minsize=230)

    def _bind_inputs(self):
        self.stop_inputs()
        self._input_service = InputService(
            keyboard_bindings={
                'down arrow': self._handle_audio_forward,
                'left arrow': self._handle_song_backward,
                'right arrow': self._handle_song_forward,
                'space': self._handle_play,
                'esc': self._handle_stop,
                'f1': self._setup_drawn_event,
            },
            gpio_bindings={
                17: self._handle_song_backward, # roxo
                22: self._handle_play, # azul
                23: self._handle_stop, # amarelo
                24: self._handle_song_forward, # orange
                27: self._handle_audio_forward, # vermelho (confirmar)
            }
        )
        self._input_service.start()

    def _draw_band(self, band: Band):
        if self._band_label is not None:
            self._band_label.destroy()

        self._band_label = Label(
            self._container,
            text=band.name.upper(),
            bg="black",
            fg="white",
            anchor="w",
            font=self._font(34, bold=True),
            padx=16,
        )
        self._band_label.bind("<Button-1>", self._setup_drawn_event)
        self._band_label.grid(row=0, column=0, sticky="nsew")

    def _draw_song(self, song: Song):
        if self._song_label is not None:
            self._song_label.destroy()

        song_name = song.name.upper()
        self._song_label = Label(
            self._container,
            text=song_name,
            fg="yellow",
            bg="black",
            font=self._font(self._song_font_size(song_name), bold=True),
            anchor="center",
        )
        self._song_label.grid(row=1, column=0, columnspan=2, sticky="nsew")

    def _draw_audios(self, song: Song, current_audio):
        
        try:
        
            for widget in self._audios_widget:
                widget.destroy()        

            self._audios_widget.clear()

            audios_frame = Frame(self._container, bg="black")
            audios_frame.grid(row=2, column=0, sticky="nsew")
            audios_frame.grid_columnconfigure(0, weight=1)

            self._audios_widget.append(audios_frame)

            audio_font_size = self._audio_font_size(song.audios)
            audio_row_height = self._audio_row_height(len(song.audios))

            for i, audio in enumerate(song.audios):
                text = "{} {}".format(i + 1, audio.name.upper())
                if audio == current_audio:
                    text = "> " + text

                label = Label(audios_frame, text=text)
                self._format_audio_item(label, audio == current_audio)
                label.configure(font=self._font(audio_font_size, bold=True), padx=18)
                label.grid(row=i, column=0, sticky="nsew")
                audios_frame.grid_rowconfigure(i, weight=0, minsize=audio_row_height)
                self._audios_widget.append(label)
                
        except Exception as error:
            print("Erro ao gerar lista de audios: {}".format(error))
            
    def _draw_autoforward(self,  song: Song):
        mode = 'AUTO' if song.autoforward else 'MANUAL'
        if self._mode_label is not None:
            self._mode_label.destroy()

        self._mode_label = Label(
            self._container,
            text=mode,
            bg="black",
            fg="lime",
            anchor="e",
            font=self._font(30, bold=True),
            padx=16,
        )
        self._mode_label.grid(row=0, column=1, sticky="nsew")

    def _draw_play_indicator(self):

        self._play_indicator_status = False

        self._play_indicator_canvas = Canvas(
            self._container,
            bg="black",
            bd=0,
            highlightthickness=0,
            width=180,
            height=180,
        )
        self._play_indicator_canvas.grid(row=2, column=1, sticky="n")
        self._draw_status_circle(False, visible=False)

    def _set_play_indicator(self, status: bool):
        self._play_indicator_status = status
        if self._play_indicator_canvas is None:
            return

        self._cancel_hide_play_indicator()
        self._cancel_blink_play_indicator()

        if status:
            self._play_indicator_blink_on = True
            self._blink_play_indicator()
        else:
            self._draw_status_circle(False, visible=True)
            self._hide_play_indicator_after_id = self._play_indicator_canvas.after(
                5000,
                self._hide_play_indicator,
            )

    def _hide_play_indicator(self):
        self._hide_play_indicator_after_id = None

        if self._play_indicator_status:
            return

        self._draw_status_circle(False, visible=False)

    def _cancel_hide_play_indicator(self):
        if self._hide_play_indicator_after_id is None:
            return

        self._play_indicator_canvas.after_cancel(self._hide_play_indicator_after_id)
        self._hide_play_indicator_after_id = None

    def _blink_play_indicator(self):
        if not self._play_indicator_status:
            return

        color = "green" if self._play_indicator_blink_on else "darkgreen"
        self._draw_status_circle(True, visible=True, color=color)
        self._play_indicator_blink_on = not self._play_indicator_blink_on
        self._blink_play_indicator_after_id = self._play_indicator_canvas.after(
            500,
            self._blink_play_indicator,
        )

    def _cancel_blink_play_indicator(self):
        if self._blink_play_indicator_after_id is None:
            return

        if self._play_indicator_canvas is not None:
            self._play_indicator_canvas.after_cancel(self._blink_play_indicator_after_id)

        self._blink_play_indicator_after_id = None

    def _draw_status_circle(self, playing: bool, visible=True, color=None):
        self._play_indicator_canvas.delete("all")

        if not visible:
            return

        color = color or ("green" if playing else "darkred")
        self._play_indicator_canvas.create_oval(
            20,
            20,
            160,
            160,
            fill=color,
            outline=color,
        )

        if playing:
            self._play_indicator_canvas.create_polygon(
                70,
                55,
                70,
                125,
                125,
                90,
                fill="white",
                outline="white",
            )
        else:
            self._play_indicator_canvas.create_rectangle(
                62,
                62,
                118,
                118,
                fill="white",
                outline="white",
            )

    def _font(self, fontSize, bold=False):
        weight = "bold" if bold else "normal"
        return ("Helvetica", fontSize, weight)

    def _format_audio_item(self, label: Label, selected: bool):

        label.configure(anchor="w")

        if selected:
            label.configure(fg="white", bg="blue")
        else:
            label.configure(fg="yellow", bg="black")

    def _song_font_size(self, text):
        if len(text) > 28:
            return 34
        if len(text) > 20:
            return 42
        return 52

    def _audio_font_size(self, audios):
        if len(audios) >= 6:
            return 24
        if len(audios) >= 4:
            return 30
        return 36

    def _audio_row_height(self, audio_count):
        if audio_count >= 6:
            return 48
        if audio_count >= 4:
            return 62
        return 76

    def _handle_song_forward(self, args):
        self._pedal_controller.next_song()
        self._handle_song_changed()

    def _handle_song_backward(self, args):
        self._pedal_controller.previous_song()
        self._handle_song_changed()

            
    def _draw_song_changed(self):
        self._set_play_indicator(False)
        current_song = self._pedal_controller.current_song()
        if current_song is not None:
            self._song_changed_bounce_control = True
            self._draw_song(current_song)
            self._draw_autoforward(current_song)
            self._draw_audios(current_song, self._pedal_controller.current_audio())
            
    def _handle_song_changed(self):
        
        if self._song_changed_bounce_control:
            self._song_changed_bounce_control = False
            r = Timer(0.5, self._draw_song_changed)
            r.start()

    def _handle_audio_forward(self, args):
        self._pedal_controller.next_audio()
        self._set_play_indicator(False)
        self._draw_audio_changed()

    def _draw_audio_changed(self):
        current_song = self._pedal_controller.current_song()
        if current_song is not None:
            self._draw_audios(current_song, self._pedal_controller.current_audio())

    def _handle_play(self, args):
        self._pedal_controller.play()

    def _handle_stop(self, args):
        self._pedal_controller.stop()

    def _handle_audio_event(self, command):
        
        if command == AUDIO_STARTS:
            self._set_play_indicator(True)
        elif command == AUDIO_ENDS:
            self._set_play_indicator(False)
            self._draw_audio_changed()

    def _end(self, args):
        self.stop_inputs()
        exit()


Painel_Screen = PainelScreen
