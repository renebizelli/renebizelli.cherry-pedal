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

## Iniciar/parar o app

Por enquanto o app roda em primeiro plano, executado manualmente:

```bash
cd ~/renebizelli.cherry-pedal
./build/cherry_pedal
```

`Ctrl+C` interrompe.

> Futuro (fora de escopo por ora, ver PRD): iniciar automaticamente no boot
> via systemd.

---

## Ajustar mapeamento de pinos GPIO ou layout da tela

Veja a conversa/README do projeto — pinos ficam centralizados em
`src/application.cpp` (topo do arquivo), layout visual em
`src/screens/*.cpp`. Qualquer ajuste deve ser feito no codigo-fonte, e o
app recompilado (`cmake --build build -j4`) antes de reiniciar.
