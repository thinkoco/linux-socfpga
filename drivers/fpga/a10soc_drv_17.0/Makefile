aclsoc_drv-objs			:= aclsoc.o aclsoc_fileio.o aclsoc_cmd.o
ccflags-$(CONFIG_INTELFPGA_ARRIA_10_OPENCL_170)	:= -DCONFIG_ALTERA_STAPL
CFLAGS_aclsoc_drv.o		:= -Wno-deprecated-declarations

obj-$(CONFIG_INTELFPGA_ARRIA_10_OPENCL_170)	+= aclsoc_drv.o
