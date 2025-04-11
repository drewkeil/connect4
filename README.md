This is a weak solver for the game Connect 4. Given any position, it is capable of finding the best move assuming both sides play optimally. This is considered a weak solver because it evaluates
any win/draw/loss as equivalent to any other win/draw/loss. A strong solver would prefer an earlier win to a later one while preferring later draws and losses.

A majority of the optimizations implemented in this solver were learned from the [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page), 
with some Connect 4 specific optimizations taken from [this guide](http://blog.gamesolver.org/) by Pascal Pons.

