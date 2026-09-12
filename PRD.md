### PRD: Cherry Pedal Disparador Grafico de Audios WAV por GPIO

Versao: 1.0
Data: 2026-08-06
Responsavel: Rene Bizelli

---

### Resumo

Cherry Pedal e uma aplicacao C++ com interface grafica fullscreen para Raspberry Pi, usada para selecionar bandas, musicas e audios WAV e disparar samples por footswitches fisicos conectados aos GPIOs. O projeto deve manter a tela grafica como parte essencial da experiencia, mas o motor de audio e entrada fisica deve priorizar baixa latencia, confiabilidade e operacao em palco.

---

### Contexto e problema

Publico-alvo
- Musicos e operadores que precisam disparar samples, introducoes, vinhetas e backing tracks durante apresentacoes.
- Bandas que precisam de um equipamento dedicado, previsivel e operavel por footswitch.

Cenarios de uso chave
- Selecionar uma banda no painel grafico.
- Navegar entre musicas e audios associados.
- Disparar o audio selecionado por GPIO ou teclado.
- Interromper a reproducao imediatamente por botao dedicado.
- Operar o equipamento em fullscreen no Raspberry Pi.

Onde essa feature sera implantada
- Sistema existente Cherry Pedal, executando em Raspberry Pi com C++, interface grafica SDL2, PortAudio para audio e libgpiod para leitura dos GPIOs.

Problemas priorizados
- Latencia perceptivel no disparo de audio, com impacto alto em uso musical ao vivo.
- Fragilidade operacional por arquivos ausentes ou caminhos absolutos, com impacto alto no boot e na apresentacao.
- Acoplamento tecnico entre UI, GPIO e player de audio, com impacto medio na manutencao.
- Necessidade de manter uma UI visual para selecao de bancos, musicas e audios, com impacto alto na usabilidade.

---

### Objetivos e metricas

| Objetivo | Metrica | Meta |
| --- | --- | --- |
| Disparar audios de forma musicalmente aceitavel | Latencia percebida entre acionamento do footswitch e inicio do audio | Hipotese: menor que 50 ms em Raspberry Pi configurado |
| Evitar disparos duplicados por contato mecanico | Eventos duplicados percebidos por acionamento | Zero disparos duplicados perceptiveis usando debounce de 30 ms |
| Evitar falhas tardias por arquivos ausentes | Arquivos WAV configurados validados na inicializacao | 100 por cento dos audios validados antes de abrir o painel |
| Manter operacao visual clara | Acoes principais disponiveis no painel | Banda, musica, audio selecionado, modo auto/manual e status de play visiveis |

---

### Escopo

Incluso
- Interface grafica fullscreen em SDL2.
- Selecao de banda por tela inicial.
- Painel de musicas e audios por banda.
- Disparo de audio por teclado e GPIO.
- Stop dedicado por teclado e GPIO.
- Leitura de GPIO via libgpiod com pull-up e debounce.
- Reproducao WAV por PortAudio com preload por musica.
- Uso de um canal de reproducao por vez.
- Validacao de arquivos de audio na inicializacao.
- Configuracao de bandas, musicas e audios via `source.json`.
- Caminhos relativos ao projeto para audios e assets.

Fora de escopo
- Modo headless sem interface grafica.
- Logs estruturados.
- Interface web de configuracao.
- Upload de WAVs pela aplicacao.
- Suporte a MIDI.
- Controle de volume por UI.
- Script offline para analisar e normalizar loudness dos WAVs.
- Loop, fade-in e fade-out.
- Dois canais de saida independentes.

---

### Requisitos funcionais

#### FR-001 Selecionar banda
A aplicacao deve exibir as bandas configuradas e permitir selecionar uma delas.

**Fluxo principal**
- O app inicia em fullscreen.
- O sistema carrega `source.json`.
- O sistema valida os arquivos WAV configurados.
- A tela inicial exibe as bandas disponiveis.
- O usuario navega entre bandas por teclado ou GPIO.
- O usuario confirma a banda selecionada.

