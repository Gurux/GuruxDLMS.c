# Boards compatible with this app 
TARGET_BOARDS := wirepasEFR32Kit 

# Define default network settings
# TODO: Change this. This must be unique for each NIC.
default_network_address ?= 0x123456
default_network_channel ?= 4

# Define a specific application area_id
# TODO: Change this. This SINK ID.
app_specific_area_id=0x83744C

# App version
app_major=1
app_minor=0
app_maintenance=0
app_development=0
