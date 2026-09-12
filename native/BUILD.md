# Build do Cherry Pedal nativo (C++) para Raspberry Pi 4

## Opção 1 — Cross-build via Docker Buildx (emulação QEMU arm64)

Pré-requisito: Docker Desktop rodando, com suporte a `linux/arm64` (padrão no
Docker Desktop atual, via `binfmt`/QEMU).

### Fase 1 — validar o toolchain (hello world)

```bash
cd native/docker
docker buildx build --platform linux/arm64 -f Dockerfile.arm64-build --target export -o out .
file out/hello
```

O `file out/hello` deve mostrar algo como:
`ELF 64-bit LSB executable, ARM aarch64 ...`

Para confirmar que roda de verdade, copie para o Pi e execute via SSH:

```bash
scp out/hello pi@<ip-do-pi>:/home/pi/hello
ssh pi@<ip-do-pi> ./hello
```

Saída esperada: `cherry-pedal native toolchain OK (arm64)`

### Desenvolvimento — testar a lógica pura (sem SDL2/GPIO/áudio real)

Para os módulos que não dependem de hardware (`SourceService` e afins), usar
a imagem de dev nativa do host (sem QEMU, muito mais rápida) em vez da
imagem arm64:

```bash
docker build -t cherry-native-dev -f native/docker/Dockerfile.dev native/docker

docker run --rm -v "<caminho-absoluto-do-repo>:/repo" -w /repo/native \
    cherry-native-dev bash -c \
    "cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build -j4 && ./build/test_source_service"
```

No Windows (Git Bash), prefixe o `docker run` com `MSYS_NO_PATHCONV=1` para
o caminho do container (`/repo`, `-w /repo/native`) não ser reescrito como
caminho do Windows.

### Motor de áudio (PortAudio) — pendente de validação no Pi

`PortAudioPlayer`/`PortAudioChannel` já compilam na imagem de dev (PortAudio
+ libsndfile instalados via apt), mas **abrem um dispositivo de áudio real**
ao construir o canal compartilhado — não há como validar reprodução de fato
sem hardware de áudio. Nenhum teste automatizado os executa; isso fica para
o teste manual no Pi (Fase de verificação "motor de áudio" do plano).

### GPIO (libgpiod) — pendente de validação no Pi

`GpiodEventSource`/`GpiodButtonFactory` compilam na imagem de dev (libgpiod
instalado via apt), mas **abrem um chip GPIO real** (`/dev/gpiochipN`) —
sem um Raspberry Pi (ou outra placa com GPIO exposto), não há como testar
de fato. A lógica de debounce (`DebouncedButton`) e o agrupamento de botões
(`InputService`) são testados isoladamente com um `FakeGpioEventSource`,
sem depender de hardware; a leitura real dos footswitches fica para o
teste manual no Pi.

### Fase seguinte — build completo do app

(Dockerfile completo com CMake + SDL2 + libgpiod será adicionado quando a UI
e a integração com GPIO forem implementadas nas próximas fases do port.)

## Opção 2 — Build direto no Raspberry Pi (fallback)

Caso o cross-build via QEMU se mostre inviável (lento demais ou com
problemas de libs), compilar direto no Pi via SSH:

```bash
ssh pi@<ip-do-pi>
sudo apt update
sudo apt install -y g++ cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
    portaudio19-dev libsndfile1-dev libgpiod-dev nlohmann-json3-dev

cd ~/renebizelli.cherry-pedal/native
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

## Deploy (ambas as opções)

Copiar para o Pi, junto do binário:

- `bands/` (arquivos WAV)
- `assets/` (imagens)
- `source.json`

```bash
rsync -av --exclude 'native' ./ pi@<ip-do-pi>:/home/pi/renebizelli.cherry-pedal/
```
