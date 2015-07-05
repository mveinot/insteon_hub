insteon_hub
===========

Command line control of the Insteon Hub

This utility is used to send PLM commands to a networked Insteon Hub that will send the corresponding Insteon control commands over the dual-mesh network.

It makes use of GNU getopt which must be installed in a more-or-less standard location.

It also uses tinyxml2 (included) under the ZLib license, permitting open source or commercial use.

An example config file is provided in insteon_hub.config

Edit it to suit your needs and copy or move it to ~/.insteon_hub

As of version 0.10.0, device names can be specified in the .insteon_hub file in a <devices> section.
See the example for details. Device types are as follows:

90 = relay
91 = dimmer
92 = scene/group (untested)
93 = thermostat (untested)
