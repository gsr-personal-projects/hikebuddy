#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

ifndef CONFIG_LVGL_GUI_ENABLE
COMPONENT_ADD_INCLUDEDIRS := 
COMPONENT_SRCDIRS := 
else
LVGLLIB = lvgl

COMPONENT_SRCDIRS := . \
    ./include \
    $(LVGLLIB) \
    $(LVGLLIB)/lv_core \
    $(LVGLLIB)/lv_draw \
    $(LVGLLIB)/lv_hal \
    $(LVGLLIB)/lv_misc \
    $(LVGLLIB)/lv_fonts \
    $(LVGLLIB)/lv_objx \
    $(LVGLLIB)/lv_themes \
    lvgl_examples \
    fonts

COMPONENT_ADD_INCLUDEDIRS := . \
    ./include \
    $(LVGLLIB) \
    $(LVGLLIB)/lv_core \
    $(LVGLLIB)/lv_draw \
    $(LVGLLIB)/lv_hal \
    $(LVGLLIB)/lv_misc \
    $(LVGLLIB)/lv_fonts \
    $(LVGLLIB)/lv_objx \
    $(LVGLLIB)/lv_themes \
    lvgl_examples \
    fonts

COMPONENT_PRIV_INCLUDEDIRS +=  . \
    ./include \
    $(LVGLLIB) \
    $(LVGLLIB)/lv_core \
    $(LVGLLIB)/lv_draw \
    $(LVGLLIB)/lv_hal \
    $(LVGLLIB)/lv_misc \
    $(LVGLLIB)/lv_fonts \
    $(LVGLLIB)/lv_objx \
    $(LVGLLIB)/lv_themes \
    lvgl_examples \
    fonts

endif  #CONFIG_LVGL_GUI_ENABLE
