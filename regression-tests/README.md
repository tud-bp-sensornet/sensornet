Manually edit the following lines inside the .csc files:
<source EXPORT="discard">[CONTIKI_DIR]/examples/rime/example-broadcast.c</source>
<commands EXPORT="discard">make example-broadcast.sky TARGET=sky</commands>
<firmware EXPORT="copy">[CONTIKI_DIR]/examples/rime/example-broadcast.sky</firmware>
