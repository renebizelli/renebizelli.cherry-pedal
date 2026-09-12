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

### Fase seguinte — build completo do app

(Dockerfile completo com CMake + SDL2 + PortAudio + libsndfile + libgpiod
será adicionado quando o projeto C++ estiver com código-fonte real nas
próximas fases do port.)

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
