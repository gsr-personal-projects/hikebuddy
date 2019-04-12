# Component Makefile

BMELIB = bme280

COMPONENT_SRCDIRS := . \
    $(BMELIB) \
    $(BMELIB)/selftest \

COMPONENT_ADD_INCLUDEDIRS := . \
    $(BMELIB) \
    $(BMELIB)/selftest \

COMPONENT_PRIV_INCLUDEDIRS +=  . \
    $(BMELIB) \
    $(BMELIB)/selftest \
