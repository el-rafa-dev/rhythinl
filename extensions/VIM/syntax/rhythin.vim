" Vim syntax file for Rhythin language
" Maintainer: El-Rafa-Dev <rafaelsousa12365@gmail.com>
" Last Change: 2025 Jul 02 (Updated by AI)

" Prevents loading the syntax file multiple times
if exists("b:current_syntax")
  finish
endif

" Set the current syntax to 'rhythin'
let b:current_syntax = "rhythin"

" --- Comments ---
" Line comments starting with #
syntax match rhythinComment "#.*$"
" Line comments starting with ;
syntax match rhythinComment ";.*$"

" --- Strings ---
" Double-quoted strings
" Note: Vim's regex for strings needs to handle escaped quotes carefully.
" The skip=/\\./ handles any escaped character inside the string.
syntax region rhythinString start=/"/ skip=/\\./ end=/"/

" String interpolation (e.g., $[...]) - This highlights the delimiters.
" For full nested expression highlighting inside, more advanced Vim scripting
" or a very complex regex would be needed, which is beyond a simple syntax file.
syntax match rhythinInterpolationDelimiter "\$\["
syntax match rhythinInterpolationDelimiter "\]"

" --- Numbers ---
" Integers and floats (e.g., 123, 123.45) with optional float suffix (f, F)
syntax match rhythinNumber "\<\d\+\(\.\d\+\)\=\([fF]\)\?\>"

" --- Operators ---
" Assignment operators (e.g., :=, +=, -=)
syntax match rhythinOperatorAssignment ":=\|\+=\|-\="
" Arithmetic operators (e.g., +, -, *, /, %)
" Corrected: Special regex characters (+, *, /) are escaped.
syntax match rhythinOperatorArithmetic "\+|\-|\*|\/|%"
" Logical operators (e.g., &&, ||, !)
syntax match rhythinOperatorLogical "&&\|||\|!"
" Comparison operators (e.g., ==, !=, <=, >=, <, >)
" Corrected: Removed '%' if it's not a literal part of your comparison operators.
" If your language *does* use '%<=' or '%>=', you would need to add back '%\<=' and '%\>='.
syntax match rhythinOperatorComparison "==\|!=\|<=\|>=|<\|>"
" Arrow operators (e.g., ->, >>, >)
" Corrected: '>' at the end is fine as is.
syntax match rhythinOperatorArrow "->\|>>\|>"
" Other operators (e.g., :)
syntax match rhythinOperatorOther ":"

" --- Keywords and Types ---
" Control keywords
syntax keyword rhythinControl try catch if but loop return def makedef using from get in local global and or lenght switch case default stop continue jmpcase true false signed unsigned parallel

" Storage types
syntax keyword rhythinType func charseq int32 int64 float32 float64 bool byte obj struct

" --- Identifiers ---
" Basic identifiers (variables)
" This will be linked to a generic Variable highlight group,
" if not overridden by more specific groups.
syntax match rhythinIdentifier "\<[a-zA-Z_][a-zA-Z0-9_]*\>"

" --- Linking to standard Vim highlight groups ---
" This tells Vim how to color your custom syntax groups.
" Standard Vim groups: Comment, String, Number, Operator, Keyword, Type, Identifier, Error, Todo, etc.

hi def link rhythinComment          Comment
hi def link rhythinString           String
hi def link rhythinInterpolationDelimiter Special
hi def link rhythinNumber           Number
hi def link rhythinOperatorAssignment Operator
hi def link rhythinOperatorArithmetic Operator
hi def link rhythinOperatorLogical   Operator
hi def link rhythinOperatorComparison Operator
hi def link rhythinOperatorArrow     Operator
hi def link rhythinOperatorOther     Operator
hi def link rhythinControl          Keyword
hi def link rhythinType             Type
hi def link rhythinIdentifier       Identifier