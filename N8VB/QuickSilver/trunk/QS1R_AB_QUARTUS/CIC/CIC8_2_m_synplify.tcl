project -new CIC8_2_m.prj
add_file CIC8_2_m.v
set_option -technology VIRTEX2
set_option -part XC2V500
set_option -synthesis_onoff_pragma 0
set_option -frequency auto
project -run synthesis
