# Component Makefile

BMELIB = bme280
LSMLIB = lsm303

COMPONENT_SRCDIRS := . \
    $(BMELIB) \
    $(BMELIB)/selftest \
    $(LSMLIB) \

COMPONENT_ADD_INCLUDEDIRS := . \
    $(BMELIB) \
    $(BMELIB)/selftest \
    $(LSMLIB) \

COMPONENT_PRIV_INCLUDEDIRS +=  . \
    $(BMELIB) \
    $(BMELIB)/selftest \
    $(LSMLIB) \