**Fluxos alternativos e excecoes**
- Se a banda tiver logo em `assets/bands`, o logo deve ser exibido.
- Se nao houver logo, a banda pode ser exibida como botao textual.

**Erros previstos**
- Arquivo de logo ausente.
- Banda configurada sem lista de musicas correspondente.

**Prioridade:** alta

---

#### FR-002 Exibir painel da banda
A aplicacao deve abrir um painel com nome da banda, musica atual, audios da musica, modo auto/manual e indicador de reproducao.

**Fluxo principal**
- O usuario seleciona uma banda.
- O sistema carrega as musicas dessa banda.
- O painel exibe a primeira musica.
- O painel destaca o audio atual.
- O painel exibe se a musica esta em modo AUTO ou MANUAL.

**Fluxos alternativos e excecoes**
- Se a banda nao tiver musicas, o painel nao deve tentar inicializar player.

**Erros previstos**
- Lista de audios vazia em uma musica.
- Erro ao renderizar imagem ou lista de audios.

**Prioridade:** alta

---

#### FR-003 Navegar entre musicas
A aplicacao deve permitir avancar e voltar entre musicas da banda selecionada.

**Fluxo principal**
- O usuario aciona proxima musica por teclado ou GPIO.
- O sistema para o audio atual, se houver.
- O sistema seleciona a proxima musica.
- O sistema pre-carrega os audios da nova musica.
- O painel atualiza musica, audios e modo auto/manual.

**Fluxos alternativos e excecoes**
- Ao avancar apos a ultima musica, o sistema volta para a primeira.
- Ao voltar antes da primeira musica, o sistema vai para a ultima.

**Erros previstos**
- Audio ausente em arquivo fisico.
- Falha ao inicializar o player de audio.

**Prioridade:** alta

---

#### FR-004 Navegar entre audios da musica
A aplicacao deve permitir avancar entre os audios associados a musica atual.

**Fluxo principal**
- O usuario aciona proximo audio por teclado ou GPIO.
- O sistema para o audio atual, se houver.
- O sistema seleciona o proximo audio.
- O painel atualiza o destaque visual.

**Fluxos alternativos e excecoes**
- Ao avancar apos o ultimo audio, o sistema volta para o primeiro.

**Erros previstos**
- Musica sem audios.

**Prioridade:** alta

---

#### FR-005 Reproduzir audio selecionado
A aplicacao deve reproduzir imediatamente o audio selecionado.

**Fluxo principal**
- O usuario aciona play por teclado ou GPIO.
- O sistema interrompe qualquer reproducao ativa no canal principal.
- O sistema executa play do WAV ja carregado em memoria.
- O painel exibe o indicador de reproducao.

**Fluxos alternativos e excecoes**
- Se outro audio estiver tocando, ele deve ser interrompido antes do novo audio iniciar.
- Se a musica estiver em modo AUTO, ao fim do audio o sistema deve avancar para o proximo audio.

**Erros previstos**
- Mixer indisponivel.
- Interface de audio ausente ou nao configurada.

**Prioridade:** alta

---

#### FR-006 Interromper reproducao
A aplicacao deve permitir interromper imediatamente qualquer audio em reproducao.

**Fluxo principal**
- O usuario aciona stop por teclado ou GPIO.
- O sistema para o canal de audio.
- O painel remove o indicador de reproducao.

**Fluxos alternativos e excecoes**
- Se nenhum audio estiver tocando, a acao nao deve gerar erro.

**Erros previstos**
- Falha no mixer durante parada.

**Prioridade:** alta

---

#### FR-007 Validar arquivos na inicializacao
A aplicacao deve validar todos os arquivos WAV configurados antes de abrir o painel.

**Fluxo principal**
- O sistema le `source.json`.
- O sistema resolve os caminhos relativos ou absolutos.
- O sistema verifica a existencia de cada arquivo WAV.
- Se todos existirem, o app continua.
- Se algum estiver ausente, o app exibe o motivo e encerra.

