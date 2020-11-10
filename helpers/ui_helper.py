from tkinter import *


def label_format(label: Label, fontSize: int, selected: bool):

    bgColor = "gray" if selected else "black"
    fgColor = "black" if selected else "yellow"

    label.configure(fg=fgColor, bg=bgColor, font=("Helvetica", fontSize))
