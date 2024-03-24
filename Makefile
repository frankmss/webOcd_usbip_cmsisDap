# @author frank (frank.mss@proton.me)

NAME := webOcd_usbip_cmsisDap

#------------------------------------------------#
#   INGREDIENTS                                  #
#------------------------------------------------#
# SRC_DIR   source directory
# OBJ_DIR   object directory
# SRCS      source files
# OBJS      object files
#
# CC        compiler
# CFLAGS    compiler flags
# CPPFLAGS  preprocessor flags

SRC_DIR     := src
OBJ_DIR     := obj
# SRCS        := \
# 	forTest.c 
SRCS				:= \
	main.c \
	axiFFJtag.c \
	vstub/vstubmod.c \
	vstub/vstub_list.c \
	vstub/vstub_net.c \
	vstub/usbip_pkt.c \
	vstub/cmsis-dap-usbip.c \
	free-dap/dap.c \
	vstub_main.c
	
	

SRCS        := $(SRCS:%=$(SRC_DIR)/%)
OBJS        := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# INC_DIR     := src/3thpart/mongoose  src/sdl_thread_lib
INC_DIR     := $(SRC_DIR) $(SRC_DIR)/vstub $(SRC_DIR)/free-dap
CC          := gcc
# CFLAGS      := -Wall -Wextra -Werror -std=gnu99 
LKFLAGS     := -lSDL2 -lpthread
# CFLAGS      := -Wall -Wextra -std=gnu99 -DLINUX 
CFLAGS      := -Wextra -std=gnu99 -DLINUX
CPPFLAGS    := -I $(INC_DIR)

#------------------------------------------------#
#   UTENSILS                                     #
#------------------------------------------------#
# RM        cleaning command
# MAKE      make command

RM          := rm --force
MAKE        := make --no-print-directory

#------------------------------------------------#
#   RECIPES                                      #
#------------------------------------------------#
# all       default goal
# %.o       compilation .c -> .o
# $(NAME)   linking .o -> binary
# clean     remove .o
# fclean    remove .o + binary
# re        remake default goal

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $^ -o $@ $(LKFLAGS)
	echo "CREATED $(NAME)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	-[ ! -d $(@D) ] && mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	echo "CREATED $@"

clean:
	$(RM) --recursive $(OBJ_DIR)
	echo "rm --recursive " $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)
	echo "rm " $(NAME)


re:
	$(MAKE) fclean
	$(MAKE) all

#------------------------------------------------#
#   SPEC                                         #
#------------------------------------------------#

.PHONY: clean fclean re
.SILENT:

####################################### END_3 ####

