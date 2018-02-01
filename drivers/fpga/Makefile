#
# Makefile for the fpga framework and fpga manager drivers.
#

# Core FPGA Manager Framework
obj-$(CONFIG_FPGA)			+= fpga-mgr.o
obj-$(CONFIG_FPGA_MGR_DEBUG_FS)		+= fpga-mgr-debugfs.o

# FPGA Manager Drivers
obj-$(CONFIG_FPGA_MGR_SOCFPGA)		+= socfpga.o
obj-$(CONFIG_FPGA_MGR_SOCFPGA_A10)	+= socfpga-a10.o

# FPGA Bridge Drivers
obj-$(CONFIG_FPGA_BRIDGE)		+= fpga-bridge.o
obj-$(CONFIG_SOCFPGA_FPGA_BRIDGE)	+= altera-hps2fpga.o altera-fpga2sdram.o
obj-$(CONFIG_ALTERA_FREEZE_BRIDGE)	+= altera-freeze-bridge.o

# High Level Interfaces
obj-$(CONFIG_FPGA_REGION)		+= fpga-region.o

# OpenCl for Altera CyClone V
obj-y					+= aclsoc_drv_14.1/
obj-y					+= aclsoc_drv_16.0/
obj-y					+= aclsoc_drv_16.1/
obj-y					+= c5soc_drv_17.0/
obj-y					+= a10soc_drv_17.0/
obj-y					+= c5soc_drv_17.1/
obj-y					+= a10soc_drv_17.1/

