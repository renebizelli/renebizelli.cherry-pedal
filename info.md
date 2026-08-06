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
