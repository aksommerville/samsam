all:
.SILENT:
.SECONDARY:
PRECMD=echo "  $@" ; mkdir -p "$(@D)" ;

ifneq (,$(strip $(filter clean,$(MAKECMDGOALS))))
clean:;rm -rf mid out
else

OPT_ENABLE:=stdlib graf text rom
PROJNAME:=samsam
PROJRDNS:=com.aksommerville.egggame.samsam
ENABLE_SERVER_AUDIO:=
BUILDABLE_DATA_TYPES:=

ifndef EGG_SDK
  EGG_SDK:=/home/andy/proj/egg
endif

include $(EGG_SDK)/etc/tool/common.mk

endif
