from tkinter import Frame
import abc
import RPi.GPIO as gpio

class Base_Screen(metaclass=abc.ABCMeta):

    def __init__(self, root):
        self._root = root
        self._app_on = True
        
        self.buttons_gpio = []

    def gpio_add_buttom(self, pin:int):
        self.buttons_gpio.append(pin)

    def gpio_init(self):
        self._app_on = True
        gpio.setmode(gpio.BCM)
        
    def gpio_destroy(self):
        for button in self.buttons_gpio:
            gpio.remove_event_detect(button)
            
        self.buttons_gpio.clear()
        #gpio.cleanup()       
        
    def gpio_set_event(self):
        for button in self.buttons_gpio:
            gpio.setup(button, gpio.IN, pull_up_down=gpio.PUD_UP)
            gpio.add_event_detect(button, gpio.FALLING, bouncetime=250)

    @abc.abstractclassmethod
    def destroy(self):
        pass