**Fluxos alternativos e excecoes**
- Caminho `path` ausente deve assumir `bands`.
- Caminho relativo deve ser resolvido a partir da raiz do projeto.

**Erros previstos**
- Arquivo WAV ausente.
- JSON invalido.
- Arquivo `source.json` ausente.

**Prioridade:** alta

---

### Requisitos nao funcionais

Performance
- Hipotese: latencia percebida menor que 50 ms no Raspberry Pi, usando WAV pre-carregado em memoria via PortAudio.
- Stream configurado com frequencia 44100 Hz, 2 canais e buffer 256 frames.
- Arquivos WAV devem ser preferencialmente PCM sem compressao, 16 bits, 44,1 kHz.

Disponibilidade
- O app deve iniciar de forma previsivel no Raspberry Pi.
- Futuro: iniciar automaticamente via systemd apos boot do sistema.

Seguranca e autorizacao
- Nao ha autenticacao de usuario no escopo atual.
- A aplicacao e local e operada fisicamente no equipamento.

Observabilidade
- Logs estruturados estao fora de escopo por decisao do projeto.
- Erros de inicializacao devem ser exibidos em popup e console.

Confiabilidade e integridade de dados
- O app nao deve alterar arquivos de audio durante a execucao.
- A configuracao deve ser lida de `source.json`.
- Arquivos ausentes devem ser detectados antes da tela operacional.

Compatibilidade e portabilidade
- Deve rodar em Raspberry Pi com Linux (Raspberry Pi OS baseado em Debian Bookworm).
- Deve continuar compilavel/executavel em ambiente de desenvolvimento sem GPIO real (via backends de teste, ver `BUILD.md`).
- Caminhos de audio e assets devem ser relativos ao projeto sempre que possivel.

Compliance
- Nao aplicavel no escopo atual.

Acessibilidade no frontend consumidor
- Interface deve ser legivel em fullscreen, com alto contraste e indicacao visual clara de musica, audio selecionado e status de reproducao.

---

### Arquitetura e abordagem

Abordagem
- Manter UI grafica fullscreen como experiencia principal.
- Isolar UI, entrada fisica, navegacao de sessao e reproducao de audio em componentes separados.
- Usar preload de audio por musica para reduzir latencia no disparo.

Componentes (C++, ver `src/`)
- `main.cpp` / `Application`: composicao da aplicacao (raiz de composicao), inicializacao, validacao, loop principal e troca de telas.
- `SourceService`: leitura do `source.json`, resolucao de caminhos e validacao de audios.
- `SetupScreen`: selecao de banda (SDL2).
- `PainelScreen`: painel operacional de musica, audio, play e stop (SDL2).
- `InputService` / `DebouncedButton` / `GpiodEventSource`: integracao com GPIO via libgpiod, com debounce por software.
- `ActionQueue`: repassa eventos de GPIO (thread de fundo) para a thread principal antes de mexer em tela/estado.
- `PedalController`: fachada de acoes da sessao.
- `SongService`: navegacao entre musicas.
- `AudioService`: navegacao e preload dos audios da musica atual.
- `PlayerService`: orquestracao de play, stop e eventos de audio, com `PlaybackSequencer` para coordenar qual reproducao e a atual.
- `PortAudioPlayer` / `PortAudioChannel`: implementacao concreta com PortAudio e um unico stream persistente (equivalente ao `Channel(0)` do pygame).
- Modelos `Band`, `Song` e `Audio`: entidades de configuracao imutaveis (structs).

Integracoes
- GPIO fisico via `libgpiod`.
- Audio via PortAudio + libsndfile (decodificacao WAV) e stack de audio do Raspberry Pi (ALSA).
- UI via SDL2 + SDL2_image + SDL2_ttf.
- Arquivos locais WAV.
- Assets locais JPG.

> Nota: o projeto foi originalmente prototipado em Python/Tkinter/pygame e
> reescrito em C++ para reduzir e estabilizar a latencia de disparo (ver
> decisao "Reescrever em C++" abaixo). O historico Python permanece no
> git da branch `feat/refactory-ia`.

