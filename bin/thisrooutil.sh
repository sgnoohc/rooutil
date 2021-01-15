#
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOUTILDIR=$(dirname "$DIR")
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOUTILDIR/lib
export PYTHONPATH=$PYTHONPATH:$ROOUTILDIR/python
export PATH=$ROOUTILDIR/bin:$PATH
