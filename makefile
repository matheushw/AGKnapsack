#=======================================================
# Makefile com comandos de compilação, execução e utilidades relacionadas ao programa
# make all: comando principal de compilação, que cria o executável
# make run: executa o programa
# make *.o: cria um dos arquivos objeto a partir da fonte
# make clean: remove todos os arquivos objeto do diretório atual
# make rebuild: como make all, mas recompila todos os arquivos objeto no diretório
# make zip: cria o arquivo zip para a submissão
#=======================================================

objects = baseline.o knap.o main.o
flags = -g

all: $(objects) 
	gcc *.o -o main $(flags)

run: all
	./main 

.c.o: $*.c
	gcc $*.c -c $(flags)

clean:
	rm *.o -f
	rm main -f
	rm zip -fr
	rm *.zip -f

FORCE:

zip: FORCE
	rm code.zip -f
	mkdir zip -p
	rm zip/* -f
	cp *.c zip
	cp *.h zip
	cp makefile zip 
	zip code.zip zip/* -j
	

rebuild:
	make clean
	make all