### Decisoes e trade-offs

#### Decisao: manter interface grafica
- **Justificativa:** A selecao visual de bandas, musicas e audios e essencial para o uso do projeto.
- **Trade-off:** O sistema depende de ambiente grafico no Raspberry, portanto nao segue o requisito headless puro.

#### Decisao: usar PortAudio com buffer pre-carregado em memoria
- **Justificativa:** Decodificar o WAV uma vez (preload) e entregar o PCM pronto ao canal compartilhado evita qualquer decodificacao no caminho de disparo, reduzindo latencia.
- **Trade-off:** Consome mais memoria quando comparado a streaming.

#### Decisao: usar um unico canal de reproducao
- **Justificativa:** Simplifica a politica de palco e garante um audio por vez.
- **Trade-off:** Nao permite sobreposicao de samples.

#### Decisao: usar `libgpiod`
- **Justificativa:** API padrao atual do Raspberry Pi OS para GPIO (sysfs GPIO esta deprecado); debounce implementado em software (`DebouncedButton`), equivalente ao `bounce_time` do antigo `gpiozero.Button`.
- **Trade-off:** Introduz dependencia direta de `libgpiod` no ambiente de producao.

#### Decisao: reescrever em C++ (SDL2 + PortAudio + libgpiod)
- **Justificativa:** A versao inicial em Python/Tkinter/pygame tinha overhead de interpretador e contencao de GIL entre a thread de deteccao de fim de audio e a UI/GPIO, alem de uma camada extra (pygame/SDL2) sobre o ALSA — tudo somando latencia e jitter no requisito mais critico do projeto (disparo musicalmente aceitavel).
- **Trade-off:** Maior esforco de desenvolvimento e manutencao por reescrita completa; exige builds nativos (cross-build via Docker Buildx com emulacao QEMU arm64, ou build direto no Pi) em vez de um interpretador portavel.

#### Decisao: nao implementar logs estruturados agora
- **Justificativa:** Reduz complexidade inicial e atende a decisao do projeto.
- **Trade-off:** Diagnostico historico fica limitado a erros exibidos no boot e console.

#### Decisao futura: normalizar audios fora do app
- **Justificativa:** Um script offline pode nivelar o volume percebido dos WAVs antes do uso em palco sem adicionar processamento no momento do disparo.
- **Trade-off:** Exige etapa previa de preparacao do acervo e pode gerar copias normalizadas dos arquivos.

---

### Dependencias

#### Tecnica: C++17 / CMake
Linguagem e sistema de build principal da aplicacao.

#### Tecnica: SDL2, SDL2_image, SDL2_ttf
Interface grafica fullscreen, carregamento de imagens (logos, seletor) e texto.

#### Tecnica: PortAudio
Reproducao de audio de baixa latencia via stream persistente.

#### Tecnica: libsndfile
Decodificacao de arquivos WAV para PCM.

#### Tecnica: libgpiod
Leitura de GPIOs com pull-up; debounce implementado em software na aplicacao.

#### Tecnica: nlohmann-json
Leitura do `source.json`.

#### Externa: interface de audio
Saida de audio USB, HAT, HDMI ou saida analogica configurada no Raspberry.

---

### Riscos e mitigacao

#### Latencia perceptivel no disparo
- **Probabilidade:** media
- **Impacto:** Alto em uso musical ao vivo.
- **Mitigacao:**
  - Usar WAV PCM.
  - Usar PortAudio com stream persistente (sem overhead de abrir/fechar por disparo).
  - Pre-carregar audios por musica.
  - Testar buffers 128, 256 e 512 no Raspberry.
- **Plano de contingencia:** Ajustar buffer do stream ou avaliar biblioteca de audio alternativa.

#### Consumo de memoria com audios longos
- **Probabilidade:** media
- **Impacto:** Pode causar lentidao ou falha em musicas com muitos audios longos.
- **Mitigacao:**
  - Pre-carregar apenas os audios da musica atual.
  - Validar uso com arquivos reais.
