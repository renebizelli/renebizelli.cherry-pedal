import tkinter as tk


def label_format(label: tk.Label, fontSize: int, selected: bool):

    bgColor = "blue" if selected else "black"
    fgColor = "white" if selected else "yellow"

    label.configure(fg=fgColor, bg=bgColor, font=(
        "Helvetica", fontSize), anchor='w')


def window_configure(root):

    root.title('Cherry')
    # root.overrideredirect(True)

    # root.grid_columnconfigure(0, weight=1)
    # root.grid_columnconfigure(1, weight=1)
    # root.grid_columnconfigure(2, weight=1)

    window_width = 800
    window_height = 400
    root.geometry("{}x{}".format(window_width, window_height))
    root.resizable(False, False)

    root.configure(bg='black')

    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    position_top = int(screen_height/2 - window_height/2)
    position_right = int(screen_width / 2 - window_width/2)
    root.geometry(
        f'{window_width}x{window_height}+{position_right}+{position_top}')
