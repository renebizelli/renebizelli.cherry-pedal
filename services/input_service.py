class InputService:

    def __init__(self, keyboard_bindings=None, gpio_bindings=None, gpio_bounce_time=0.03):
        self._keyboard_bindings = keyboard_bindings or {}
        self._gpio_bindings = gpio_bindings or {}
        self._gpio_bounce_time = gpio_bounce_time
        self._keyboard_hooks = []
        self._gpio_buttons = []
        self._running = False
        self._keyboard = self._load_keyboard()
        self._gpio_button_class = self._load_gpio_button()

    def start(self):
        self._running = True
        self._start_keyboard()
        self._start_gpio()

    def stop(self):
        self._running = False
        self._stop_keyboard()
        self._stop_gpio()

    def _load_keyboard(self):
        try:
            import keyboard
            return keyboard
        except ImportError:
            return None

    def _load_gpio_button(self):
        try:
            from gpiozero import Button
            return Button
        except ImportError:
            return None

    def _start_keyboard(self):
        if self._keyboard is None:
            return

        for key, callback in self._keyboard_bindings.items():
            hook = self._keyboard.on_press_key(key, callback)
            self._keyboard_hooks.append(hook)

    def _stop_keyboard(self):
        if self._keyboard is None:
            return

        for hook in self._keyboard_hooks:
            self._keyboard.unhook(hook)

        self._keyboard_hooks.clear()

    def _start_gpio(self):
        if self._gpio_button_class is None or len(self._gpio_bindings) == 0:
            return

        for pin, callback in self._gpio_bindings.items():
            button = self._gpio_button_class(
                pin,
                pull_up=True,
                bounce_time=self._gpio_bounce_time,
            )
            button.when_pressed = lambda callback=callback: callback(None)
            self._gpio_buttons.append(button)

    def _stop_gpio(self):
        for button in self._gpio_buttons:
            button.close()

        self._gpio_buttons.clear()


Input_Service = InputService
