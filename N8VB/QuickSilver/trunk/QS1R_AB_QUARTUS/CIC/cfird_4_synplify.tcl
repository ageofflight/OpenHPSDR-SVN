project -new cfird_4.prj
add_file cfird_4.v
set_option -technology VIRTEX2
set_option -part XC2V500
set_option -synthesis_onoff_pragma 0
set_option -frequency auto
project -run synthesis
