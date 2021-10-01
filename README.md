# GVShell

## Como compilar e rodar

Para compilar o Shell através de Makefile, utilize o `make`. Exemplo:
```
make
```
Após compilado, basta apenas rodá-lo através do terminal:
```
./gvs
```

Outro comando de utilidade é o `make clean`: 
```
make clean
```
Ele pode ser usado para limpar os arquivos gerados pelo `make`, assim podendo recompilar após alterações.

## Comandos e Processos suportados

Esses são so comandos disponíveis:
- cd: Troca o diretório atual
- ls: Lista os arquivos no diretório
- exit: Encerra o shell
- Comandos gerais encontrados em Shells Unix (cat, clear, etc.)
- Suporte para Pipes

## Detalhes extras

Esse Shell foi desenvolvido utilizando algumas bibliotecas extras para tratamento de input e output. Os arquivos necessários para compilação são apenas o ``gvs.c`` e ``Makefile``. Ele foi implementado no Windows através do Subsistema Linux (WSL2).