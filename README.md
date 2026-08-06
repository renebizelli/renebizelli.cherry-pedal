# Cherry Pedal

Aplicacao Python/Tkinter para disparar audios WAV por GPIO/teclado em um Raspberry Pi, mantendo painel grafico fullscreen.

## Instalar no Raspberry

```bash
cd /home/pi/renebizelli.cherry-pedal
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

Se o `tkinter` nao estiver disponivel no Raspberry Pi OS:

```bash
sudo apt update
sudo apt install python3-tk
```

## Executar

```bash
source .venv/bin/activate
python cherry.py
```

## Observacoes

- Os audios devem estar em WAV e configurados no `source.json`.
- O campo `path` do `source.json` pode ser relativo ao projeto, como `bands`.
- O app valida os arquivos de audio na inicializacao e exibe erro se algum WAV estiver ausente.
- A leitura dos GPIOs usa `gpiozero.Button` com `pull_up=True` e `bounce_time=0.03`.
