# Lisp Interpreter

## Build & Execute Instructions
- make
- ./Lisp.exe

## Deficiencies
- Lacks error messages and good documentation

## Goals
- Libraries (Bootstrapping)
- have good error messages 
- map & filter
- ":" (cons syntactic sugar - Haskell like)
- "++" append (haskell like)

## Latest
- let (should create a local environment that is tied to a parent environment. lookup should check the most local
environment before checking the parent. It will iterate through parent environments until it finds the variable
or it does not exist)
- let does create a new scope, but the set function still places the variable in the global rho 

## Currently Working On
- let


