# Troubleshooting: erro de audio ALSA no Raspberry Pi

Este documento descreve um erro comum de configuracao de audio no Raspberry Pi e uma possivel correcao para quando o sistema nao encontra o dispositivo de audio padrao.

## Sintoma

Ao iniciar a aplicacao ou testar audio, o ALSA pode exibir mensagens semelhantes a:

```text
ALSA lib confmisc.c:767:(parse_card) cannot find card '2'
ALSA lib conf.c:4568:(_snd_config_evaluate) function snd_func_card_driver returned error: No such file or directory
ALSA lib confmisc.c:392:(snd_func_concat) error evaluating strings
ALSA lib conf.c:4568:(_snd_config_evaluate) function snd_func_concat returned error: No such file or directory
ALSA lib confmisc.c:1246:(snd_func_refer) error evaluating name
ALSA lib conf.c:4568:(_snd_config_evaluate) function snd_func_refer returned error: No such file or directory
ALSA lib conf.c:5047:(snd_config_expand) Evaluate error: No such file or directory
ALSA lib pcm.c:2565:(snd_pcm_open_noupdate) Unknown PCM default
```

## Causa provavel

O sistema esta tentando usar uma placa de audio que nao existe ou nao esta disponivel como dispositivo padrao.

No erro acima, o ALSA tenta encontrar a placa `2`, mas ela nao esta configurada corretamente.

## Solucao recomendada

Edite ou crie o arquivo:

```text
/etc/asound.conf
```

Com o conteudo:

```text
defaults.pcm.card 1
defaults.ctl.card 1
```

Esses valores fazem o ALSA usar a placa de audio `1` como padrao para playback e controle.

## Alternativa

Tambem e possivel ajustar o arquivo global do ALSA:

```bash
sudo nano /usr/share/alsa/alsa.conf
```

Procure pelas linhas:

```text
defaults.ctl.card 0
defaults.pcm.card 0
```

Altere para:

```text
defaults.ctl.card 1
defaults.pcm.card 1
```

Salve o arquivo e reinicie a aplicacao.

## Observacoes

- O numero correto da placa pode variar conforme a interface de audio usada.
- Para listar os dispositivos de audio disponiveis, use:

```bash
aplay -l
```

- Prefira configurar a interface de audio USB ou HAT usada no palco como dispositivo padrao.

---

## Atualizacao (Raspberry Pi OS Bookworm): PipeWire ignora o `default` do ALSA

A partir do Bookworm, o Raspberry Pi OS roda **PipeWire** (`pipewire-pulse`)
como servidor de audio. Isso muda o diagnostico acima: mesmo com
`/etc/asound.conf` e `~/.asoundrc` apontando corretamente para a placa
certa (`defaults.pcm.card 1`, por exemplo), o dispositivo ALSA `default`
pode continuar saindo pela placa errada (ex.: HDMI) porque o PipeWire
intercepta o `default` via seu proprio plugin ALSA e usa o **sink padrao
do PipeWire**, nao a configuracao do ALSA.

**Sintoma:** `aplay -D default arquivo.wav` ou `speaker-test -D default`
nao produz som (ou sai pela saida errada), mas `aplay -D plughw:1,0
arquivo.wav` funciona normalmente — confirmando que o problema e o
roteamento do PipeWire, nao a placa/cabo/volume.

**Diagnostico:**

```bash
pactl list short sinks
pactl get-default-sink
wpctl status   # mostra as streams ativas e para onde estao roteadas
```

Se o sink padrao ou o roteamento apontar para a placa errada (ex.:
`alsa_output.platform-bcm2835_audio.stereo-fallback` = HDMI em vez de
`...platform-bcm2835_audio.2.stereo-fallback` = P2/fone):

```bash
pactl set-default-sink alsa_output.platform-bcm2835_audio.2.stereo-fallback
```

(`wpctl set-default <id>` tambem existe, mas apresentou `segmentation
fault` nos testes deste projeto — prefira `pactl`.)

Depois de trocar o sink padrao, **reinicie qualquer app ja rodando**
(inclusive o `cherry_pedal`) — conexoes ja abertas no PipeWire nao
migram sozinhas para o novo sink padrao.

Para ajustar o volume dessa saida especifica (independente do
`alsamixer`):

```bash
pactl set-sink-volume alsa_output.platform-bcm2835_audio.2.stereo-fallback 100%
```
