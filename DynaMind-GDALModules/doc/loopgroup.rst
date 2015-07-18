==========
Loop Group
==========

:index:`LoopGroup <GDALModules; LoopGroup | Grouping and repeatedly executing grouped modules >` can be used to group modules
and groups. Further it allows modules inside the group to be repeatedly executed and iteratively change data. The loop group itself does not manipulate any data.

Parameter
---------

+-------------------+------------------------+--------------------------------------------------------------------------+
|        Name       |          Type          |       Description                                                        |
+===================+========================+==========================================================================+
|Runs               | INT                    | number of runs                                                           |
+-------------------+------------------------+--------------------------------------------------------------------------+
|writeStreams       | STRING_LIST            | data streams that are iteratively changed                                |
+-------------------+------------------------+--------------------------------------------------------------------------+
|readStreams        | STRING_LIST            | data streams that are not iteratively changed                            |
+-------------------+------------------------+--------------------------------------------------------------------------+

Data-stream
-----------
The LoopGroup itself does not manipulate any data but takes care that the right data are passed to the modules.
The LoopGroup distinguishes between two modes, read and write.

- read mode: data are not iteratively processed and for the timestep t+1 the original data-stream is used.
- write mode: data processed by the group are used as input for the group at timestep t+1






