# Usage
Place a file named `output.prellvm` at the root of the project. <br>
Run `prellvm` in the project directory. the llvm language code will be displayed in stderr, additional debug info will be printed in stdout. <br>
Example command: `./main  2>output.ll && llc output.ll && gcc output.s` make compile will execute the same command. <br>

# Prerequisites
`llvm`
`llc`
`gcc`

# prellvm syntax
prellvm is agnostic to indentation, newlines, and whitespace. <br>
there isn't comment. <br>
for more information consult `prellvm.azur` inside the standard library. <br>

# Ressources used
LLVM tutorial at: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/
