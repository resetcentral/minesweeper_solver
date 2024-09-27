# Minesweeper Solver
[![Coverage Status](https://coveralls.io/repos/github/resetcentral/minesweeper_solver/badge.svg)](https://coveralls.io/github/resetcentral/minesweeper_solver)

A console program that generates random Minesweeper games and then solves them.

```
1 1 1             1 2 3 f 1       1 2 2 2 1 1         1 1 1 
1 f 1             1 f f 2 1   1 1 2 f f 2 f 1   1 1 1 1 f 1 
2 2 1 1 1 1       1 2 2 1     1 f 2 2 2 2 1 1   1 f 2 2 1 1 
f 2 1 2 f 1 1 1 1 1 1 1   1 1 2 1 2 1 1         1 2 f 1     
1 2 f 3 2 3 2 f 2 2 f 1   1 f 1   1 f 1           1 1 1     
  1 1 2 f 2 f 4 f 3 1 1   1 1 1   1 1 2 1 1     1 1 1       
  1 1 2 1 3 2 4 f 2                   1 f 1     1 f 1       
  1 f 2 1 1 f 2 2 2 1                 1 1 1     1 1 1       
1 2 4 f 2 1 1 1 1 f 2 1     1 1 1         1 1 1   1 1 1     
1 f 3 f 2       2 3 f 1     1 f 1         1 f 1 1 2 f 1     
1 1 2 1 1       1 f 2 1   1 2 2 1     1 1 2 1 2 2 f 2 1     
  1 1 1     1 2 4 3 2 1 1 2 f 1 1 1 1 1 f 1   1 f 2 1       
1 2 f 1     1 f f f 2 2 f 3 2 2 1 f 2 2 1 2 1 2 1 1   1 1 1 
1 f 2 1     2 4 f 3 3 f 3 2 f 1 2 3 f 1   1 f 1       2 f 2 
1 1 1     1 2 f 2 1 3 f 4 2 1 1 1 f 2 1   1 1 2 1 1   2 f 2 
          1 f 2 1   2 f f 1     1 1 1         1 f 1   1 1 1 

Minefield Swept!
```

## Build
Dependencies:
- This project depends on boost v1.86 (specifically `boost::rational`)

Build:
```
cmake build -B build
cmake --build build --target runner
```

## Usage
Run:
`./build/runner`