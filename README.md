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
There is also a simple ls utility included within this package.

To use my ls, simply reproduce the commands from above and run `bin/ls` instead of `bin/rshell` to test it out.
For now, -a and -R are working fine (as far as I know) and -l has some kinks to be worked out. Also putting a hard path such as `/` or `/bin` will give it a hard time and cause some problems.

