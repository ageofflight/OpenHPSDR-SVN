project -new CIC8_130.prj
add_file CIC8_130.v
set_option -technology VIRTEX2
set_option -part XC2V500
set_option -synthesis_onoff_pragma 0
set_option -frequency auto
project -run synthesis
