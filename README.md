# Computação Distribuída

Código fonte do trabalho prático realizado no semestre 2018.1 para a disciplina de Computação Distribuída (INE5418).

Enunciado:
> ### Implementação de Exclusão Mútua em Sistemas Distribuídos
>
> Implementar os três algoritmos de exclusão mútua estudados na disciplina:
>
> * Baseado em Anel (passagem de token);
>
> * Baseado em Servidor / Coordenador;
>
> * Baseado em Multicast e Relógios Lógicos.

## Instruções de uso

Requisitos:
* OpenMPI

Instalação em sistemas Ubuntu:

```sudo apt install openmpi-bin libopenmpi-dev```

Compilar com: ```make P=nome_do_arquivo```. 

Opções:
* ```make install```: instala OpenMPI para sistemas Ubuntu.
* ```make oversubscribe P=nome_do_arquivo```: para casos em que o sistema informa que não há slots suficientes para o número de processos definido.
* Descomentar a flag DEBUG no arquivo Makefile para mais informações na execução.
* Para alterar número de processos, altere o número da flag ```-np``` no arquivo Makefile.
