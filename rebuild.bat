del *.s /Y
del *.o /Y
del *.l /Y
del *.x /Y

gcc -c -O ./src/lib/crt/crt.c -liocs -ldos -lbas
gcc -c -O ./src/lib/dma/dma.c -liocs -ldos -lbas
gcc -c -O ./src/lib/draw/dr_com.c -liocs -ldos -lbas
gcc -c -O ./src/lib/draw/dr_gr.c -liocs -ldos -lbas
gcc -c -O ./src/lib/draw/dr_sp.c -liocs -ldos -lbas
gcc -c -O ./src/lib/draw/dr_tx.c -liocs -ldos -lbas
gcc -c -O ./src/lib/input/input.c -liocs -ldos -lbas
gcc -c -O ./src/lib/timer/timer.c -liocs -ldos -lbas
lib GAMELIB.L crt.o dma.o dr_com.o dr_gr.o dr_sp.o dr_tx.o input.o timer.o

make

del *.o /Y
del *.l /Y

