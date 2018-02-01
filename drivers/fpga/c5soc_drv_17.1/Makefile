aclsoc_drv-objs				:= aclsoc.o aclsoc_fileio.o aclsoc_cmd.o
ccflags-$(CONFIG_INTELFPGA_CYCLONE_V_OPENCL_171)	:= -DCONFIG_ALTERA_STAPL
CFLAGS_aclsoc_drv.o			:= -Wno-deprecated-declarations

obj-$(CONFIG_INTELFPGA_CYCLONE_V_OPENCL_171)	+= aclsoc_drv.o
