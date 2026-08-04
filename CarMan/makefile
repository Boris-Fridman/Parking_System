# ----------------------------------------------------------------------------------------------------------------------
# 
#   Terminal Colors
#  
#   The information about colors can be found in the sine:
#   https://en.wikipedia.org/wiki/ANSI_escape_code
#  
# ----------------------------------------------------------------------------------------------------------------------


#   Foreground Colors                                                                                                   
TermBlack           = "\\033[30m"
TermRed             = "\\033[31m"
TermGreen           = "\\033[32m"
TermYello           = "\\033[33m"
TermBlue            = "\\033[34m"
TermMagenta         = "\\033[35m"
TermCyan            = "\\033[36m"
TermWhite           = "\\033[37m"#  (Light Gray)

TermBrightBlack     = "\\033[90m"#  (Dark Gray)
TermBrightRed       = "\\033[91m"
TermBrightGreen     = "\\033[92m"
TermBrightYello     = "\\033[93m"
TermBrightBlue      = "\\033[94m"
TermBrightMagenta   = "\\033[95m"
TermBrightCyan      = "\\033[96m"
TermBrightWhite     = "\\033[97m"

# ----------------------------------------------------------------------------------------------------------------------
#  Background Colors                                                                                                   
TermBGBlack         = "\\033[40m"
TermBGRed           = "\\033[41m"
TermBGGreen         = "\\033[42m"
TermBGYello         = "\\033[43m"
TermBGBlue          = "\\033[44m"
TermBGMagenta       = "\\033[45m"
TermBGCyan          = "\\033[46m"
TermBGWhite         = "\\033[47m"#  (Light Gray)

TermBGBrightBlack   = "\\033[100m"# (Dark Gray)
TermBGBrightRed     = "\\033[101m"
TermBGBrightGreen   = "\\033[102m"
TermBGBrightYello   = "\\033[103m"
TermBGBrightBlue    = "\\033[104m"
TermBGBrightMagenta = "\\033[105m"
TermBGBrightCyan    = "\\033[106m"
TermBGBrightWhite   = "\\033[107m"


TermColorsReset     = "\\033[39\;49m"

MAKEFLAGS += --no-print-directory

OUT_DIR ?= .

CXX = gcc
CXXARM = arm-linux-gnueabihf-gcc

COLORSENABLE = -fdiagnostics-color=always

#Allow debugging:
DEBUGEN = -g
#Define Optimization:
OPTLEVEL = -O2
#Standard:
STANDARD = -std=c99
#Enforce standard (no compiler specific features allowed):
ENFORCESTANDARD = -pedantic
#Warning flags:
WARNINGFLAGS = -Wall -Wextra

#Definitions:
 #Unlock GNU/Linux extensions, POSIX standards, and non-standard functions in the GNU C Library (glibc).
DEFINES = -D_GNU_SOURCE

#source files - use all top level .c files:
SRCS = \
*.c                                      \
../Common_Libs/CommonData.c              \
../Common_Libs/Configuration/ConfMan.c

TARGNAME = carman
EXT = .elf
TARGET = "$(OUT_DIR)/$(TARGNAME)$(EXT)"
ARMTARGET = "$(OUT_DIR)/$(TARGNAME)-arm$(EXT)"

#Includes:
INCLUDES = -I.\
-I./../Common_Libs                       \
-I./../Common_Libs/Configuration               

#Link math lib:
LINKMATH = -lm


#Implementing database:
COMPFLAGS    = $(COLORSENABLE) $(STANDARD) $(ENFORCESTANDARD) $(WARNINGFLAGS) $(DEFINES) $(SRCS) -o $(TARGET) $(INCLUDES) $(LINKMATH)
ARMCOMPFLAGS = $(COLORSENABLE) $(STANDARD) $(ENFORCESTANDARD) $(WARNINGFLAGS) $(DEFINES) $(SRCS) -o $(ARMTARGET) $(INCLUDES) $(LINKMATH)

#OBJS = $(SRCS:.c=.o)

# Default target that executes when you type 'make'
all:
	@$(MAKE) release
	@$(MAKE) crossrelease

debug: compdebug
compdebug:
	@echo "$(TermYello)Running Debug Compilation$(TermColorsReset)"
	@$(CXX) -g -O0 -DDEBUG $(COMPFLAGS);
debug: showresult	

release: comprelease
comprelease: 
	@echo "$(TermYello)Running Release Compilation$(TermColorsReset)"
	@$(CXX) -O3 -Werror -DRELEASE $(COMPFLAGS)
release: showresult


crossdebug: compcrossdebug
compcrossdebug:
	@echo "$(TermYello)Running Debug Arm Specific Compilation$(TermColorsReset)"
	@$(CXXARM) -g -O0 -DDEBUG $(ARMCOMPFLAGS);
crossdebug: showresult	

crossrelease: compcrossrelease
compcrossrelease: 
	@echo "$(TermYello)Running Release Arm Specific Compilation$(TermColorsReset)"
	@$(CXXARM) -O3 -Werror -DRELEASE $(ARMCOMPFLAGS)
crossrelease: showresult


showfinalresult: showresult
showresult:
	@if [ $$? -eq 0 ]; then \
		echo "$(TermBrightGreen)Success!$(TermColorsReset)"; \
	else \
		echo "$(TermBrightRed)Failed.$(TermColorsReset)"; \
	fi



clean:
	rm -f $(TARGET)