- **Plano de contingencia:** Usar streaming em vez de preload total apenas para backing tracks longas.

#### Audios com volumes percebidos muito diferentes
- **Probabilidade:** media
- **Impacto:** Pode exigir ajuste manual na mesa ou causar diferenca desconfortavel entre samples.
- **Mitigacao:**
  - Futuro: criar script offline para analisar loudness dos WAVs.
  - Futuro: gerar relatorio dos arquivos com maior diferenca de volume percebido.
  - Futuro: normalizar copias dos WAVs para um alvo definido, como hipotese inicial `-16 LUFS` ou `-18 LUFS`.
- **Plano de contingencia:** Ajustar manualmente os arquivos de audio antes do show.

#### Erro de audio no Raspberry
- **Probabilidade:** media
- **Impacto:** App pode iniciar, mas nao reproduzir audio.
- **Mitigacao:**
  - Configurar interface de audio do sistema.
  - Testar ALSA/PipeWire antes da apresentacao.
  - Exibir erros de inicializacao quando ocorrerem.
- **Plano de contingencia:** Ajustar configuracao de audio do Raspberry ou trocar a saida fisica.

#### GPIO com disparos duplicados
- **Probabilidade:** media
- **Impacto:** Pode iniciar ou trocar audio indevidamente.
- **Mitigacao:**
  - Usar `libgpiod` com debounce por software de 30 ms (`DebouncedButton`).
  - Testar footswitches reais.
- **Plano de contingencia:** Aumentar a janela de debounce conforme o hardware.

#### Arquivo WAV ausente ou caminho incorreto
- **Probabilidade:** media
- **Impacto:** Pode impedir apresentacao correta.
- **Mitigacao:**
  - Validar todos os audios na inicializacao.
  - Usar caminhos relativos ao projeto.
- **Plano de contingencia:** Exibir erro claro e corrigir `source.json` ou acervo.

---

### Criterios de aceitacao

- O app abre em fullscreen com tela de selecao de banda.
- O app valida todos os WAVs configurados antes de abrir o painel.
- Quando houver WAV ausente, o app exibe o motivo e encerra.
- O usuario consegue selecionar uma banda pela interface.
- O painel exibe banda, musica, audios, modo auto/manual e indicador de play.
- O usuario consegue navegar entre musicas por teclado e GPIO.
- O usuario consegue navegar entre audios por teclado e GPIO.
- O usuario consegue reproduzir o audio selecionado por teclado e GPIO.
- O usuario consegue interromper a reproducao por teclado e GPIO.
- O sistema usa PortAudio com um unico stream/canal compartilhado para reproducao (`PortAudioChannel`).
- O sistema usa `libgpiod` com pull-up e debounce por software de 30 ms (`DebouncedButton`).
- O sistema nao depende de caminhos absolutos para audios e assets.
- Futuro: script de normalizacao gera WAVs com volume percebido consistente antes da execucao do app.

---

### Testes e validacao

Tipos de teste obrigatorios
- Teste de carga de `source.json`.
- Teste de validacao de arquivos WAV.
- Teste de navegacao entre bandas, musicas e audios.
- Teste de reproducao e stop com player falso.
- Teste de GPIO com footswitch real no Raspberry.
- Teste de audio real no Raspberry com a interface de audio final.
- Teste de boot manual do app em fullscreen.
- Futuro: teste de normalizacao offline dos WAVs e comparacao de loudness antes/depois.
- Futuro: teste de inicializacao automatica via systemd.

Estrategia de validacao
- Validar localmente imports, carga de configuracao e regras sem depender de GPIO real.
- Validar no Raspberry Pi a leitura dos footswitches, debounce, saida de audio e latencia percebida.
- Testar pelo menos buffers 128, 256 e 512 se houver latencia ou instabilidade.
- Fazer ensaio com o acervo real de WAVs antes de uso em palco.
