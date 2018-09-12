
Project template for Nordic Semi nRF5 series MCUs
=================================================

This repository contains a simple project template for
Nordic Semi nRF5x series CPUs.

It depends on:

 * the Nordic Semi nrfx driver submodule
 * the ARM CMSIS submodule
 * included CMSIS Configuration Wizard (copied from the latest nRF5 SDK, upstream seems to be `https://sourceforge.net/p/cmsisconfig/activity/`)

Also, in this branch:

 * the Nordic Semi nRF 802.15.4 radio driver submodule


Cloning this project
--------------------

As submodules are used, you have to use `git clone --recursive`
or, after cloning, `git submodule init && git submodule update`.


Using this project as a template
--------------------------------

If you want to adapt this template to your (new) project
then follow the following steps:

In the Makefile, change:

 * `PROJECT_NAME`
 * depending on your target platform, optionally `CPU`, `CPUDEFINE` and `LINKER_SCRIPT`
 * `SRC_FILES`
 * optionally more

Also:

 * move `project.c` to the new name of your project
 * optionally pick the correct template headers `nrfx_glue.h`, `nrfx_log.h` and `nrfx_config.h` from `nrfx/templates/`
 * optionally run `make nrfx_config` to configure the nordic nrfx submodule.


