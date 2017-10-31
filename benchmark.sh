#!/bin/bash

echo "[----ref2.jpg cmp2a.jpg----]"
./TwoPlayerTetris imgs/ref2.jpg imgs/cmp2a.jpg $1 $2
echo "[----ref2.jpg cmp2b.jpg----]"
./TwoPlayerTetris imgs/ref2.jpg imgs/cmp2b.jpg $1 $2
echo "[----ref2.jpg cmp2c.jpg----]"
./TwoPlayerTetris imgs/ref2.jpg imgs/cmp2c.jpg $1 $2
echo "[----ref2.jpg cmp2d.jpg----]"
./TwoPlayerTetris imgs/ref2.jpg imgs/cmp2d.jpg $1 $2

# end of script
