project -new HB_2.prj
add_file HB_2.v
set_option -technology VIRTEX2
set_option -part XC2V500
set_option -synthesis_onoff_pragma 0
set_option -frequency auto
project -run synthesis
