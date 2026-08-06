### PRD: Cherry Pedal Disparador Grafico de Audios WAV por GPIO

Versao: 1.0
Data: 2026-08-06
Responsavel: Rene Bizelli

---

### Resumo

Cherry Pedal e uma aplicacao Python com interface grafica fullscreen para Raspberry Pi, usada para selecionar bandas, musicas e audios WAV e disparar samples por footswitches fisicos conectados aos GPIOs. O projeto deve manter a tela grafica como parte essencial da experiencia, mas o motor de audio e entrada fisica deve priorizar baixa latencia, confiabilidade e operacao em palco.

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
- Sistema existente Cherry Pedal, executando em Raspberry Pi com Python 3, interface grafica Tkinter, pygame para audio e gpiozero para leitura dos GPIOs.

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
- Interface grafica fullscreen em Tkinter.
- Selecao de banda por tela inicial.
- Painel de musicas e audios por banda.
- Disparo de audio por teclado e GPIO.
- Stop dedicado por teclado e GPIO.
- Leitura de GPIO via gpiozero com pull-up e debounce.
- Reproducao WAV por pygame.mixer.Sound com preload por musica.
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
- Hipotese: latencia percebida menor que 50 ms no Raspberry Pi, usando WAV pre-carregado com `pygame.mixer.Sound`.
- Mixer configurado com frequencia 44100 Hz, 16 bits, 2 canais e buffer 256.
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
- Deve rodar em Raspberry Pi com Python 3.
- Deve continuar importavel em ambiente de desenvolvimento sem GPIO real.
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

Componentes
- `cherry.py`: composicao da aplicacao, inicializacao, validacao e abertura das telas.
- `SourceService`: leitura do `source.json`, resolucao de caminhos e validacao de audios.
- `SetupScreen`: selecao de banda.
- `PainelScreen`: painel operacional de musica, audio, play e stop.
- `InputService`: integracao com teclado e gpiozero.
- `PedalController`: fachada de acoes da sessao.
- `SongService`: navegacao entre musicas.
- `AudioService`: navegacao e preload dos audios da musica atual.
- `PlayerService`: orquestracao de play, stop e eventos de audio.
- `PygameAudioPlayer`: implementacao concreta com `pygame.mixer.Sound` e `Channel(0)`.
- Modelos `Band`, `Song` e `Audio`: entidades de configuracao imutaveis.

Integracoes
- GPIO fisico via `gpiozero.Button`.
- Audio via `pygame.mixer`, SDL2 e stack de audio do Raspberry Pi.
- Arquivos locais WAV.
- Assets locais JPG.

### Decisoes e trade-offs

#### Decisao: manter interface grafica
- **Justificativa:** A selecao visual de bandas, musicas e audios e essencial para o uso do projeto.
- **Trade-off:** O sistema depende de ambiente grafico no Raspberry, portanto nao segue o requisito headless puro.

#### Decisao: usar `pygame.mixer.Sound`
- **Justificativa:** `Sound` carrega WAV em memoria e reduz latencia no disparo.
- **Trade-off:** Consome mais memoria quando comparado a streaming via `pygame.mixer.music`.

#### Decisao: usar um unico canal de reproducao
- **Justificativa:** Simplifica a politica de palco e garante um audio por vez.
- **Trade-off:** Nao permite sobreposicao de samples.

#### Decisao: usar `gpiozero.Button`
- **Justificativa:** API mais simples e alinhada ao Raspberry Pi para pull-up e debounce.
- **Trade-off:** Introduz dependencia direta de `gpiozero` no ambiente de producao.

#### Decisao: nao implementar logs estruturados agora
- **Justificativa:** Reduz complexidade inicial e atende a decisao do projeto.
- **Trade-off:** Diagnostico historico fica limitado a erros exibidos no boot e console.

---

### Dependencias

#### Tecnica: Python 3
Runtime principal da aplicacao.

#### Tecnica: pygame
Reproducao de audio WAV via mixer.

#### Tecnica: gpiozero
Leitura de GPIOs com pull-up e debounce.

#### Tecnica: Pillow
Carregamento e exibicao de imagens no Tkinter.

#### Tecnica: keyboard
Atalhos de teclado para desenvolvimento e operacao alternativa.

#### Tecnica: tkinter
Interface grafica fullscreen. No Raspberry Pi OS pode exigir pacote de sistema `python3-tk`.

#### Externa: interface de audio
Saida de audio USB, HAT, HDMI ou saida analogica configurada no Raspberry.

---

### Riscos e mitigacao

#### Latencia perceptivel no disparo
- **Probabilidade:** media
- **Impacto:** Alto em uso musical ao vivo.
- **Mitigacao:**
  - Usar WAV PCM.
  - Usar `pygame.mixer.Sound`.
  - Pre-carregar audios por musica.
  - Testar buffers 128, 256 e 512 no Raspberry.
- **Plano de contingencia:** Ajustar buffer do mixer ou avaliar biblioteca de audio alternativa.

#### Consumo de memoria com audios longos
- **Probabilidade:** media
- **Impacto:** Pode causar lentidao ou falha em musicas com muitos audios longos.
- **Mitigacao:**
  - Pre-carregar apenas os audios da musica atual.
  - Validar uso com arquivos reais.
- **Plano de contingencia:** Usar `pygame.mixer.music` apenas para backing tracks longas.

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
  - Usar `gpiozero.Button` com `bounce_time=0.03`.
  - Testar footswitches reais.
- **Plano de contingencia:** Aumentar `bounce_time` conforme o hardware.

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
- O sistema usa `pygame.mixer.Sound` e `Channel(0)` para reproducao.
- O sistema usa `gpiozero.Button` com `pull_up=True` e `bounce_time=0.03`.
- O sistema nao depende de caminhos absolutos para audios e assets.

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
- Futuro: teste de inicializacao automatica via systemd.

Estrategia de validacao
- Validar localmente imports, carga de configuracao e regras sem depender de GPIO real.
- Validar no Raspberry Pi a leitura dos footswitches, debounce, saida de audio e latencia percebida.
- Testar pelo menos buffers 128, 256 e 512 se houver latencia ou instabilidade.
- Fazer ensaio com o acervo real de WAVs antes de uso em palco.
