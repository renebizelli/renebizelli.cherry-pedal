# Operação do Cherry Pedal no Raspberry Pi

Guia de uso do dia a dia: conectar no Pi, subir uma nova versão do app, e
adicionar músicas/bandas novas. Para instruções de build, veja
[`BUILD.md`](BUILD.md).

---

## Conectar via SSH

O acesso já está configurado por chave (par `cherry-pedal-access`; a chave
publica ja foi autorizada no Pi — ver `claude-session.txt` para o comando
usado).

```bash
ssh -i ~/.ssh/raspberry_pedal pi@192.168.15.5
```

IP e usuario podem mudar se a rede/config do Pi mudar — confirme com quem
administra o equipamento se a conexao falhar. Se a chave privada
`~/.ssh/raspberry_pedal` nao existir na maquina que voce esta usando, ela
precisa ser gerada e a chave publica correspondente autorizada em
`~/.ssh/authorized_keys` no Pi (mesmo processo do `claude-session.txt`).

---

## Subir uma nova versao do app

### Se o Pi tem o repositorio clonado (recomendado)

```bash
ssh -i ~/.ssh/raspberry_pedal pi@192.168.15.5
cd ~/renebizelli.cherry-pedal
git pull
cmake --build build -j4
```

Reinicie o app (veja "Iniciar/parar o app" abaixo) para carregar o binario
novo.

### Se preferir so copiar um binario pre-compilado (cross-build feito fora do Pi)

```bash
scp build/cherry_pedal pi@192.168.15.5:~/renebizelli.cherry-pedal/build/cherry_pedal
```

---

## Adicionar uma musica nova a uma banda existente

1. Copie o(s) arquivo(s) WAV para dentro de `bands/<id-da-banda>/<id-da-musica>/`,
   criando as pastas se necessario. Exemplo, uma nova musica `encore` para a
   banda `ask`:

   ```
   bands/ask/encore/encore.wav
   ```

2. Edite `source.json` e adicione uma entrada na lista da banda
   correspondente (a chave do JSON com o `id` da banda, ex. `"ask"`):

   ```json
   {
     "name": "Encore",
     "id": "encore",
     "autoforward": false,
     "audios": [
       { "file": "encore.wav", "name": "audio" }
     ]
   }
   ```

   - `id`: identificador unico da musica dentro da banda — tem que bater
     com o nome da pasta em `bands/<banda>/`.
   - `name`: o que aparece na tela.
   - `autoforward`: `true` se, ao terminar um audio, o painel deve avancar
     sozinho para o proximo audio da lista (sem tocar automaticamente —
     so troca a selecao). `false` para navegacao manual.
   - `audios`: lista de arquivos WAV dessa musica, na ordem em que aparecem
     no painel. Cada um tem `file` (nome do arquivo dentro da pasta da
     musica) e `name` (rotulo exibido).

3. Envie os arquivos novos e o `source.json` atualizado para o Pi (via
   `git push`/`git pull`, ou `rsync`/`scp` direto) e reinicie o app.

O app valida todos os WAVs configurados na inicializacao — se algum
arquivo estiver faltando ou o caminho errado, ele mostra o erro e nao abre
o painel.

---

## Adicionar uma banda nova

1. Crie a pasta `bands/<id-da-banda>/` com uma subpasta por musica, cada
   uma com seus WAVs (mesmo padrao acima).

2. (Opcional) Adicione um logo em `assets/bands/<id-da-banda>.jpg`. Sem
   logo, a banda aparece como um botao com o nome em texto.

3. Edite `source.json`:

   - Adicione a banda na lista `"bands"`:

     ```json
     { "id": "nova-banda", "name": "Nova Banda", "logo": true }
     ```

     (`"logo": false` se nao for adicionar `assets/bands/nova-banda.jpg`.)

   - Adicione uma nova chave no JSON com esse mesmo `id`, contendo a lista
     de musicas da banda (mesmo formato da secao anterior):

     ```json
     "nova-banda": [
       {
         "name": "Primeira musica",
         "id": "primeira-musica",
         "autoforward": false,
         "audios": [
           { "file": "intro.wav", "name": "audio" }
         ]
       }
     ]
     ```

4. Envie tudo para o Pi e reinicie o app.

---

## Calibrar a touchscreen (toque não acerta o que deveria)

O app tem suporte a clique/toque (ex.: tocar no nome da banda no painel
volta para a tela de selecao). Se o toque estiver acertando um ponto
diferente de onde voce encostou (comum em telas resistivas como a
ADS7846, que precisam de calibracao apos instalar o sistema), o problema
e de calibracao do X11, nao do app.

```bash
sudo apt install -y xinput-calibrator
DISPLAY=:0 xinput_calibrator
```

Siga os pontos exibidos na tela tocando fisicamente em cada um. Ao final,
a ferramenta imprime um bloco de configuracao (`Section "InputClass"...`).
Salve esse bloco em:

```bash
sudo nano /etc/X11/xorg.conf.d/99-calibration.conf
```

Reinicie a sessao grafica (ou o Pi) para aplicar.

---

## Iniciar/parar o app manualmente

```bash
cd ~/renebizelli.cherry-pedal
./build/cherry_pedal
```

`Ctrl+C` interrompe. Para matar uma instancia rodando em segundo plano:

```bash
killall cherry_pedal
```

(evite `pkill -f cherry_pedal` por SSH — o proprio comando SSH carrega
"cherry_pedal" na sua linha de comando e pode se automatar/derrubar a
propria sessao.)

---

## Inicio automatico ao ligar a Raspberry

O Pi ja faz autologin do usuario `pi` direto no desktop grafico
(LXDE/PIXEL). O Cherry Pedal foi adicionado ao autostart dessa sessao:

```
~/.config/autostart/cherry-pedal.desktop
```

Ou seja: **basta ligar a Raspberry** que o app abre sozinho (com um
atraso de 3s para dar tempo do desktop/audio inicializarem) — nao
precisa de SSH nem de digitar nada.

### Desativar o autostart (para desenvolvimento/teste de uma versao nova)

Renomeie ou mova o arquivo temporariamente:

```bash
mv ~/.config/autostart/cherry-pedal.desktop ~/.config/autostart/cherry-pedal.desktop.disabled
```

E para reativar:

```bash
mv ~/.config/autostart/cherry-pedal.desktop.disabled ~/.config/autostart/cherry-pedal.desktop
```

Reinicie a sessao grafica (ou o Pi) para que a mudanca tenha efeito — o
autostart so roda no login, nao ao editar o arquivo com o app ja aberto.

> Futuro (se algum dia for necessario rodar sem desktop grafico): migrar
> para um servico systemd dedicado em vez do autostart do LXDE.

---

## Ajustar mapeamento de pinos GPIO ou layout da tela

Veja a conversa/README do projeto — pinos ficam centralizados em
`src/application.cpp` (topo do arquivo), layout visual em
`src/screens/*.cpp`. Qualquer ajuste deve ser feito no codigo-fonte, e o
app recompilado (`cmake --build build -j4`) antes de reiniciar.
