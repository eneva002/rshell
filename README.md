# rshell
hw0 --- `rshell`

The `rshell` program is a shell designed to mimic most modern day shells.

It should read and execute commands on an individual basis, as well as when combined using connectors as listed below.

```
cmd            = executable [argumentList] [connector cmd]
connector      = || or && or ;
```

To compile and run my shell you must simply clone my repo, and type the following:

```
cd rshell
make
git checkout hw0
make
bin/rshell
``` 
