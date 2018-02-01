aclsoc_drv-objs			:= aclsoc.o aclsoc_fileio.o aclsoc_cmd.o
ccflags-$(CONFIG_ALTERA_OPENCL_161)	:= -DCONFIG_ALTERA_STAPL
CFLAGS_aclsoc_drv.o			:= -Wno-deprecated-declarations

obj-$(CONFIG_ALTERA_OPENCL_161)		+= aclsoc_drv.o
