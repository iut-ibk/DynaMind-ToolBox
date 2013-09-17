DynaMind-Executable is a stand alone executable to dyanmind simulations. It is part of the [DynaMind-Toolbox](https://github.com/iut-ibk/DynaMind-ToolBox) 

use in Linux and MacOSX

~~~
 dynamind --input-file mysimulation.dyn
~~~


Following options are avalible

|option|description|examples|
|------|-------|--|
|help | show options| |
|input-file | set simulation file | |
|replace | replace string in input file: ([STRING]\|[STRING];)* |e.g. \--replace 'CLIMATECHANGE\|cf+0.01; MASTERPLAN\|sprawl.shp|
|repeat | repeat simulation| |
|cpfile | Copy generated files: ([SOURCEFILE],[TARGETPATH]DMITERATION[FILENAME];)*| |
|verbose | verbose output| |
|nodecache | node cache size| |
|attributecache |attribute cache size| |
|sqlquerystack | sql query cache size| |
|blockwriting | sql write block size| |
|loglevel | logger level 0-3 (Debug-Standard-Warning-Error)| |
|ompthreads| number of threads used by omp| |
|settings| set an environment variable|e.g. set path to swmm file \--settings SWMM=parallel-swmm/swmm|
|show-settings| show environment variables| |
