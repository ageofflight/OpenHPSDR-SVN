project -new CIC3_26.prj
add_file CIC3_26.v
set_option -technology VIRTEX2
set_option -part XC2V500
set_option -synthesis_onoff_pragma 0
set_option -frequency auto
project -run synthesis
