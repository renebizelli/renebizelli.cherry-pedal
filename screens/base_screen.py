from tkinter import Frame
import abc


class Base_Screen(metaclass=abc.ABCMeta):

    def __init__(self, root):
        self._root = root

    @abc.abstractclassmethod
    def destroy(self):
        pass
