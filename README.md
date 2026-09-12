# Cherry Pedal

Aplicacao C++/SDL2 para disparar audios WAV por GPIO/teclado em um Raspberry
Pi 4, mantendo painel grafico fullscreen. Usa PortAudio para reproducao de
baixa latencia e libgpiod para leitura dos footswitches.

Veja [`BUILD.md`](BUILD.md) para instrucoes completas de build (cross-build
via Docker Buildx, build direto no Pi, verificacao visual headless da UI) e
deploy.

## Build rapido (no Raspberry Pi)

```bash
sudo apt update
sudo apt install -y g++ cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
    portaudio19-dev libsndfile1-dev libgpiod-dev nlohmann-json3-dev \
    fonts-dejavu-core

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

## Executar

A partir da raiz do projeto, pois os caminhos de `source.json`, `bands/` e
`assets/` sao relativos ao diretorio de trabalho:

```bash
./build/cherry_pedal
```

## Observacoes

- Os audios devem estar em WAV e configurados no `source.json`.
- O campo `path` do `source.json` pode ser relativo ao projeto, como `bands`.
- O app valida os arquivos de audio na inicializacao e exibe erro se algum
  WAV estiver ausente.
- A leitura dos GPIOs usa `libgpiod` com pull-up e debounce por software
  (30 ms).
- Leitura de GPIO normalmente exige pertencer ao grupo `gpio`
  (`sudo usermod -aG gpio $USER`, requer novo login) ou rodar como root.
