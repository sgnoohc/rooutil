# RooUtil

## Quick start

    > git clone git@github.com:sgnoohc/rooutil.git
    > source rooutil/thisrooutil.sh
    > makeclass.sh -x /path/to/your/baby.root   t    wwwtree  tas  www
                                                ^    ^^^^^^^  ^^^  ^^^
      argument 1 : tree name
      argument 2 : make class output file name (e.g. wwwtree.cc, wwwtree.h)
      argument 3 : name of the namespace defined in make class .cc/.h
      argument 4 : name of the global instance defined in make class .cc/.h

    > make # First time compilation should compile "wwwtree".cc along with any rooutil related stuff. Next time will be faster if only process.cc is touched
    > ./doAnalysis /path/to/your/baby.root test [NEVENTS=-1]
