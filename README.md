insteon_hub
===========

Command line control of the Insteon Hub

This utility is used to send HTTP commands to a networked Insteon Hub that will send the corresponding Insteon control commands over the dual-mesh network.

It makes use of GNU getopt and libcurl which must be installed in more-or-less standard locations.

It also uses tinyxml2 (included) under the ZLib license, permitting open source or commercial use.

An example config file is provided in insteon_hub.config

Edit it to suit your needs and copy or move it to ~/.insteon_hub
