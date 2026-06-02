BB68.x: main.o ball.o shot.o racket.o block.o item.o numfont.o score.o count.o stage.o sTitle.o sPlay.o sResult.o 
	gcc -s -O -o BB68.x -lzm -liocs -ldos -lbas -lgame main.o ball.o shot.o racket.o block.o item.o numfont.o score.o count.o stage.o sTitle.o sPlay.o sResult.o

main.o: ./src/main.c
	gcc -c -O ./src/main.c -liocs -ldos -lbas

ball.o: ./src/object/ball.c
	gcc -c -O ./src/object/ball.c -liocs -ldos -lbas

shot.o: ./src/object/shot.c
	gcc -c -O ./src/object/shot.c -liocs -ldos -lbas

racket.o: ./src/object/racket.c
	gcc -c -O ./src/object/racket.c -liocs -ldos -lbas

block.o: ./src/object/block.c
	gcc -c -O ./src/object/block.c -liocs -ldos -lbas

item.o: ./src/object/item.c
	gcc -c -O ./src/object/item.c -liocs -ldos -lbas

numfont.o: ./src/object/numfont.c
	gcc -c -O ./src/object/numfont.c -liocs -ldos -lbas

score.o: ./src/object/score.c
	gcc -c -O ./src/object/score.c -liocs -ldos -lbas

count.o: ./src/object/count.c
	gcc -c -O ./src/object/count.c -liocs -ldos -lbas

stage.o: ./src/object/stage.c
	gcc -c -O ./src/object/stage.c -liocs -ldos -lbas

sTitle.o: ./src/scene/sTitle.c
	gcc -c -O ./src/scene/sTitle.c -liocs -ldos -lbas

sPlay.o: ./src/scene/sPlay.c
	gcc -c -O ./src/scene/sPlay.c -liocs -ldos -lbas

sResult.o: ./src/scene/sResult.c
	gcc -c -O ./src/scene/sResult.c -liocs -ldos -lbas
