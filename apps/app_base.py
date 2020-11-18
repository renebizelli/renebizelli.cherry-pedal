from tkinter import Frame
import abc


class AppBase(metaclass=abc.ABCMeta):

    def __init__(self, root):
        self._root = root

    @abc.abstractclassmethod
    def destroy(self):
        pass

    def get_container(self):
        container = Frame(self._root, bg="black")
        container.pack(side="left", fill="both", expand=True)
        container.grid_columnconfigure(0, weight=1, pad=0, minsize=300)
        container.grid_columnconfigure(1, weight=1, pad=0, minsize=300)
        container.grid_columnconfigure(2, weight=1, pad=0, minsize=200)
        return container
